#ifndef _PERIMETER_COMMON_EVENTS_
#define _PERIMETER_COMMON_EVENTS_


const char SIMPLE_GAME_CURRENT_VERSION[]= 
#include "..\version.h" 
;

extern const unsigned int INTERNAL_BUILD_VERSION;
//-------------------------------

// Адаптация строк для передачи по сети
class StringInWrapper
{
	string& value_;
public:
	explicit StringInWrapper(string& value) : value_(value) {}
	string& operator()() { return value_; }
};

class StringOutWrapper
{
	const string& value_;
public:
	explicit StringOutWrapper(const string& value) : value_(value) {}
	const string& operator()() const { return value_; }
};

inline XBuffer& operator > (XBuffer& in, StringInWrapper& str) { str() = in(in.tell()); in += str().size() + 1; return in; }
inline XBuffer& operator < (XBuffer& out, const StringOutWrapper& str) { out < str().c_str() < '\0'; return out; }


//--------------------------------

#include "CommonCommands.h"

#include "NetPlayer.h"
#include "..\Units\UnitAttribute.h"



enum terEventID
{
	NETCOM_ID_NONE=0,

	EVENT_ID_PING = 1,


	NETCOM_4C_ID_START_LOAD_GAME,

	NETCOM_4G_ID_UNIT_COMMAND,
	NETCOM_4G_ID_REGION,
	NETCOM_4G_ID_FORCED_DEFEAT,

	NETCOM_4G_ID_CHAT_MESSAGE,

	NETCOM_4H_ID_BACK_GAME_INFORMATION,
	NETCOM_4C_ID_DISPLAY_DISTRINC_AREAS,
	NETCOM_4C_ID_SAVE_LOG,
	NETCOM_4C_ID_SEND_LOG_2_HOST,

	NETCOM_4H_ID_BACK_GAME_INFORMATION_2,

	NETCOM_4H_ID_SIMPLE_BACK_GAME_INFORMATION,

	NETCOM_4H_ID_REQUEST_PAUSE,
	NETCOM_4C_ID_PAUSE,

	NETCOM_ID_NEXT_QUANT,

	EVENT_ID_VERSION,
	EVENT_ID_ERROR,

	////---
	NETCOMC_ID_JOIN_REQUEST,
	NETCOM_4C_ID_JOIN_RESPONSE,

	NETCOM_4H_ID_REJOIN_REQUEST,
	NETCOM_4C_ID_REJOIN_RESPONCE,
	NETCOM_4C_ID_REQUEST_LAST_QUANTS_COMMANDS,
	NETCOM_4H_ID_RESPONCE_LAST_QUANTS_COMMANDS,
	//NETCOM_4C_ID_REJOIN_RESPONSE,

	NETCOMC_ID_PLAYER_READY,
	NETCOM_ID_START_GAME,
	NETCOM_4C_ID_CONTINUE_GAME_AFTER_HOST_MIGRATE,

	NETCOM_4C_ID_CUR_MISSION_DESCRIPTION_INFO,
	NETCOM_4H_ID_CHANGE_PLAYER_BELLIGERENT,
	NETCOM_4H_ID_CHANGE_PLAYER_COLOR,
	NETCOM_4H_ID_CHANGE_REAL_PLAYER_TYPE,
	NETCOM_4H_ID_CHANGE_PLAYER_DIFFICULTY,
	NETCOM_4H_ID_CHANGE_PLAYER_CLAN,
	NETCOM_4H_ID_CHANGE_PLAYER_HANDICAP,
	NETCOM_4H_ID_CHANGE_MAP,

	NETCOM_4H_ID_START_LOAD_GAME,

	EVENT_ID_CREATE_GAME,
	NETCOM_4H_ID_CREATE_GAME,

	NETCOM_4C_ID_ALIFE_PACKET,
	NETCOM_4H_ID_ALIFE_PACKET,

	NETCOM_4C_ID_CLIENT_IS_NOT_RESPONCE,

