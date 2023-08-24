#ifndef _PERIMETER_COMMON_EVENTS_
#define _PERIMETER_COMMON_EVENTS_


//-------------------------------

// Адаптация строк для передачи по сети
class StringInWrapper
{
	std::string& value_;
public:
	explicit StringInWrapper(std::string& value) : value_(value) {}
	std::string& operator()() { return value_; }
};

class StringOutWrapper
{
	const std::string& value_;
public:
	explicit StringOutWrapper(const std::string& value) : value_(value) {}
	const std::string& operator()() const { return value_; }
};

inline XBuffer& operator > (XBuffer& in, StringInWrapper&& str) { str() = in(in.tell()); in += str().size() + 1; return in; }
inline XBuffer& operator < (XBuffer& out, const StringOutWrapper& str) { out < str().c_str() < '\0'; return out; }


//--------------------------------

#include "CommonCommands.h"

#include "NetPlayer.h"
#include "../Units/UnitAttribute.h"



enum terEventID
{
	NETCOM_ID_NONE=0,

	EVENT_ID_PING = 1,


	NETCOM_4C_ID_START_LOAD_GAME,

	NETCOM_4G_ID_UNIT_COMMAND,
	NETCOM_4G_ID_REGION,
	NETCOM_4G_ID_FORCED_DEFEAT,
    NETCOM_4G_ID_EXIT,
	NETCOM_4G_ID_CHAT_MESSAGE,

	NETCOM_4H_ID_BACK_GAME_INFORMATION,
	NETCOM_4C_ID_DISPLAY_DISTRINC_AREAS,
    NETCOM_4C_ID_DESYNC_NOTIFY,
    NETCOM_4H_ID_DESYNC_ACKNOWLEDGE,
    NETCOM_4C_ID_DESYNC_RESTORE,  
	NETCOM_4C_ID_SEND_LOG_2_HOST,

	NETCOM_4H_ID_BACK_GAME_INFORMATION_2,

	NETCOM_4H_ID_SIMPLE_BACK_GAME_INFORMATION,

	NETCOM_4H_ID_REQUEST_PAUSE,
	NETCOM_4C_ID_PAUSE,

	NETCOM_ID_NEXT_QUANT,

	////---
	NETCOMC_ID_JOIN_REQUEST,
	NETCOM_4C_ID_JOIN_RESPONSE,

	NETCOM_4H_ID_REJOIN_REQUEST,
	NETCOM_4C_ID_REJOIN_RESPONCE,
	NETCOM_4C_ID_REQUEST_LAST_QUANTS_COMMANDS,
	NETCOM_4H_ID_RESPONCE_LAST_QUANTS_COMMANDS,

	NETCOMC_ID_PLAYER_READY,
	NETCOM_4C_ID_CONTINUE_GAME_AFTER_HOST_MIGRATE,

	NETCOM_4C_ID_CUR_MISSION_DESCRIPTION_INFO,
	NETCOM_4H_ID_CHANGE_PLAYER_BELLIGERENT,
	NETCOM_4H_ID_CHANGE_PLAYER_COLOR,
	NETCOM_4H_ID_CHANGE_REAL_PLAYER_TYPE,
	NETCOM_4H_ID_CHANGE_PLAYER_DIFFICULTY,
	NETCOM_4H_ID_CHANGE_PLAYER_CLAN,
    NETCOM_4H_ID_CHANGE_PLAYER_HANDICAP,
    NETCOM_4H_ID_CHANGE_PLAYER_SEAT,
	NETCOM_4H_ID_CHANGE_MAP,

	NETCOM_4H_ID_START_LOAD_GAME,

	NETCOM_4C_ID_LATENCY_STATUS,
	NETCOM_4H_ID_LATENCY_RESPONSE,

	NETCOM_4C_ID_CLIENT_IS_NOT_RESPONCE,

	EVENT_ID_SERVER_TIME_CONTROL,
};

//-------------------------------

struct netCommandGeneral
{
	terEventID EventID;

	netCommandGeneral(terEventID event_id) { EventID = event_id; }
	virtual ~netCommandGeneral() = default;

    netCommandGeneral(const netCommandGeneral&) = delete;
    netCommandGeneral& operator=(netCommandGeneral const&) = delete;

	virtual void Write(XBuffer& out) const {}
};

//-------------------------------

struct terEventPing : netCommandGeneral
{
    terEventPing() : netCommandGeneral(EVENT_ID_PING){ };
};

//-------------------------------

struct netCommand4G_Exit : netCommandGeneral
{
    NETID netid;
    explicit netCommand4G_Exit(NETID netid_) : netCommandGeneral(NETCOM_4G_ID_EXIT), netid(netid_) {
    }
    
    explicit netCommand4G_Exit(XBuffer& in) : netCommandGeneral(NETCOM_4G_ID_EXIT) {
        in.read(&netid, sizeof(netid));
    }

    void Write(XBuffer& out) const override {
        out.write(&netid, sizeof(netid));
    }
};

//-------------------------------------------

struct netCommand4C_StartLoadGame : netCommandGeneral {

	MissionDescription* missionDescription_ = nullptr;

	netCommand4C_StartLoadGame(const MissionDescription* missionDescription) : netCommandGeneral(NETCOM_4C_ID_START_LOAD_GAME) {
        if (missionDescription) {
            missionDescription_ = new MissionDescription(*missionDescription);
        }
	}
    
    ~netCommand4C_StartLoadGame() override {
        delete missionDescription_;
    }

