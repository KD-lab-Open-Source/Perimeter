#ifndef __HYPERSPACE_H__
#define __HYPERSPACE_H__

extern const char * KEY_REPLAY_REEL;


void getMissionDescriptionInThePlayReelFile(const char* fname, MissionDescription& md);
bool isCorrectPlayReelFile(const char* fname);


class terHyperSpace
{
public:

	terHyperSpace(class PNetCenter* net_client, MissionDescription& mission);
	virtual ~terHyperSpace();

	//void Start(MissionDescription& mission, const char* fnamePlayReel=NULL);
	bool SingleQuant();
	bool MultiQuant();
	bool PrimaryQuant();

	void SetServerSpeedScale(float scale){ ServerSpeedScale = scale; }

    virtual void ShowInfo() {};
    std::string GetNetInfo();

	virtual bool ReceiveEvent(terEventID event, InOutNetComBuffer& in);

	virtual void receiveCommand(const netCommand4G_UnitCommand& command)=0;
	virtual void receiveCommand(const netCommand4G_Region& reg)=0;
	virtual bool forcedDefeat(int playerID)=0;


	void sendCommand(netCommandGame* command);

	virtual void Quant(){}

	bool multiPlayer() const { return pNetCenter != 0; }

	size_t getCurrentGameQuant() { return currentQuant; }
	size_t getConfirmQuant() { return confirmQuant; }
protected:
	PNetCenter* pNetCenter; // живет дольше this, !0 == MultiPlayer

private:

	size_t currentQuant;
	//bool flag_endCurQuant;
	size_t lagQuant;
	size_t dropQuant;
	size_t confirmQuant;

	unsigned int signatureGame;

	int  m_nLastPlayerReceived;

	bool netPause;
	bool isNetPause(void) { return netPause; }

	int EmptyCount;

	int TimeOffset;

	int EventTime;
	int EventDeltaTime;
	int EventWaitTime;
	int MaxEventTime;
	int RealMaxEventTime;
	int EventLagHole;

	int AverageEventTime;
	int AverageEventCount;
	int AverageEventSum;
	int AverageDampDelta;

	int StopServerTime;
	int StopServerMode;

	int MaxCorrectionTime;
	float MaxDeltaCorrectionTime;
	float MaxCorrection;

	int MinCorrectionTime;
	float MinDeltaCorrectionTime;
	float MinCorrection;

	float ServerSpeedScale;

//--------------------------
private:
public:
	//Command History
	std::vector<netCommandGame*> fullListGameCommands;
	unsigned int lastQuant_inFullListGameCommands; //Кванты считаются с 1-го!
	unsigned int curGameComPosition;

	std::vector<netCommandGame*> replayListGameCommands;
	unsigned int endQuant_inReplayListGameCommands;
	std::vector<netCommandGame*>::iterator curRePlayPosition;

	static void clearListGameCommands(std::vector<netCommandGame*> & gcList){
		std::vector<netCommandGame*>::iterator p;
		for(p=gcList.begin(); p!=gcList.end(); p++){
			delete *p;
		}
		gcList.clear();
	}

	void stopGame_HostMigrate();

	bool flag_savePlayReel;
	bool flag_rePlayReel;
	bool flag_autoSavePlayReel;

	void sendListGameCommand2Host(unsigned int begQuant, unsigned int endQuant=UINT_MAX);
	void putInputGameCommand2fullListGameCommandAndCheckAllowedRun(netCommandGame* pnc);

    size_t serializeGameCommands(XBuffer& out) const;
    void deserializeGameCommands(XBuffer& in, size_t len);

	bool loadPlayReel(const char* fname);

	enum SAVE_REPLAY_RESULT {
		SAVE_REPLAY_OK,
		SAVE_REPLAY_RW_ERROR_OR_DISK_FULL,
		SAVE_REPLAY_RW_ERROR
	};
	SAVE_REPLAY_RESULT savePlayReel(const char* fname) const;
	void autoSavePlayReel();
	void allSavePlayReel();
	bool flag_stopSavePlayReel;
	void stopPlayReel() { flag_stopSavePlayReel=true; }


	MissionDescription curMission;

	//report log
	struct sLogElement{
		sLogElement(){
			pLog=0;
		}
		sLogElement(int _quant, XBuffer* _plog){
			quant=_quant;
			xassert(_plog);
			pLog=_plog;
		}
		~sLogElement(){
		}
		int quant;
		XBuffer* pLog;
	};
	std::list<sLogElement> logList;
	sLogElement* getLogElement(unsigned int quant){
		std::list<sLogElement>::iterator p;
		for(p=logList.begin(); p!=logList.end(); p++){
			if((*p).quant==quant) return &(*p);
		}
		return 0;
	}
	void pushBackLogList(int quant, XBuffer& xb){
		XBuffer* pb=new XBuffer(xb.tell()+1);
		pb->write(xb.address(), xb.tell());
		logList.push_back(sLogElement(quant, pb));
	}
	XBuffer* getLogInLogList(int quant){
		std::list<sLogElement>::iterator p;
		for(p=logList.begin(); p!=logList.end(); p++){
			if(p->quant==quant) return p->pLog;
		}
		return 0;
	}
	void clearLogList(){
		std::list<sLogElement>::iterator p;
		for(p=logList.begin(); p!=logList.end(); p++){
			if(p->pLog) delete p->pLog;
		}
		logList.clear();
	}
	void eraseLogListUntil(int quant){
		std::list<sLogElement>::iterator p;
		for(p=logList.begin(); p!=logList.end(); ){
			if(p->quant <= quant) {
				if(p->pLog) delete p->pLog;
				p=logList.erase(p);
			}
			else break;
		}
	}
	void writeLogList2Buffer(XBuffer& buf){
		std::list<sLogElement>::iterator p;
		for(p=logList.begin(); p!=logList.end(); p++){
            buf.write(p->pLog->address(), p->pLog->tell());
		}
	}

	size_t clientGeneralCommandCounterInListCommand;// аналог fullListGameCommands.size()
	size_t lastRealizedQuant; //по идее это currentQuant
	size_t allowedRealizingQuant;

	size_t lastQuantAllowedTimeCommand;
	size_t generalCommandCounter4TimeCommand;

	size_t getInternalLagQuant();

	void logQuant();
	void sendLog(unsigned int quant);

	bool flag_HostMigrate;

    SDL_mutex* m_FullListGameCommandLock;

};

#endif //__HYPERSPACE_H__