#ifndef __HYPERSPACE_H__
#define __HYPERSPACE_H__

extern const char * KEY_SAVE_PLAY_REEL;
extern const char * KEY_REPLAY_REEL;


void getMissionDescriptionInThePlayReelFile(const char* fname, MissionDescription& md);
bool isCorrectPlayReelFile(const char* fname);


class terHyperSpace
{
public:

	terHyperSpace(class PNetCenter* net_client, MissionDescription& mission);
	~terHyperSpace();

	//void Start(MissionDescription& mission, const char* fnamePlayReel=NULL);
	bool SingleQuant();
	bool MultiQuant();
	bool PrimaryQuant();

	void SetServerSpeedScale(float scale){ ServerSpeedScale = scale; }

	void ShowInfo();

	virtual bool ReceiveEvent(terEventID event, InOutNetComBuffer& in);

	virtual void receiveCommand(const netCommand4G_UnitCommand& command)=0;
	virtual void receiveCommand(const netCommand4G_Region& reg)=0;
	virtual bool forcedDefeat(int playerID)=0;


	void sendCommand(const netCommand4G_UnitCommand& command);
	void sendCommand(const netCommand4G_Region& command);

	virtual void Quant(){}

	bool multiPlayer() const { return pNetCenter != 0; }

	unsigned long getCurrentGameQuant() { return currentQuant; }
	unsigned long getConfirmQuant() { return confirmQuant; }
protected:
	PNetCenter* pNetCenter; // живет дольше this, !0 == MultiPlayer

private:

	unsigned long currentQuant;
	//bool flag_endCurQuant;
	unsigned long lagQuant;
	unsigned long dropQuant;
	unsigned long confirmQuant;

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
	vector<netCommandGame*> fullListGameCommands;
	unsigned int lastQuant_inFullListGameCommands; //Кванты считаются с 1-го!
	unsigned int curGameComPosition;

	vector<netCommandGame*> replayListGameCommands;
	unsigned int endQuant_inReplayListGameCommands;
	vector<netCommandGame*>::iterator curRePlayPosition;

	static void clearListGameCommands(vector<netCommandGame*> & gcList){
		vector<netCommandGame*>::iterator p;
		for(p=gcList.begin(); p!=gcList.end(); p++){
			delete *p;
		}
		gcList.clear();
	}

	void stopGame_HostMigrate();

	bool flag_savePlayReel;
	bool flag_rePlayReel;
	bool flag_autoSavePlayReel;

	void sendListGameCommand2Host(unsigned int begQuant, unsigned int endQuant=ULONG_MAX);
	void putInputGameCommand2fullListGameCommandAndCheckAllowedRun(netCommandGame* pnc);


	bool loadPlayReel(const char* fname);

	enum SAVE_REPLAY_RESULT {
		SAVE_REPLAY_OK,
		SAVE_REPLAY_RW_ERROR_OR_DISK_FULL,
		SAVE_REPLAY_RW_ERROR
	};
	SAVE_REPLAY_RESULT savePlayReel(const char* fname);
	void autoSavePlayReel(void);
	void allSavePlayReel(void);
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
	list<sLogElement> logList;
	sLogElement* getLogElement(unsigned int quant){
		list<sLogElement>::iterator p;
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
		list<sLogElement>::iterator p;
		for(p=logList.begin(); p!=logList.end(); p++){
			if(p->quant==quant) return p->pLog;
		}
		return 0;
	}
	void clearLogList(){
		list<sLogElement>::iterator p;
		for(p=logList.begin(); p!=logList.end(); p++){
			if(p->pLog) delete p->pLog;
		}
		logList.clear();
	}
	void eraseLogListUntil(int quant){
		list<sLogElement>::iterator p;
		for(p=logList.begin(); p!=logList.end(); ){
			if(p->quant <= quant) {
				if(p->pLog) delete p->pLog;
				p=logList.erase(p);
			}
			else break;
		}
	}
	void writeLogList2File(XStream& file){
		list<sLogElement>::iterator p;
		for(p=logList.begin(); p!=logList.end(); p++){
			file.write(p->pLog->address(), p->pLog->tell());
		}
	}

	unsigned long clientGeneralCommandCounterInListCommand;// аналог fullListGameCommands.size()
	unsigned long lastRealizedQuant; //по идее это currentQuant
	unsigned long allowedRealizingQuant;

	unsigned long lastQuantAllowedTimeCommand;
	unsigned long generalCommandCounter4TimeCommand;

	long getInternalLagQuant(void);

	void logQuant();
	void sendLog(unsigned int quant);

	bool flag_HostMigrate;

	CRITICAL_SECTION m_FullListGameCommandLock;

};

#endif //__HYPERSPACE_H__