	netCommand4C_StartLoadGame(XBuffer& in) : netCommandGeneral(NETCOM_4C_ID_START_LOAD_GAME) {
        bool hasMission = false;
        in.read(&hasMission, sizeof(hasMission));
		if (hasMission) {
            missionDescription_ = new MissionDescription();
            missionDescription_->read(in);
        }
	}

	void Write(XBuffer& out) const override {
        bool hasMission = missionDescription_ != nullptr;
        out.write(&hasMission, sizeof(hasMission));
		if (hasMission) {
            missionDescription_->write(out);
        }
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////
class netCommandGame : public netCommandGeneral {
public:
    unsigned int PlayerID_ = 0;
    unsigned int curCommandQuant_ = -1;
    unsigned int curCommandCounter_ = -1;
    bool flag_lastCommandInQuant_ = false;
    
	explicit netCommandGame(unsigned int playerID, terEventID event_id) : netCommandGeneral(event_id), PlayerID_(playerID) {
	}

    explicit netCommandGame(terEventID event_id, XBuffer& in) : netCommandGame(0, event_id) {
        in.read(&PlayerID_, sizeof(PlayerID_));
        in.read(&curCommandQuant_, sizeof(curCommandQuant_));
        in.read(&curCommandCounter_, sizeof(curCommandCounter_));
        in.read(&flag_lastCommandInQuant_, sizeof(flag_lastCommandInQuant_));
    }
    
	void setCurCommandQuantAndCounter(unsigned int ccq, unsigned int ccc){
		curCommandQuant_=ccq;
		curCommandCounter_=ccc;
	}
	void setFlagLastCommandInQuant(){
		flag_lastCommandInQuant_=true;
	}

	bool operator == (const netCommandGame &secop) const;
    
    void Write(XBuffer& out) const override {
        out.write(&PlayerID_, sizeof(PlayerID_));
        out.write(&curCommandQuant_, sizeof(curCommandQuant_));
        out.write(&curCommandCounter_, sizeof(curCommandCounter_));
        out.write(&flag_lastCommandInQuant_, sizeof(flag_lastCommandInQuant_));
    }
};

//Внимание ! можно защитить простым "плавающим" паролем
class netCommand4G_ForcedDefeat : public netCommandGame
{
public:

	explicit netCommand4G_ForcedDefeat(unsigned int playerID) : netCommandGame(playerID, NETCOM_4G_ID_FORCED_DEFEAT){
	}

	explicit netCommand4G_ForcedDefeat(XBuffer& in) : netCommandGame(NETCOM_4G_ID_FORCED_DEFEAT, in){
	}

	void Write(XBuffer& out) const override {
        netCommandGame::Write(out);
	}

    bool operator == (const netCommand4G_ForcedDefeat &secop) const{
        return(curCommandQuant_	==secop.curCommandQuant_		&&
        curCommandCounter_		==secop.curCommandCounter_		&&
        flag_lastCommandInQuant_==secop.flag_lastCommandInQuant_);
    }
};

class netCommand4G_UnitCommand : public netCommandGame
{
public:
	netCommand4G_UnitCommand(unsigned int playerID, const terUnitID& owner, const UnitCommand& unitCommand)
					: netCommandGame(playerID, NETCOM_4G_ID_UNIT_COMMAND), owner_(owner), unitCommand_(unitCommand) {
	}
	explicit netCommand4G_UnitCommand(XBuffer& in) : netCommandGame(NETCOM_4G_ID_UNIT_COMMAND, in){
		in.read(&owner_, sizeof(owner_));
		unitCommand_.Read(in);
	}
	
	void Write(XBuffer& out) const override {
        netCommandGame::Write(out);
		out.write(&owner_, sizeof(owner_));
		unitCommand_.Write(out);
	}

	bool operator == (const netCommand4G_UnitCommand &secop) const{
		return(curCommandQuant_	==secop.curCommandQuant_		&&
		curCommandCounter_		==secop.curCommandCounter_		&&
		flag_lastCommandInQuant_==secop.flag_lastCommandInQuant_&&
		owner_					==secop.owner_					&&
		unitCommand_			==secop.unitCommand_);
	}

	const terUnitID& owner() const { return owner_; }
	const UnitCommand& unitCommand() const { return unitCommand_; }

private:
	terUnitID owner_;
	UnitCommand unitCommand_;
};



struct netCommand4G_Region : public netCommandGame {
public:
	netCommand4G_Region(unsigned int playerID, XBuffer& out) : netCommandGame(playerID, NETCOM_4G_ID_REGION) {
		dataSize_=out.tell();
		pData_=new unsigned char[dataSize_];
		memcpy(pData_, out, dataSize_);
	}
	netCommand4G_Region(XBuffer& in) : netCommandGame(NETCOM_4G_ID_REGION, in){
		in.read(&dataSize_, sizeof(dataSize_));
		pData_=new unsigned char[dataSize_];
		in.read(pData_, dataSize_);
	}
	~netCommand4G_Region() override {
		delete[] pData_;
		pData_= nullptr;
	}
	
	void Write(XBuffer& out) const override {
        netCommandGame::Write(out);
		out.write(&dataSize_, sizeof(dataSize_));
		out.write(pData_, dataSize_);
	}

	bool operator == (const netCommand4G_Region &secop) const{
		return(curCommandQuant_		==secop.curCommandQuant_			&&
			curCommandCounter_		==secop.curCommandCounter_			&&
			flag_lastCommandInQuant_==secop.flag_lastCommandInQuant_	&&
			dataSize_				==secop.dataSize_					&&
			(memcmp(pData_, secop.pData_, dataSize_)==0) );
	}

	unsigned int dataSize_;
	unsigned char* pData_;
};

////////////////////////////////////
//CHAT
class netCommand4G_ChatMessage : public netCommandGeneral
{
public:
	netCommand4G_ChatMessage(const bool clanOnly_, const std::string& text_, const std::string& locale_) : netCommandGeneral(NETCOM_4G_ID_CHAT_MESSAGE){
        playerID = -1; //Added by host
        clanOnly = clanOnly_;
        text = text_;
        locale = locale_;
	}
    
	netCommand4G_ChatMessage(XBuffer& in) : netCommandGeneral(NETCOM_4G_ID_CHAT_MESSAGE){
        in.read(&playerID, sizeof(playerID));
        in.read(&clanOnly, sizeof(clanOnly));
		in > StringInWrapper(text);
        in > StringInWrapper(locale);
	}
    
    ~netCommand4G_ChatMessage() = default;
    
	void Write(XBuffer& out) const override {
        out.write(&playerID, sizeof(playerID));
        out.write(&clanOnly, sizeof(clanOnly));
		out < StringOutWrapper(text);
        out < StringOutWrapper(locale);
	}
    
	int playerID;
    bool clanOnly;
    std::string text;
    std::string locale;
};

////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
struct netCommand4H_BackGameInformation : public netCommandGeneral {
public:
	netCommand4H_BackGameInformation (unsigned int quant, XBuffer& vmapData, XBuffer& gameData) : netCommandGeneral(NETCOM_4H_ID_BACK_GAME_INFORMATION){
		quant_=quant;
		VDataSize_=vmapData.tell();
		pVData_=new unsigned char[VDataSize_];
		memcpy(pVData_, vmapData, VDataSize_);
		GDataSize_=gameData.tell();
		pGData_=new unsigned char[GDataSize_];
		memcpy(pGData_, gameData, GDataSize_);
	}
	netCommand4H_BackGameInformation (XBuffer& in) : netCommandGeneral(NETCOM_4H_ID_BACK_GAME_INFORMATION){
		in.read(&quant_, sizeof(quant_));
		in.read(&VDataSize_, sizeof(VDataSize_));
		pVData_=new unsigned char[VDataSize_];
		in.read(pVData_, VDataSize_);
		in.read(&GDataSize_, sizeof(GDataSize_));
		pGData_=new unsigned char[GDataSize_];
		in.read(pGData_, GDataSize_);
	}
	~netCommand4H_BackGameInformation (void){
		delete[] pVData_;
		delete[] pGData_;
	}
	
	void Write(XBuffer& out) const override {
		out.write(&quant_, sizeof(quant_));
		out.write(&VDataSize_, sizeof(VDataSize_));
		out.write(pVData_, VDataSize_);
		out.write(&GDataSize_, sizeof(GDataSize_));
		out.write(pGData_, GDataSize_);
	}

	bool operator == (const netCommand4H_BackGameInformation &secop) const {
		return ( (quant_ == secop.quant_) && 
			(VDataSize_== secop.VDataSize_) && (memcmp(pVData_, secop.pVData_, VDataSize_)==0) &&
			(GDataSize_== secop.GDataSize_) && (memcmp(pGData_, secop.pGData_, GDataSize_)==0) );
	}
	bool equalGData(const netCommand4H_BackGameInformation &secop){
		return ( (GDataSize_== secop.GDataSize_) && (memcmp(pGData_, secop.pGData_, GDataSize_)==0) );
	};
	bool equalVData(const netCommand4H_BackGameInformation &secop){
		return ( (VDataSize_== secop.VDataSize_) && (memcmp(pVData_, secop.pVData_, VDataSize_)==0) );
	};
	unsigned int quant_;
	unsigned int VDataSize_;
	unsigned char* pVData_;
	unsigned int GDataSize_;
	unsigned char* pGData_;
};

struct netCommand4H_SimpleBackGameInformation : public netCommandGeneral {
public:
	netCommand4H_SimpleBackGameInformation(unsigned int quant, unsigned int signature) : netCommandGeneral(NETCOM_4H_ID_SIMPLE_BACK_GAME_INFORMATION){
		quant_=quant;
		signature_=signature;
	}
	netCommand4H_SimpleBackGameInformation(XBuffer& in) : netCommandGeneral(NETCOM_4H_ID_SIMPLE_BACK_GAME_INFORMATION){
		in.read(&quant_, sizeof(quant_));
		in.read(&signature_, sizeof(signature_));
	}
	~netCommand4H_SimpleBackGameInformation(void){
	}
	
	void Write(XBuffer& out) const override {
		out.write(&quant_, sizeof(quant_));
		out.write(&signature_, sizeof(signature_));
	}

	unsigned int quant_;
	unsigned int signature_;
};



struct netCommand4H_BackGameInformation2 : public netCommandGeneral {
public:
    netCommand4H_BackGameInformation2 () : netCommandGeneral(NETCOM_4H_ID_BACK_GAME_INFORMATION_2) {
    }
    
	netCommand4H_BackGameInformation2 (unsigned int lagQuant, unsigned int quant, unsigned int signature, bool replay=0, int state=0) : netCommandGeneral(NETCOM_4H_ID_BACK_GAME_INFORMATION_2){
		lagQuant_=lagQuant;
		quant_=quant;
		signature_=signature;
		replay_=replay;
		state_=state;
	}
    netCommand4H_BackGameInformation2 (XBuffer& in) : netCommandGeneral(NETCOM_4H_ID_BACK_GAME_INFORMATION_2){
        in.read(&lagQuant_, sizeof(lagQuant_));
        in.read(&quant_, sizeof(quant_));
        in.read(&signature_, sizeof(signature_));
        in.read(&replay_, sizeof(replay_));
        in.read(&state_, sizeof(state_));
    }
    netCommand4H_BackGameInformation2& operator=(const netCommand4H_BackGameInformation2& other) {
        lagQuant_ = other.lagQuant_;
        quant_ = other.quant_;
        signature_ = other.signature_;
        replay_ = other.replay_;
        state_ = other.state_;
        return *this;
    }
    netCommand4H_BackGameInformation2 (const netCommand4H_BackGameInformation2& other) : netCommandGeneral(NETCOM_4H_ID_BACK_GAME_INFORMATION_2) {
        lagQuant_ = other.lagQuant_;
        quant_ = other.quant_;
        signature_ = other.signature_;
        replay_ = other.replay_;
        state_ = other.state_;
    }

	void Write(XBuffer& out) const override {
		out.write(&lagQuant_, sizeof(lagQuant_));
		out.write(&quant_, sizeof(quant_));
		out.write(&signature_, sizeof(signature_));
		out.write(&replay_, sizeof(replay_));
		out.write(&state_, sizeof(state_));
	}

	bool operator == (const netCommand4H_BackGameInformation2 &secop) const {
		//lagQuant не сравнивается !
		return ( (quant_ == secop.quant_) && 
			(signature_== secop.signature_) );
	}

	unsigned int lagQuant_ = 0;
	unsigned int quant_ = 0;
	unsigned int signature_ = 0;
	unsigned int replay_ = 0;
	unsigned int state_ = 0;
};


struct netCommand4C_DisplayDistrincAreas : public netCommandGeneral {
public:
	netCommand4C_DisplayDistrincAreas(bool tmp, XBuffer& distrincAreas) : netCommandGeneral(NETCOM_4C_ID_DISPLAY_DISTRINC_AREAS){
		DASize_=distrincAreas.tell();
		pDAData_=new unsigned char[DASize_];
		memcpy(pDAData_, distrincAreas, DASize_);
	}
	netCommand4C_DisplayDistrincAreas(XBuffer& in) : netCommandGeneral(NETCOM_4C_ID_DISPLAY_DISTRINC_AREAS){
		in.read(&DASize_, sizeof(DASize_));
		pDAData_=new unsigned char[DASize_];
		in.read(pDAData_, DASize_);
	}
	~netCommand4C_DisplayDistrincAreas(void){
		delete[] pDAData_;
	}
	
	void Write(XBuffer& out) const override {
		out.write(&DASize_, sizeof(DASize_));
		out.write(pDAData_, DASize_);
	}

	unsigned int DASize_;
	unsigned char* pDAData_;
};

///Sent from server when client is detected to be desynced
struct netCommand4C_DesyncNotify : public netCommandGeneral {
public:
    int desync_amount;
    std::string gameID;

    netCommand4C_DesyncNotify(const std::string& gameID_) : netCommandGeneral(NETCOM_4C_ID_DESYNC_NOTIFY) {
        desync_amount = 0;
        gameID = gameID_;
    }

    netCommand4C_DesyncNotify(XBuffer& in) : netCommandGeneral(NETCOM_4C_ID_DESYNC_NOTIFY) {
        in > desync_amount;
        in > StringInWrapper(gameID);
    }

    void Write(XBuffer& out) const override {
        out < desync_amount;
        out < StringOutWrapper(gameID);
    };
};

///Sent to server to share the state of client and also acknowledge that its now in desync state
struct netCommand4H_DesyncAcknowledge : public netCommandGeneral {
public:
    std::unique_ptr<MissionDescription> missionDescription;
    XBuffer netlog = XBuffer(0, true);

    netCommand4H_DesyncAcknowledge(std::unique_ptr<MissionDescription> missionDescription_) : netCommandGeneral(NETCOM_4H_ID_DESYNC_ACKNOWLEDGE), missionDescription(std::move(missionDescription_)) {
    }

    netCommand4H_DesyncAcknowledge(XBuffer& in) : netCommandGeneral(NETCOM_4H_ID_DESYNC_ACKNOWLEDGE) {
        missionDescription = std::make_unique<MissionDescription>();
        missionDescription->read(in);
        in > netlog;
    }

    void Write(XBuffer& out) const override {
        missionDescription->write(out);
        out < netlog;
    }
};

///Sent by server to restore client state from same state
struct netCommand4C_DesyncRestore : public netCommandGeneral {
public:
    int desync_amount;
    std::unique_ptr<MissionDescription> missionDescription;

    netCommand4C_DesyncRestore(std::unique_ptr<MissionDescription> missionDescription_) : netCommandGeneral(NETCOM_4C_ID_DESYNC_RESTORE), missionDescription(std::move(missionDescription_)) {
        desync_amount = 0;
    }

    netCommand4C_DesyncRestore(XBuffer& in) : netCommandGeneral(NETCOM_4C_ID_DESYNC_RESTORE) {
        in > desync_amount;
        missionDescription = std::make_unique<MissionDescription>();
        missionDescription->read(in);
    }

    void Write(XBuffer& out) const override {
        out < desync_amount;
        missionDescription->write(out);
    }
};

struct netCommand4C_sendLog2Host : public netCommandGeneral {
public:
	netCommand4C_sendLog2Host(unsigned int _begQuant) : netCommandGeneral(NETCOM_4C_ID_SEND_LOG_2_HOST){
		begQuant=_begQuant;
	}
	netCommand4C_sendLog2Host (XBuffer& in) : netCommandGeneral(NETCOM_4C_ID_SEND_LOG_2_HOST){
		in > begQuant;
	}
	void Write(XBuffer& out) const override {
		out < begQuant;
	};
	unsigned int begQuant;
};


//----------------------------------------------------
struct netCommand4H_RequestPause : netCommandGeneral
{
	int playerID;
	bool pause;
	netCommand4H_RequestPause(int _playerID, bool _pause) : netCommandGeneral(NETCOM_4H_ID_REQUEST_PAUSE){
		playerID=_playerID;
		pause=_pause;
	}
	netCommand4H_RequestPause(XBuffer& in) : netCommandGeneral(NETCOM_4H_ID_REQUEST_PAUSE){
		in.read(&playerID, sizeof(playerID));
		in.read(&pause, sizeof(pause));
	}
	void Write(XBuffer& out) const override {
		out.write(&playerID, sizeof(playerID));
		out.write(&pause, sizeof(pause));
	}
};

struct netCommand4C_Pause : netCommandGeneral
{
	enum { NOT_PLAYER_ID=-1 };
	int playersIDArr[NETWORK_PLAYERS_MAX];
	bool pause;
	netCommand4C_Pause(const int _playersIDArr[NETWORK_PLAYERS_MAX], bool _pause) : netCommandGeneral(NETCOM_4C_ID_PAUSE){
		memcpy(playersIDArr, _playersIDArr, sizeof(playersIDArr));
		pause=_pause;
	}
	netCommand4C_Pause(XBuffer& in) : netCommandGeneral(NETCOM_4C_ID_PAUSE){
		in.read(&playersIDArr[0], sizeof(playersIDArr));
		in.read(&pause, sizeof(pause));
	}
	void Write(XBuffer& out) const override {
		out.write(&playersIDArr[0], sizeof(playersIDArr));
		out.write(&pause, sizeof(pause));
	}

};

struct netCommandNextQuant : netCommandGeneral
{
	enum { NOT_QUANT_CONFIRMATION=-1 };
	unsigned int numberQuant_;
	unsigned int amountCommandsPerQuant_;
	unsigned int quantConfirmation_;
	unsigned int globalCommandCounter_;
	bool flag_pause_;
	float kTime_;


	netCommandNextQuant(unsigned int numberQuant, unsigned int amountCommandsPerQuant, unsigned int globalCommandCounter, unsigned int quantConfirmation=NOT_QUANT_CONFIRMATION, bool flag_pause=0, float kTime=1.f) : netCommandGeneral(NETCOM_ID_NEXT_QUANT){
		numberQuant_=numberQuant;
		amountCommandsPerQuant_=amountCommandsPerQuant;
		quantConfirmation_=quantConfirmation;
		globalCommandCounter_=globalCommandCounter;
		flag_pause_=flag_pause;
		kTime_=kTime;
	}
	netCommandNextQuant(XBuffer& in) : netCommandGeneral(NETCOM_ID_NEXT_QUANT){
		in.read(&numberQuant_, sizeof(numberQuant_));
		in.read(&amountCommandsPerQuant_, sizeof(amountCommandsPerQuant_));
		in.read(&quantConfirmation_, sizeof(quantConfirmation_));
		in.read(&globalCommandCounter_, sizeof(globalCommandCounter_));
		in.read(&flag_pause_, sizeof(flag_pause_));
		in.read(&kTime_, sizeof(kTime_));
	}

	void Write(XBuffer& out) const override {
		out.write(&numberQuant_, sizeof(numberQuant_));
		out.write(&amountCommandsPerQuant_, sizeof(amountCommandsPerQuant_));
		out.write(&quantConfirmation_, sizeof(quantConfirmation_));
		out.write(&globalCommandCounter_, sizeof(globalCommandCounter_));
		out.write(&flag_pause_, sizeof(flag_pause_));
		out.write(&kTime_, sizeof(kTime_));
	}
};

//---------------------------------

struct netCommand4H_ReJoinRequest : netCommandGeneral
{
	unsigned int currentLastQuant;
	unsigned int confirmedQuant;
	netCommand4H_ReJoinRequest(unsigned int _currentLastQuant, unsigned int _confirmedQuant) : netCommandGeneral(NETCOM_4H_ID_REJOIN_REQUEST) {
		currentLastQuant=_currentLastQuant;
		confirmedQuant=_confirmedQuant;
	}
	netCommand4H_ReJoinRequest(XBuffer& in) : netCommandGeneral(NETCOM_4H_ID_REJOIN_REQUEST){
		in.read(&currentLastQuant, sizeof(currentLastQuant));
		in.read(&confirmedQuant, sizeof(confirmedQuant));
	}

	void Write(XBuffer& out) const override {
		out.write(&currentLastQuant, sizeof(currentLastQuant));
		out.write(&confirmedQuant, sizeof(confirmedQuant));
	}
};

struct netCommand4C_ReJoineResponce : netCommandGeneral
{
	NETID hostNETID_;
	netCommand4C_ReJoineResponce(NETID hostNETID) : netCommandGeneral(NETCOM_4C_ID_REJOIN_RESPONCE) {
		hostNETID_=hostNETID;
	}
	netCommand4C_ReJoineResponce(XBuffer& in) : netCommandGeneral(NETCOM_4C_ID_REJOIN_RESPONCE){
		in.read(&hostNETID_, sizeof(hostNETID_));
	}
	void Write(XBuffer& out) const override {
		out.write(&hostNETID_, sizeof(hostNETID_));
	}
};

struct netCommand4C_RequestLastQuantsCommands : netCommandGeneral
{
	unsigned int beginQunat_;
	netCommand4C_RequestLastQuantsCommands(unsigned int beginQunat) : netCommandGeneral(NETCOM_4C_ID_REQUEST_LAST_QUANTS_COMMANDS) {
		beginQunat_=beginQunat;
	}
	netCommand4C_RequestLastQuantsCommands(XBuffer& in) : netCommandGeneral(NETCOM_4C_ID_REQUEST_LAST_QUANTS_COMMANDS){
		in.read(&beginQunat_, sizeof(beginQunat_));
	}

	void Write(XBuffer& out) const override {
		out.write(&beginQunat_, sizeof(beginQunat_));
	}
};

struct netCommand4H_ResponceLastQuantsCommands : netCommandGeneral
{
	unsigned int beginQuantCommandTransmit;
	unsigned int endQuantCommandTransmit;
	unsigned int finGeneraCommandCounter;
	unsigned int sizeCommandBuf;
	char* pData;

	netCommand4H_ResponceLastQuantsCommands(unsigned int _bQC, unsigned int _eQC, unsigned int _fGCC, unsigned int _szBuf, char* _pData) : netCommandGeneral(NETCOM_4H_ID_RESPONCE_LAST_QUANTS_COMMANDS) {
		beginQuantCommandTransmit=_bQC;
		endQuantCommandTransmit=_eQC;
		finGeneraCommandCounter=_fGCC;
		sizeCommandBuf=_szBuf;
		pData= new char[sizeCommandBuf];
		memcpy(pData, _pData, sizeCommandBuf);
	}
	netCommand4H_ResponceLastQuantsCommands(XBuffer& in) : netCommandGeneral(NETCOM_4H_ID_RESPONCE_LAST_QUANTS_COMMANDS) {
		in.read(&beginQuantCommandTransmit, sizeof(beginQuantCommandTransmit));
		in.read(&endQuantCommandTransmit, sizeof(endQuantCommandTransmit));
		in.read(&finGeneraCommandCounter,sizeof(finGeneraCommandCounter));
		in.read(&sizeCommandBuf, sizeof(sizeCommandBuf));
		pData= new char[sizeCommandBuf];
		in.read(pData, sizeCommandBuf);
	}
	~netCommand4H_ResponceLastQuantsCommands(void){
		delete[] pData;
	}
	void Write(XBuffer& out) const override {
		out.write(&beginQuantCommandTransmit, sizeof(beginQuantCommandTransmit));
		out.write(&endQuantCommandTransmit, sizeof(endQuantCommandTransmit));
		out.write(&finGeneraCommandCounter,sizeof(finGeneraCommandCounter));
		out.write(&sizeCommandBuf, sizeof(sizeCommandBuf));
		out.write(pData, sizeCommandBuf);
	}
};
/*
struct netCommand4C_ReJoinResponse : netCommandGeneral
{
	NETID playerNETID_;
	NETID groupNETID_;
	netCommand4C_JoinResponse(NETID playerNETID, NETID groupNETID) : netCommandGeneral(NETCOM_4C_ID_REJOIN_RESPONSE) {
		playerNETID_=playerNETID;
		groupNETID_=groupNETID;
	}
	netCommand4C_ReJoinResponse(XBuffer& in) : netCommandGeneral(NETCOM_4C_ID_REJOIN_RESPONSE) {
		in.read(&playerNETID_, sizeof(playerNETID_));
		in.read(&groupNETID_, sizeof(groupNETID_));
	}

	void Write(XBuffer& out) const override {
		out.write(&playerNETID_, sizeof(playerNETID_));
		out.write(&groupNETID_, sizeof(groupNETID_));
	}
};
*/



struct netCommandC_PlayerReady : netCommandGeneral
{
	unsigned int gameCRC_;
	netCommandC_PlayerReady(unsigned int gameCRC) : netCommandGeneral(NETCOMC_ID_PLAYER_READY){
		gameCRC_=gameCRC;
	}
	netCommandC_PlayerReady(XBuffer& in) : netCommandGeneral(NETCOMC_ID_PLAYER_READY){
		in.read(&gameCRC_, sizeof(gameCRC_));
	}
	void Write(XBuffer& out) const override {
		out.write(&gameCRC_, sizeof(gameCRC_));
	}
};


struct NetLatencyInfo {
    //Timestamp when this info was sent, also to return for the next latency status round
    uint64_t timestamp = 0;
    //Server quant
    size_t quant = 0;
    //Client player ids
    std::vector<int> player_ids = {};
    //Client pings in us
    std::vector<size_t> player_pings = {};
    //Client last packet in us (use with server provided timestamp above) 
    std::vector<size_t> player_last_seen = {};
    //Client quant number
    std::vector<size_t> player_quants = {};
    
    void clear() {
		timestamp = 0;
		quant = 0;
        player_ids.clear();
        player_pings.clear();
        player_last_seen.clear();
        player_quants.clear();
    }

    void read(XBuffer& in) {
		in.read(timestamp);
		in.read(quant);
        uint32_t amount = 0;
        in.read(amount);
        for (size_t i = 0; i < amount; ++i) {
            int ei = 0;
            in.read(ei);
            player_ids.push_back(ei);
            size_t e = 0;
            in.read(e);
            player_pings.push_back(e);
            in.read(e);
            player_last_seen.push_back(e);
            in.read(e);
            player_quants.push_back(e);
        }
    }

    void write(XBuffer& out) const {
		out.write(timestamp);
		out.write(quant);
        uint32_t amount = player_ids.size();
        out.write(amount);
        for (size_t i = 0; i < amount; ++i) {
            int ei = player_ids[i];
            out.write(ei);
            size_t e = player_pings[i];
            out.write(e);
            e = player_last_seen[i];
            out.write(e);
            e = player_quants[i];
            out.write(e);
        }
    }
};

struct netCommand4C_LatencyStatus : netCommandGeneral
{    
    //Contains server info about clients from last round
    NetLatencyInfo info;
    
	netCommand4C_LatencyStatus() : netCommandGeneral(NETCOM_4C_ID_LATENCY_STATUS) {
    }
    
	explicit netCommand4C_LatencyStatus(XBuffer& in) : netCommandGeneral(NETCOM_4C_ID_LATENCY_STATUS){
        info.read(in);
    }
    
    void Write(XBuffer& out) const override {
        info.write(out);
    }
};

struct netCommand4H_LatencyResponse : netCommandGeneral
{
    uint64_t timestamp = 0;
    netCommand4H_LatencyResponse() : netCommandGeneral(NETCOM_4H_ID_LATENCY_RESPONSE) {
    }
    
    explicit netCommand4H_LatencyResponse(XBuffer& in) : netCommandGeneral(NETCOM_4H_ID_LATENCY_RESPONSE){
        in.read(timestamp);
    }
    
    void Write(XBuffer& out) const override {
        out.write(timestamp);
    }
};


struct netCommand4C_ClientIsNotResponce : netCommandGeneral
{
	netCommand4C_ClientIsNotResponce(std::string& _clientNotResponceList) : netCommandGeneral(NETCOM_4C_ID_CLIENT_IS_NOT_RESPONCE){
		clientNotResponceList=_clientNotResponceList;
	}
	netCommand4C_ClientIsNotResponce(XBuffer& in) : netCommandGeneral(NETCOM_4C_ID_CLIENT_IS_NOT_RESPONCE){
		in > StringInWrapper(clientNotResponceList);
	}
	void Write(XBuffer& out) const override {
		out < StringOutWrapper(clientNotResponceList);
	}
	std::string clientNotResponceList;
};




struct netCommand4C_ContinueGameAfterHostMigrate : netCommandGeneral
{
	netCommand4C_ContinueGameAfterHostMigrate() : netCommandGeneral(NETCOM_4C_ID_CONTINUE_GAME_AFTER_HOST_MIGRATE){}
	netCommand4C_ContinueGameAfterHostMigrate(XBuffer& in) : netCommandGeneral(NETCOM_4C_ID_CONTINUE_GAME_AFTER_HOST_MIGRATE){}
	//void Write(XBuffer& out) const override;
};

struct netCommand4H_StartLoadGame : netCommandGeneral
{
    uint32_t ready;
	netCommand4H_StartLoadGame(bool state) : netCommandGeneral(NETCOM_4H_ID_START_LOAD_GAME){
        ready=state?1:0;
	}
	netCommand4H_StartLoadGame(XBuffer& in) : netCommandGeneral(NETCOM_4H_ID_START_LOAD_GAME){
		in > ready;
	}
	void Write(XBuffer& out) const override {
		out < ready;
	};
};

struct netCommand4C_CurrentMissionDescriptionInfo : netCommandGeneral
{
	MissionDescription missionDescription_;

	netCommand4C_CurrentMissionDescriptionInfo(const MissionDescription& missionDescription, int playerID) : netCommandGeneral(NETCOM_4C_ID_CUR_MISSION_DESCRIPTION_INFO) {
		missionDescription_=missionDescription;
        missionDescription_.activePlayerID = playerID;
	}

	netCommand4C_CurrentMissionDescriptionInfo(XBuffer& in) : netCommandGeneral(NETCOM_4C_ID_CUR_MISSION_DESCRIPTION_INFO) {
		//missionDescription_.read(in);
		missionDescription_.simpleRead(in);
	}

	void Write(XBuffer& out) const override {
		//missionDescription_.write(out);
		missionDescription_.simpleWrite(out);
	}
};

struct netCommand4H_ChangePlayerBelligerent : netCommandGeneral
{
	netCommand4H_ChangePlayerBelligerent(int idxPlayerData, terBelligerent newBelligerent) : netCommandGeneral(NETCOM_4H_ID_CHANGE_PLAYER_BELLIGERENT){
		idxPlayerData_=idxPlayerData;
		newBelligerent_=newBelligerent;
	}
	netCommand4H_ChangePlayerBelligerent(XBuffer& in) : netCommandGeneral(NETCOM_4H_ID_CHANGE_PLAYER_BELLIGERENT){
		in.read(&idxPlayerData_, sizeof(idxPlayerData_));
		in.read(&newBelligerent_, sizeof(newBelligerent_));
	}
	void Write(XBuffer& out) const override {
		out.write(&idxPlayerData_, sizeof(idxPlayerData_));
		out.write(&newBelligerent_, sizeof(newBelligerent_));
	};
	int idxPlayerData_;
	terBelligerent newBelligerent_;
};

struct netCommand4H_ChangePlayerColor : netCommandGeneral
{
	netCommand4H_ChangePlayerColor(int idxPlayerData, int newColor, bool direction_) : netCommandGeneral(NETCOM_4H_ID_CHANGE_PLAYER_COLOR){
		idxPlayerData_=idxPlayerData;
		newColor_=newColor;
        direction=direction_;
	}
	netCommand4H_ChangePlayerColor(XBuffer& in) : netCommandGeneral(NETCOM_4H_ID_CHANGE_PLAYER_COLOR){
		in.read(&idxPlayerData_, sizeof(idxPlayerData_));
		in.read(&newColor_, sizeof(newColor_));
        in.read(&direction, sizeof(direction));
	}
	void Write(XBuffer& out) const override {
		out.write(&idxPlayerData_, sizeof(idxPlayerData_));
		out.write(&newColor_, sizeof(newColor_));
        out.write(&direction, sizeof(direction));
	};
	int idxPlayerData_;
	int newColor_;
    bool direction;
};


struct netCommand4H_ChangeRealPlayerType : netCommandGeneral
{
	netCommand4H_ChangeRealPlayerType(int idxPlayerData, RealPlayerType newRealPlayerType) : netCommandGeneral(NETCOM_4H_ID_CHANGE_REAL_PLAYER_TYPE){
		idxPlayerData_=idxPlayerData;
		newRealPlayerType_=newRealPlayerType;
	}
	netCommand4H_ChangeRealPlayerType(XBuffer& in) : netCommandGeneral(NETCOM_4H_ID_CHANGE_REAL_PLAYER_TYPE){
		in.read(&idxPlayerData_, sizeof(idxPlayerData_));
		in.read(&newRealPlayerType_, sizeof(newRealPlayerType_));
	}
	void Write(XBuffer& out) const override {
		out.write(&idxPlayerData_, sizeof(idxPlayerData_));
		out.write(&newRealPlayerType_, sizeof(newRealPlayerType_));
	};
	int idxPlayerData_;
	RealPlayerType newRealPlayerType_;
};

struct netCommand4H_ChangePlayerDifficulty : netCommandGeneral
{
	netCommand4H_ChangePlayerDifficulty(int idxPlayerData, Difficulty difficulty) : netCommandGeneral(NETCOM_4H_ID_CHANGE_PLAYER_DIFFICULTY){
		idxPlayerData_=idxPlayerData;
		difficulty_=difficulty;
	}
	netCommand4H_ChangePlayerDifficulty(XBuffer& in) : netCommandGeneral(NETCOM_4H_ID_CHANGE_PLAYER_DIFFICULTY){
		in.read(&idxPlayerData_, sizeof(idxPlayerData_));
		in.read(&difficulty_, sizeof(difficulty_));
	}
	void Write(XBuffer& out) const override {
		out.write(&idxPlayerData_, sizeof(idxPlayerData_));
		out.write(&difficulty_, sizeof(difficulty_));
	};
	int idxPlayerData_;
	Difficulty difficulty_;
};
struct netCommand4H_ChangePlayerClan : netCommandGeneral
{
	netCommand4H_ChangePlayerClan(int idxPlayerData, int clan) : netCommandGeneral(NETCOM_4H_ID_CHANGE_PLAYER_CLAN){
		idxPlayerData_=idxPlayerData;
		clan_=clan;
	}
	netCommand4H_ChangePlayerClan(XBuffer& in) : netCommandGeneral(NETCOM_4H_ID_CHANGE_PLAYER_CLAN){
		in.read(&idxPlayerData_, sizeof(idxPlayerData_));
		in.read(&clan_, sizeof(clan_));
	}
	void Write(XBuffer& out) const override {
		out.write(&idxPlayerData_, sizeof(idxPlayerData_));
		out.write(&clan_, sizeof(clan_));
	};
	int idxPlayerData_;
	int clan_;
};


struct netCommand4H_ChangePlayerHandicap : netCommandGeneral
{
    netCommand4H_ChangePlayerHandicap(int idxPlayerData, int handicap) : netCommandGeneral(NETCOM_4H_ID_CHANGE_PLAYER_HANDICAP){
        idxPlayerData_=idxPlayerData;
        handicap_=handicap;
    }
    netCommand4H_ChangePlayerHandicap(XBuffer& in) : netCommandGeneral(NETCOM_4H_ID_CHANGE_PLAYER_HANDICAP){
        in.read(&idxPlayerData_, sizeof(idxPlayerData_));
        in.read(&handicap_, sizeof(handicap_));
    }
    void Write(XBuffer& out) const override {
        out.write(&idxPlayerData_, sizeof(idxPlayerData_));
        out.write(&handicap_, sizeof(handicap_));
    };
    int idxPlayerData_;
    int handicap_;
};

struct netCommand4H_ChangePlayerSeat : netCommandGeneral
{
    netCommand4H_ChangePlayerSeat(int idxPlayerData) : netCommandGeneral(NETCOM_4H_ID_CHANGE_PLAYER_SEAT){
        idxPlayerData_=idxPlayerData;
    }
    netCommand4H_ChangePlayerSeat(XBuffer& in) : netCommandGeneral(NETCOM_4H_ID_CHANGE_PLAYER_SEAT){
        in > idxPlayerData_;
    }
    void Write(XBuffer& out) const override {
        out < idxPlayerData_;
    };
    int idxPlayerData_;
};

///Command sent to host when a map is selected from list, establishes the hostMissionDescription
struct netCommand4H_ChangeMap : netCommandGeneral
{
	MissionDescription missionDescription_;

	netCommand4H_ChangeMap(MissionDescription& missionDescription) : netCommandGeneral(NETCOM_4H_ID_CHANGE_MAP){
		missionDescription_=missionDescription;
	}
	netCommand4H_ChangeMap(XBuffer& in) : netCommandGeneral(NETCOM_4H_ID_CHANGE_MAP){
		missionDescription_.read(in);
	}
	void Write(XBuffer& out) const override {
		missionDescription_.write(out);
	}
};

struct terEventControlServerTime : netCommandGeneral
{
	float scale;

	terEventControlServerTime(float s) : netCommandGeneral(EVENT_ID_SERVER_TIME_CONTROL){ scale = s; }
	terEventControlServerTime(XBuffer& in);

	void Write(XBuffer& out) const override;
};

//-------------------------------

extern LogStream netLog;


#endif