	EVENT_ID_SERVER_TIME_CONTROL,
};

//-------------------------------

struct netCommandGeneral
{
	terEventID EventID;

	netCommandGeneral(terEventID event_id) { EventID = event_id; }
	virtual ~netCommandGeneral() {}

	virtual void Write(XBuffer& out) const {}
};

//-------------------------------

struct terEventPing : netCommandGeneral
{
	terEventPing() : netCommandGeneral(EVENT_ID_PING){ };
};

//-------------------------------------------

struct netCommand4C_StartLoadGame : netCommandGeneral {

	MissionDescription missionDescription_;

	netCommand4C_StartLoadGame(MissionDescription& missionDescription) : netCommandGeneral(NETCOM_4C_ID_START_LOAD_GAME) {
		missionDescription_=missionDescription;
	}

	netCommand4C_StartLoadGame(XBuffer& in) : netCommandGeneral(NETCOM_4C_ID_START_LOAD_GAME) {
		missionDescription_.read(in);
	}

	void Write(XBuffer& out) const {
		missionDescription_.write(out);
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////
class netCommandGame : public netCommandGeneral {
public:
	netCommandGame(terEventID event_id) : netCommandGeneral(event_id) {
	}
	unsigned int curCommandQuant_;
	unsigned int curCommandCounter_;
	bool flag_lastCommandInQuant_;
	void setCurCommandQuantAndCounter(unsigned int ccq, unsigned int ccc){
		curCommandQuant_=ccq;
		curCommandCounter_=ccc;
	}
	void setFlagLastCommandInQuant(){
		flag_lastCommandInQuant_=true;
	}
	bool operator == (const netCommandGame &secop) const;
};

//Внимание ! можно защитить простым "плавающим" паролем
class netCommand4G_ForcedDefeat : public netCommandGame
{
public:
	netCommand4G_ForcedDefeat(const int _userID) : netCommandGame(NETCOM_4G_ID_FORCED_DEFEAT){
		curCommandQuant_=-1;
		curCommandCounter_=-1;
		userID=_userID;
		flag_lastCommandInQuant_=0;
	}
	netCommand4G_ForcedDefeat(XBuffer& in) : netCommandGame(NETCOM_4G_ID_FORCED_DEFEAT){
		in.read(&curCommandQuant_, sizeof(curCommandQuant_));
		in.read(&curCommandCounter_, sizeof(curCommandCounter_));
		in.read(&userID, sizeof(userID));
		in.read(&flag_lastCommandInQuant_, sizeof(flag_lastCommandInQuant_));
	}
	void Write(XBuffer& out) const{
		out.write(&curCommandQuant_, sizeof(curCommandQuant_));
		out.write(&curCommandCounter_, sizeof(curCommandCounter_));
		out.write(&userID, sizeof(userID));
		out.write(&flag_lastCommandInQuant_, sizeof(flag_lastCommandInQuant_));
	}
	int userID;
	bool operator == (const netCommand4G_ForcedDefeat &secop) const{
		return (memcmp(this, &secop, sizeof(*this))==0);
	}
};

class netCommand4G_UnitCommand : public netCommandGame
{
public:
	netCommand4G_UnitCommand(const terUnitID& owner, const UnitCommand& unitCommand)
					: netCommandGame(NETCOM_4G_ID_UNIT_COMMAND), owner_(owner), unitCommand_(unitCommand) {
		curCommandQuant_=-1;
		curCommandCounter_=-1;
		flag_lastCommandInQuant_=0;
	}
	netCommand4G_UnitCommand(XBuffer& in) : netCommandGame(NETCOM_4G_ID_UNIT_COMMAND){
		in.read(&curCommandQuant_, sizeof(curCommandQuant_));
		in.read(&curCommandCounter_, sizeof(curCommandCounter_));
		in.read(&flag_lastCommandInQuant_, sizeof(flag_lastCommandInQuant_));
		in.read(&owner_, sizeof(owner_));
		unitCommand_.Read(in);
		//in.read(*this);
	}
	
	void Write(XBuffer& out) const{
		out.write(&curCommandQuant_, sizeof(curCommandQuant_));
		out.write(&curCommandCounter_, sizeof(curCommandCounter_));
		out.write(&flag_lastCommandInQuant_, sizeof(flag_lastCommandInQuant_));
		out.write(&owner_, sizeof(owner_));
		unitCommand_.Write(out);
		//out.write(*this);
	}

	netCommand4G_UnitCommand(const netCommand4G_UnitCommand& donor) : netCommandGame(NETCOM_4G_ID_UNIT_COMMAND){
		curCommandQuant_=donor.curCommandQuant_;
		curCommandCounter_=donor.curCommandCounter_;
		flag_lastCommandInQuant_=donor.flag_lastCommandInQuant_;
		owner_=donor.owner_;
		unitCommand_=donor.unitCommand_;
		//memcpy(this, &donor, sizeof(*this));
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
	netCommand4G_Region(unsigned int playerID, XBuffer& out) : netCommandGame(NETCOM_4G_ID_REGION) {
		playerID_=playerID;
		curCommandQuant_=-1;
		curCommandCounter_=-1;
		flag_lastCommandInQuant_=0;
		dataSize_=out.tell();
		pData_=new unsigned char[dataSize_];
		memcpy(pData_, out, dataSize_);
	}
	netCommand4G_Region(XBuffer& in) : netCommandGame(NETCOM_4G_ID_REGION){
		in.read(&playerID_, sizeof(playerID_));
		in.read(&curCommandQuant_, sizeof(curCommandQuant_));
		in.read(&curCommandCounter_, sizeof(curCommandCounter_));
		in.read(&flag_lastCommandInQuant_, sizeof(flag_lastCommandInQuant_));
		in.read(&dataSize_, sizeof(dataSize_));
		pData_=new unsigned char[dataSize_];
		in.read(pData_, dataSize_);
	}
	~netCommand4G_Region(void){
		delete pData_;
		pData_=0;
	}
	
	void Write(XBuffer& out) const{
		out.write(&playerID_, sizeof(playerID_));
		out.write(&curCommandQuant_, sizeof(curCommandQuant_));
		out.write(&curCommandCounter_, sizeof(curCommandCounter_));
		out.write(&flag_lastCommandInQuant_, sizeof(flag_lastCommandInQuant_));
		out.write(&dataSize_, sizeof(dataSize_));
		out.write(pData_, dataSize_);
	}

	netCommand4G_Region(const netCommand4G_Region& donor) : netCommandGame(NETCOM_4G_ID_REGION){
		memcpy(this, &donor, sizeof(*this));
		pData_=new unsigned char[dataSize_];
		memcpy(pData_, donor.pData_, dataSize_);
	}

	bool operator == (const netCommand4G_Region &secop) const{
		return(playerID_			==secop.playerID_					&&
			curCommandQuant_		==secop.curCommandQuant_			&&
			curCommandCounter_		==secop.curCommandCounter_			&&
			flag_lastCommandInQuant_==secop.flag_lastCommandInQuant_	&&
			dataSize_				==secop.dataSize_					&&
			(memcmp(pData_, secop.pData_, dataSize_)==0) );
	}

	unsigned int playerID_;
	unsigned int dataSize_;
	unsigned char* pData_;
};

////////////////////////////////////
//CHAT
class netCommand4G_ChatMessage : public netCommandGeneral
{
public:
	netCommand4G_ChatMessage(const int _userID, const char* textStr) : netCommandGeneral(NETCOM_4G_ID_CHAT_MESSAGE){
		userID=_userID;
		dataSize_=strlen(textStr)+1;
		pData_=new char[dataSize_];
		memcpy(pData_, textStr, dataSize_);
	}
	netCommand4G_ChatMessage(XBuffer& in) : netCommandGeneral(NETCOM_4G_ID_CHAT_MESSAGE){
		in.read(&userID, sizeof(userID));
		in.read(&dataSize_, sizeof(dataSize_));
		pData_=new char[dataSize_];
		in.read(pData_, dataSize_);
	}
	void Write(XBuffer& out) const{
		out.write(&userID, sizeof(userID));
		out.write(&dataSize_, sizeof(dataSize_));
		out.write(pData_, dataSize_);
	}
	const char* getText(){
		if(dataSize_>0){
			pData_[dataSize_-1]=0;
		}
		return pData_;
	}
	int userID;
	unsigned int dataSize_;
	char* pData_;
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
		delete pVData_;
		delete pGData_;
	}
	
	void Write(XBuffer& out) const{
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
	
	void Write(XBuffer& out) const{
		out.write(&quant_, sizeof(quant_));
		out.write(&signature_, sizeof(signature_));
	}

	unsigned int quant_;
	unsigned int signature_;
};



struct netCommand4H_BackGameInformation2 : public netCommandGeneral {
public:
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

	void Write(XBuffer& out) const{
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
	unsigned int lagQuant_;
	unsigned int quant_;
	unsigned int signature_;
	unsigned int replay_;
	unsigned int state_;
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
		delete pDAData_;
	}
	
	void Write(XBuffer& out) const{
		out.write(&DASize_, sizeof(DASize_));
		out.write(pDAData_, DASize_);
	}

	unsigned int DASize_;
	unsigned char* pDAData_;
};

struct netCommand4C_SaveLog : public netCommandGeneral {
public:
	netCommand4C_SaveLog() : netCommandGeneral(NETCOM_4C_ID_SAVE_LOG){
		v=0;
	}
	netCommand4C_SaveLog(XBuffer& in) : netCommandGeneral(NETCOM_4C_ID_SAVE_LOG){
		in > v;
	}
	void Write(XBuffer& out) const {
		out < v;
	};
	int v;
};

struct netCommand4C_sendLog2Host : public netCommandGeneral {
public:
	netCommand4C_sendLog2Host(unsigned int _begQuant) : netCommandGeneral(NETCOM_4C_ID_SEND_LOG_2_HOST){
		begQuant=_begQuant;
	}
	netCommand4C_sendLog2Host (XBuffer& in) : netCommandGeneral(NETCOM_4C_ID_SEND_LOG_2_HOST){
		in > begQuant;
	}
	void Write(XBuffer& out) const {
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
	void Write(XBuffer& out) const {
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
	void Write(XBuffer& out) const {
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
	unsigned long globalCommandCounter_;
	bool flag_pause_;
	float kTime_;


	netCommandNextQuant(unsigned int numberQuant, unsigned int amountCommandsPerQuant, unsigned long globalCommandCounter, unsigned int quantConfirmation=NOT_QUANT_CONFIRMATION, bool flag_pause=0, float kTime=1.f) : netCommandGeneral(NETCOM_ID_NEXT_QUANT){
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

	void Write(XBuffer& out) const {
		out.write(&numberQuant_, sizeof(numberQuant_));
		out.write(&amountCommandsPerQuant_, sizeof(amountCommandsPerQuant_));
		out.write(&quantConfirmation_, sizeof(quantConfirmation_));
		out.write(&globalCommandCounter_, sizeof(globalCommandCounter_));
		out.write(&flag_pause_, sizeof(flag_pause_));
		out.write(&kTime_, sizeof(kTime_));
	}
};


//------------------------------

struct terEventVersion : netCommandGeneral
{
	unsigned int Version;

	terEventVersion(unsigned int version) : netCommandGeneral(EVENT_ID_VERSION) { 
		Version = version; 
	}
	terEventVersion(XBuffer& in);

	void Write(XBuffer& out) const;
};

//---------------------------------

struct terEventError : netCommandGeneral
{
	unsigned int PlayerID;
	unsigned int ErrorCode;

	terEventError(unsigned int player_id,unsigned int error_code) : netCommandGeneral(EVENT_ID_ERROR) {
		ErrorCode = error_code;
		PlayerID = player_id;
	}
	terEventError(XBuffer& in);

	void Write(XBuffer& out) const;
};


//Сейчас фактически не используется т.к. запускается её внутренний аналог - ExecuteInternalCommand(PNC_COMMAND__START_HOST_AND_CREATE_GAME_AND_STOP_FIND_HOST, true);
struct netCommand4H_CreateGame : netCommandGeneral {
	MissionDescription missionDescription_;
	PlayerData createPlayerData_;
	char gameName_[MAX_MULTIPALYER_GAME_NAME];
	char computerName_[MAX_COMPUTERNAME_LENGTH+1];
	unsigned int internalNumverVersion_;
	char simpleGameVersion_[sizeof(SIMPLE_GAME_CURRENT_VERSION)];

	netCommand4H_CreateGame(const char* gameName, const char * computerName, MissionDescription& missionDescription, PlayerData& createPlayerData) : netCommandGeneral(NETCOM_4H_ID_CREATE_GAME) {
		strncpy(gameName_, gameName, MAX_MULTIPALYER_GAME_NAME);
		strncpy(computerName_, computerName, MAX_COMPUTERNAME_LENGTH+1);
		missionDescription_=missionDescription;
		createPlayerData_=createPlayerData;
		internalNumverVersion_=INTERNAL_BUILD_VERSION;
		strncpy(simpleGameVersion_, SIMPLE_GAME_CURRENT_VERSION, sizeof(simpleGameVersion_));
	}

	netCommand4H_CreateGame(XBuffer& in) : netCommandGeneral(NETCOM_4H_ID_CREATE_GAME) {
		in.read(gameName_, MAX_MULTIPALYER_GAME_NAME);
		in.read(computerName_, MAX_COMPUTERNAME_LENGTH+1);
		missionDescription_.read(in);
		in.read(&createPlayerData_, sizeof(createPlayerData_));
		in.read(&internalNumverVersion_, sizeof(internalNumverVersion_));
		in.read(&simpleGameVersion_, sizeof(simpleGameVersion_));
	}
	void Write(XBuffer& out) const {
		out.write(gameName_, MAX_MULTIPALYER_GAME_NAME);
		out.write(computerName_, MAX_COMPUTERNAME_LENGTH+1);
		missionDescription_.write(out);
		out.write(&createPlayerData_, sizeof(createPlayerData_));
		out.write(&internalNumverVersion_, sizeof(internalNumverVersion_));
		out.write(&simpleGameVersion_, sizeof(simpleGameVersion_));
	}
};




struct netCommandC_JoinRequest : netCommandGeneral
{
	//DWORD hostGameID_;
	PlayerData joinPlayerData_;
	char computerName_[MAX_COMPUTERNAME_LENGTH+1];
	unsigned int internalNumverVersion_;
	char simpleGameVersion_[sizeof(SIMPLE_GAME_CURRENT_VERSION)];

	netCommandC_JoinRequest(/*DWORD hostGameID,*/ PlayerData& joinPlayerData, const char* computerName) : netCommandGeneral(NETCOMC_ID_JOIN_REQUEST) {
		//hostGameID_=hostGameID;
		joinPlayerData_=joinPlayerData;
		strncpy(computerName_, computerName, MAX_COMPUTERNAME_LENGTH+1);
		internalNumverVersion_=INTERNAL_BUILD_VERSION;
		strncpy(simpleGameVersion_, SIMPLE_GAME_CURRENT_VERSION, sizeof(simpleGameVersion_));
	}
	netCommandC_JoinRequest(XBuffer& in) : netCommandGeneral(NETCOMC_ID_JOIN_REQUEST){
		//in.read(&hostGameID_, sizeof(hostGameID_));
		in.read(&joinPlayerData_, sizeof(joinPlayerData_));
		in.read(computerName_, MAX_COMPUTERNAME_LENGTH+1);
		in.read(&internalNumverVersion_, sizeof(internalNumverVersion_));
		in.read(&simpleGameVersion_, sizeof(simpleGameVersion_));
	}

	void Write(XBuffer& out) const{
		//out.write(&hostGameID_, sizeof(hostGameID_));
		out.write(&joinPlayerData_, sizeof(joinPlayerData_));
		out.write(computerName_, MAX_COMPUTERNAME_LENGTH+1);
		out.write(&internalNumverVersion_, sizeof(internalNumverVersion_));
		out.write(&simpleGameVersion_, sizeof(simpleGameVersion_));
	}
};

enum netCommandJoinRequestResult{
	NCJRR_OK,
	NCJRR_INCORRECT_VERSION,
	NCJRR_GAME_CLOSED
};

struct netCommand4C_JoinResponse : netCommandGeneral
{
	DPNID playerDPNID_;
	DPNID groupDPNID_;
	netCommandJoinRequestResult result_;
	netCommand4C_JoinResponse(DPNID playerDPNID, DPNID groupDPNID, netCommandJoinRequestResult result) : netCommandGeneral(NETCOM_4C_ID_JOIN_RESPONSE) { 
		playerDPNID_=playerDPNID;
		groupDPNID_=groupDPNID;
		result_=result;
	}
	netCommand4C_JoinResponse(XBuffer& in) : netCommandGeneral(NETCOM_4C_ID_JOIN_RESPONSE) {
		in.read(&playerDPNID_, sizeof(playerDPNID_));
		in.read(&groupDPNID_, sizeof(groupDPNID_));
		in.read(&result_, sizeof(result_));
	}

	void Write(XBuffer& out) const {
		out.write(&playerDPNID_, sizeof(playerDPNID_));
		out.write(&groupDPNID_, sizeof(groupDPNID_));
		out.write(&result_, sizeof(result_));
	}
};




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

	void Write(XBuffer& out) const{
		out.write(&currentLastQuant, sizeof(currentLastQuant));
		out.write(&confirmedQuant, sizeof(confirmedQuant));
	}
};

struct netCommand4C_ReJoineResponce : netCommandGeneral
{
	DPNID hostDPNID_;
	netCommand4C_ReJoineResponce(DPNID hostDPNID) : netCommandGeneral(NETCOM_4C_ID_REJOIN_RESPONCE) {
		hostDPNID_=hostDPNID;
	}
	netCommand4C_ReJoineResponce(XBuffer& in) : netCommandGeneral(NETCOM_4C_ID_REJOIN_RESPONCE){
		in.read(&hostDPNID_, sizeof(hostDPNID_));
	}
	void Write(XBuffer& out) const{
		out.write(&hostDPNID_, sizeof(hostDPNID_));
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

	void Write(XBuffer& out) const{
		out.write(&beginQunat_, sizeof(beginQunat_));
	}
};

struct netCommand4H_ResponceLastQuantsCommands : netCommandGeneral
{
	unsigned int beginQuantCommandTransmit;
	unsigned int endQuantCommandTransmit;
	unsigned int finGeneraCommandCounter;
	unsigned int sizeCommandBuf;
	unsigned char* pData;

	netCommand4H_ResponceLastQuantsCommands(unsigned int _bQC, unsigned int _eQC, unsigned int _fGCC, unsigned int _szBuf, unsigned char* _pData) : netCommandGeneral(NETCOM_4H_ID_RESPONCE_LAST_QUANTS_COMMANDS) {
		beginQuantCommandTransmit=_bQC;
		endQuantCommandTransmit=_eQC;
		finGeneraCommandCounter=_fGCC;
		sizeCommandBuf=_szBuf;
		pData= new unsigned char[sizeCommandBuf];
		memcpy(pData, _pData, sizeCommandBuf);
	}
	netCommand4H_ResponceLastQuantsCommands(XBuffer& in) : netCommandGeneral(NETCOM_4H_ID_RESPONCE_LAST_QUANTS_COMMANDS) {
		in.read(&beginQuantCommandTransmit, sizeof(beginQuantCommandTransmit));
		in.read(&endQuantCommandTransmit, sizeof(endQuantCommandTransmit));
		in.read(&finGeneraCommandCounter,sizeof(finGeneraCommandCounter));
		in.read(&sizeCommandBuf, sizeof(sizeCommandBuf));
		pData= new unsigned char[sizeCommandBuf];
		in.read(pData, sizeCommandBuf);
	}
	~netCommand4H_ResponceLastQuantsCommands(void){
		delete pData;
	}
	void Write(XBuffer& out) const{
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
	DPNID playerDPNID_;
	DPNID groupDPNID_;
	netCommand4C_JoinResponse(DPNID playerDPNID, DPNID groupDPNID) : netCommandGeneral(NETCOM_4C_ID_REJOIN_RESPONSE) { 
		playerDPNID_=playerDPNID;
		groupDPNID_=groupDPNID;
	}
	netCommand4C_ReJoinResponse(XBuffer& in) : netCommandGeneral(NETCOM_4C_ID_REJOIN_RESPONSE) {
		in.read(&playerDPNID_, sizeof(playerDPNID_));
		in.read(&groupDPNID_, sizeof(groupDPNID_));
	}

	void Write(XBuffer& out) const {
		out.write(&playerDPNID_, sizeof(playerDPNID_));
		out.write(&groupDPNID_, sizeof(groupDPNID_));
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
	void Write(XBuffer& out) const {
		out.write(&gameCRC_, sizeof(gameCRC_));
	}
};

/*
struct netCommandStartGame : netCommandGeneral
{
	netCommandStartGame() : netCommandGeneral(NETCOM_ID_START_GAME){}
	netCommandStartGame(XBuffer& in) : netCommandGeneral(NETCOM_ID_START_GAME){}
	//void Write(XBuffer& out) const;
};
*/
struct netCommand4C_AlifePacket : netCommandGeneral
{
	netCommand4C_AlifePacket() : netCommandGeneral(NETCOM_4C_ID_ALIFE_PACKET){}
	netCommand4C_AlifePacket(XBuffer& in) : netCommandGeneral(NETCOM_4C_ID_ALIFE_PACKET){}
};

struct netCommand4H_AlifePacket : netCommandGeneral
{
	netCommand4H_AlifePacket() : netCommandGeneral(NETCOM_4H_ID_ALIFE_PACKET){}
	netCommand4H_AlifePacket(XBuffer& in) : netCommandGeneral(NETCOM_4H_ID_ALIFE_PACKET){}
};


struct netCommand4C_ClientIsNotResponce : netCommandGeneral
{
	netCommand4C_ClientIsNotResponce(string& _clientNotResponceList) : netCommandGeneral(NETCOM_4C_ID_CLIENT_IS_NOT_RESPONCE){
		clientNotResponceList=_clientNotResponceList;
	}
	netCommand4C_ClientIsNotResponce(XBuffer& in) : netCommandGeneral(NETCOM_4C_ID_CLIENT_IS_NOT_RESPONCE){
		in > StringInWrapper(clientNotResponceList);
	}
	void Write(XBuffer& out) const {
		out < StringOutWrapper(clientNotResponceList);
	}
	string clientNotResponceList;
};




struct netCommand4C_ContinueGameAfterHostMigrate : netCommandGeneral
{
	netCommand4C_ContinueGameAfterHostMigrate() : netCommandGeneral(NETCOM_4C_ID_CONTINUE_GAME_AFTER_HOST_MIGRATE){}
	netCommand4C_ContinueGameAfterHostMigrate(XBuffer& in) : netCommandGeneral(NETCOM_4C_ID_CONTINUE_GAME_AFTER_HOST_MIGRATE){}
	//void Write(XBuffer& out) const;
};

struct netCommand4H_StartLoadGame : netCommandGeneral
{
	netCommand4H_StartLoadGame() : netCommandGeneral(NETCOM_4H_ID_START_LOAD_GAME){
		v=0;
	}
	netCommand4H_StartLoadGame(XBuffer& in) : netCommandGeneral(NETCOM_4H_ID_START_LOAD_GAME){
		in > v;
	}
	void Write(XBuffer& out) const {
		out < v;
	};
	int v;
};

struct netCommand4C_CurrentMissionDescriptionInfo : netCommandGeneral
{
	MissionDescription missionDescription_;

	netCommand4C_CurrentMissionDescriptionInfo(MissionDescription& missionDescription) : netCommandGeneral(NETCOM_4C_ID_CUR_MISSION_DESCRIPTION_INFO) {
		missionDescription_=missionDescription;
	}

	netCommand4C_CurrentMissionDescriptionInfo(XBuffer& in) : netCommandGeneral(NETCOM_4C_ID_CUR_MISSION_DESCRIPTION_INFO) {
		//missionDescription_.read(in);
		missionDescription_.simpleRead(in);
	}

	void Write(XBuffer& out) const {
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
	void Write(XBuffer& out) const {
		out.write(&idxPlayerData_, sizeof(idxPlayerData_));
		out.write(&newBelligerent_, sizeof(newBelligerent_));
	};
	int idxPlayerData_;
	terBelligerent newBelligerent_;
};

struct netCommand4H_ChangePlayerColor : netCommandGeneral
{
	netCommand4H_ChangePlayerColor(int idxPlayerData, int newColor) : netCommandGeneral(NETCOM_4H_ID_CHANGE_PLAYER_COLOR){
		idxPlayerData_=idxPlayerData;
		newColor_=newColor;
	}
	netCommand4H_ChangePlayerColor(XBuffer& in) : netCommandGeneral(NETCOM_4H_ID_CHANGE_PLAYER_COLOR){
		in.read(&idxPlayerData_, sizeof(idxPlayerData_));
		in.read(&newColor_, sizeof(newColor_));
	}
	void Write(XBuffer& out) const {
		out.write(&idxPlayerData_, sizeof(idxPlayerData_));
		out.write(&newColor_, sizeof(newColor_));
	};
	int idxPlayerData_;
	int newColor_;
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
	void Write(XBuffer& out) const {
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
	void Write(XBuffer& out) const {
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
	void Write(XBuffer& out) const {
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
	void Write(XBuffer& out) const {
		out.write(&idxPlayerData_, sizeof(idxPlayerData_));
		out.write(&handicap_, sizeof(handicap_));
	};
	int idxPlayerData_;
	int handicap_;
};


struct netCommand4H_ChangeMap : netCommandGeneral
{
	MissionDescription missionDescription_;

	netCommand4H_ChangeMap(MissionDescription& missionDescription) : netCommandGeneral(NETCOM_4H_ID_CHANGE_MAP){
		missionDescription_=missionDescription;
	}
	netCommand4H_ChangeMap(XBuffer& in) : netCommandGeneral(NETCOM_4H_ID_CHANGE_MAP){
		missionDescription_.read(in);
	}
	void Write(XBuffer& out) const {
		missionDescription_.write(out);
	}
};





struct terEventControlServerTime : netCommandGeneral
{
	float scale;

	terEventControlServerTime(float s) : netCommandGeneral(EVENT_ID_SERVER_TIME_CONTROL){ scale = s; }
	terEventControlServerTime(XBuffer& in);

	void Write(XBuffer& out) const ;
};

//-------------------------------

struct GameInfo
{
	DWORD id;
	char  Name[PERIMETER_CONTROL_NAME_SIZE];
};

//-------------------------------

extern LogStream netLog;


#endif
