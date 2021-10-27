#include "StdAfx.h"
#include "NetPlayer.h"
#include "EditFunctions.h"
#include "Scripts/Config.hi"
#include "GameContent.h"
#include "Runtime.h"

PlayerData::PlayerData()
{
    strcpy(playerName, "");
}

void PlayerData::set(int playerIDIn, const std::string& name, terBelligerent belligerentIn, int colorIndexIn, RealPlayerType realPlayerTypeIn) 
{
	playerID = playerIDIn;
	realPlayerType = realPlayerTypeIn;

//	if(isMultiplayer) 
//		realPlayerType=zREAL_PLAYER_TYPE_OPEN;
//	else if(isAIIn) 
//		realPlayerType=REAL_PLAYER_TYPE_AI;
//	else 
//		realPlayerType=REAL_PLAYER_TYPE_PLAYER;

	belligerent = belligerentIn;
	colorIndex = colorIndexIn;
    setName(name);
	netid=0;

	char buf[sizeof(SIMPLE_GAME_CURRENT_VERSION)];
	strncpy(buf, SIMPLE_GAME_CURRENT_VERSION, sizeof(SIMPLE_GAME_CURRENT_VERSION));
	int i;
	for(i=0; i<sizeof(SIMPLE_GAME_CURRENT_VERSION); i++){
		if(SIMPLE_GAME_CURRENT_VERSION[i]=='.' || SIMPLE_GAME_CURRENT_VERSION[i]==0){
			buf[i]=0; break;
		}
	}
	gameVersion=atoi(buf)<<16;
	i++;
	if(i < sizeof(buf)) gameVersion+=atoi(&buf[i]);
}

void PlayerData::setName(const std::string& name) {
    strncpy(playerName, name.c_str(), PLAYER_MAX_NAME_LEN);
}

void PlayerData::read(XBuffer& in) 
{ 
	in.read(playerID);
	in.read(realPlayerType);
	in.read(belligerent);
	in.read(colorIndex);
	in.read(clan);
	in.read(difficulty);
	in.read(handicap);
	in.read(flag_playerStartReady);
	in.read(flag_playerGameReady);
	in.read(gameVersion);
    in.read(playerName, sizeof(playerName));
	in.read(netid);
}

void PlayerData::write(XBuffer& out) const 
{ 
	out.write(playerID);
	out.write(realPlayerType);
	out.write(belligerent);
	out.write(colorIndex);
	out.write(clan);
	out.write(difficulty);
	out.write(handicap);
	out.write(flag_playerStartReady);
	out.write(flag_playerGameReady);
	out.write(gameVersion);
    out.write(playerName, sizeof(playerName));
	out.write(netid);
}

//-------------------------------------------------
void MissionDescription::init()
{
	version = currentShortVersion;
	worldName = "";
	difficulty = DIFFICULTY_HARD;
	missionNumber = - 1;
	playerAmountScenarioMax = 0;
	activePlayerID = 0;
	globalTime = 1;
	gameSpeed = 0;
	gamePaused = false;
	originalSaveName = 0;
	worldID_ = -1;
	gameType_ = GT_SINGLE_PLAYER;
	flag_missionDescriptionUpdate = true;
    gameContent = 0;
}

MissionDescription::MissionDescription()
: missionDescriptionID(editMissionDescriptionID)
{
	init();
    load();
}

void MissionDescription::read(XBuffer& in) 
{
	in > StringInWrapper(worldName.value()) > StringInWrapper(missionName_) > StringInWrapper(missionDescriptionStr_) > StringInWrapper(saveName_) > StringInWrapper(saveNameBinary_); 
	in.read(difficulty);
	for(int i = 0; i < NETWORK_PLAYERS_MAX; i++)
		playersData[i].read(in);
	in.read(&playersShufflingIndices[0], sizeof(playersShufflingIndices[0])*NETWORK_PLAYERS_MAX);
	in.read(&playerAmountScenarioMax, sizeof(playerAmountScenarioMax));
	in.read(&gameType_,sizeof(gameType_));
	in.read(&activePlayerID, sizeof(activePlayerID));
    in.read(&missionNumber,sizeof(missionNumber));
    in.read(&gameContent,sizeof(gameContent));
    load();
}

void MissionDescription::write(XBuffer& out) const 
{ 
	out < StringOutWrapper(worldName.value()) < StringOutWrapper(missionName_) < StringOutWrapper(missionDescriptionStr_) < StringOutWrapper(saveName_) < StringOutWrapper(saveNameBinary_); 
	out.write(difficulty);
	for(int i = 0; i < NETWORK_PLAYERS_MAX; i++)
		playersData[i].write(out);
	out.write(&playersShufflingIndices[0], sizeof(playersShufflingIndices[0])*NETWORK_PLAYERS_MAX);
	out.write(&playerAmountScenarioMax, sizeof(playerAmountScenarioMax));
	out.write(&gameType_,sizeof(gameType_));
	out.write(&activePlayerID, sizeof(activePlayerID));
    out.write(&missionNumber,sizeof(missionNumber));
    out.write(&gameContent,sizeof(gameContent));
}

void MissionDescription::simpleRead(XBuffer& in) 
{ 
	in > StringInWrapper(worldName.value()) > StringInWrapper(missionName_) > StringInWrapper(saveName_) > StringInWrapper(saveNameBinary_);
	unsigned char tmp;
	int i;
    char playerName[PLAYER_MAX_NAME_LEN+1];
	for(i=0; i<NETWORK_PLAYERS_MAX; i++){
		in.read(&tmp, sizeof(tmp)); playersData[i].playerID=(int)tmp;
		in.read(&tmp, sizeof(tmp)); playersData[i].realPlayerType=(RealPlayerType)tmp;
		in.read(&tmp, sizeof(tmp)); playersData[i].belligerent=(terBelligerent)tmp;
		in.read(&tmp, sizeof(tmp)); playersData[i].colorIndex=(int)tmp;
		in.read(&tmp, sizeof(tmp)); playersData[i].clan=(int)tmp;
		in.read(&tmp, sizeof(tmp)); playersData[i].difficulty=(Difficulty)tmp;
		in.read(&tmp, sizeof(tmp)); playersData[i].handicap=(int)tmp;
		in.read(&playersData[i].flag_playerStartReady, sizeof(playersData[i].flag_playerStartReady) );
		//read string
        in.read(playerName, PLAYER_MAX_NAME_LEN);
        playerName[PLAYER_MAX_NAME_LEN] = 0;
        playersData[i].setName(playerName);
	}
	in.read(&tmp, sizeof(tmp)); playerAmountScenarioMax=(int)tmp;
	in.read(&tmp, sizeof(tmp)); gameType_=(GameType)tmp;
	in.read(&tmp, sizeof(tmp)); activePlayerID=(int)tmp;
    in.read(&tmp, sizeof(tmp)); missionNumber=(int)tmp;
    in.read(&tmp, sizeof(tmp)); gameContent=(int)tmp;
    load();
}

void MissionDescription::simpleWrite(XBuffer& out) const 
{ 
	out < StringOutWrapper(worldName.value()) < StringOutWrapper(missionName_) < StringOutWrapper(saveName_) < StringOutWrapper(saveNameBinary_);
	unsigned char tmp;
	int i;
	for(i=0; i<NETWORK_PLAYERS_MAX; i++){
		tmp=(unsigned char)playersData[i].playerID;			out.write(&tmp, sizeof(tmp));
		tmp=(unsigned char)playersData[i].realPlayerType;	out.write(&tmp, sizeof(tmp));
		tmp=(unsigned char)playersData[i].belligerent;		out.write(&tmp, sizeof(tmp));
		tmp=(unsigned char)playersData[i].colorIndex;		out.write(&tmp, sizeof(tmp));
		tmp=(unsigned char)playersData[i].clan;				out.write(&tmp, sizeof(tmp));
		tmp=(unsigned char)playersData[i].difficulty;		out.write(&tmp, sizeof(tmp));
		tmp=(unsigned char)playersData[i].handicap;			out.write(&tmp, sizeof(tmp));
		out.write(&playersData[i].flag_playerStartReady, sizeof(playersData[i].flag_playerStartReady) );
        out.write(playersData[i].name(), PLAYER_MAX_NAME_LEN);
	}
	tmp=(unsigned char)playerAmountScenarioMax;		out.write(&tmp, sizeof(tmp));
	tmp=(unsigned char)gameType_;					out.write(&tmp, sizeof(tmp));
    tmp=(unsigned char)activePlayerID;				out.write(&tmp, sizeof(tmp));
    tmp=(unsigned char)missionNumber;				out.write(&tmp, sizeof(tmp));
    tmp=(unsigned char)gameContent;					out.write(&tmp, sizeof(tmp));
}


void MissionDescription::clearAllPlayerStartReady()
{
	setChanged();
	for(unsigned int i=0; i<playerAmountScenarioMax; i++){
		playersData[i].flag_playerStartReady=0;
	}
}

bool MissionDescription::setPlayerStartReady(NETID netid)
{
	setChanged();
	for(int i=0; i<playerAmountScenarioMax; i++){
		if(playersData[i].netid==netid){
			playersData[i].flag_playerStartReady=true;
		}
	}
	return 0;
}

bool MissionDescription::isAllRealPlayerStartReady()
{
	bool result=1;
	for(unsigned int i=0; i<playerAmountScenarioMax; i++){
		if(playersData[i].realPlayerType==REAL_PLAYER_TYPE_PLAYER)
			result&=playersData[i].flag_playerStartReady;
	}
	return result;
}

int MissionDescription::playersAmount() const 
{
	int cntPlayers=0;
	for(unsigned int i=0; i<playerAmountScenarioMax; i++){
		if(playersData[i].realPlayerType != REAL_PLAYER_TYPE_CLOSE && playersData[i].realPlayerType != REAL_PLAYER_TYPE_OPEN)
			cntPlayers++;
	}
	return cntPlayers;
}

int MissionDescription::playersMaxEasily() const
{
	int cntPlayers=0;
	for(unsigned int i=0; i<playerAmountScenarioMax; i++){
		if(playersData[i].realPlayerType != REAL_PLAYER_TYPE_CLOSE)
			cntPlayers++;
	}
	return cntPlayers;
}


void MissionDescription::clearAllPlayerGameReady(void)
{
	setChanged();
	for(unsigned int i=0; i<playerAmountScenarioMax; i++){
		playersData[i].flag_playerGameReady=0;
	}
}

int MissionDescription::getUniquePlayerColor(int begColor, bool dirBack)
{
	int i,c;
	for(c=0; c<playerAllowedColorSize; c++){
		int curColor;
		if(dirBack==0)curColor=(begColor+c)%playerAllowedColorSize;
		else curColor=(begColor-c)%playerAllowedColorSize;
		bool error=0;
		for(i=0; i<playerAmountScenarioMax; i++){
			if(playersData[i].realPlayerType==REAL_PLAYER_TYPE_AI || playersData[i].realPlayerType==REAL_PLAYER_TYPE_PLAYER){
				if(playersData[i].colorIndex==curColor) {
					error=1;
					break;
				}
			}
		}
		if(error==0) return curColor;
	}
	return -1;
}

int MissionDescription::getUniquePlayerClan(void)
{
	int i, c;
	for(c=0; c<NETWORK_PLAYERS_MAX; c++){
		bool error=0;
		for(i=0; i<playerAmountScenarioMax; i++){
			if(playersData[i].realPlayerType==REAL_PLAYER_TYPE_AI || playersData[i].realPlayerType==REAL_PLAYER_TYPE_PLAYER){
				if(playersData[i].clan==c) {
					error=1;
					break;
				}
			}
		}
		if(error==0) return c;
	}
	return -1;
}

void MissionDescription::disconnect2PlayerData(int idxPlayerData)
{
	setChanged();
	if(playersData[idxPlayerData].realPlayerType==REAL_PLAYER_TYPE_PLAYER || playersData[idxPlayerData].realPlayerType==REAL_PLAYER_TYPE_AI){
		playersData[idxPlayerData].realPlayerType=REAL_PLAYER_TYPE_OPEN;
	}
}

void MissionDescription::connectAI2PlayersData(int idxPlayerData)
{
	setChanged();
	//if(playersData[i].playerID==PlayerData::PLAYER_ID_NONE)
	playersData[idxPlayerData].playerID=idxPlayerData;
	playersData[idxPlayerData].realPlayerType=REAL_PLAYER_TYPE_AI;
	playersData[idxPlayerData].netid=0;
	int colorNewPlayer=getUniquePlayerColor();
	if(colorNewPlayer!=-1) playersData[idxPlayerData].colorIndex=colorNewPlayer;
	int newClan=getUniquePlayerClan();
	if(newClan!=-1) playersData[idxPlayerData].clan=newClan;
}

int MissionDescription::connectNewPlayer2PlayersData(PlayerData& pd, NETID netid)
{
	setChanged();
	int result=-1;
	for(int i=0; i<playerAmountScenarioMax; i++){
		if(playersData[i].realPlayerType==REAL_PLAYER_TYPE_OPEN){
			playersData[i]=pd;
			//if(playersData[i].playerID==PlayerData::PLAYER_ID_NONE)
			playersData[i].playerID=i;
			playersData[i].realPlayerType=REAL_PLAYER_TYPE_PLAYER;
			playersData[i].netid=netid;
			int colorNewPlayer=getUniquePlayerColor();
			if(colorNewPlayer!=-1) playersData[i].colorIndex=colorNewPlayer;
			int newClan=getUniquePlayerClan();
			if(newClan!=-1) playersData[i].clan=newClan;
			result=i;
			break;
		}
	}
	return result;
}

int MissionDescription::connectLoadPlayer2PlayersData(PlayerData& pd, NETID netid)
{
	setChanged();
	int result=-1;
	for (int i=0; i<playerAmountScenarioMax; i++) {
        //TODO revisit this, do we need to match name? we should just and use colors as matching
		if (playersData[i].name()==pd.name() && playersData->colorIndex == pd.colorIndex) {
			if(playersData[i].realPlayerType==REAL_PLAYER_TYPE_OPEN){
                playersData[i].setName(pd.name());
				playersData[i].realPlayerType=REAL_PLAYER_TYPE_PLAYER;
				playersData[i].netid=netid;
				result=i;
				break;
			}
		}
	}
	return result;
}

bool MissionDescription::setPlayerNETID(unsigned int idx, NETID netid)
{
	setChanged();
	xassert(idx < playerAmountScenarioMax);
	if(idx < playerAmountScenarioMax){
		if(playersData[idx].realPlayerType==REAL_PLAYER_TYPE_PLAYER){
			playersData[idx].netid=netid;
			return 1;
		}
	}
	return 0;
}

bool MissionDescription::disconnectPlayer2PlayerDataByIndex(unsigned int idx)
{
	setChanged();
	xassert(idx < playerAmountScenarioMax);
	if(idx < playerAmountScenarioMax){
		if(playersData[idx].realPlayerType==REAL_PLAYER_TYPE_PLAYER){
			playersData[idx].realPlayerType=REAL_PLAYER_TYPE_OPEN;
			playersData[idx].netid=0;
			return 1;
		}
	}
	return 0;
}

bool MissionDescription::disconnectPlayer2PlayerDataByNETID(NETID netid)
{
	setChanged();
	for(int i=0; i<playerAmountScenarioMax; i++){
		if(playersData[i].netid==netid){
			return disconnectPlayer2PlayerDataByIndex(i);
		}
	}
	return 0;
}


void MissionDescription::getAllOtherPlayerName(std::string& outStr)
{
	for(int i=0; i<playerAmountScenarioMax; i++){
		if( (playersData[i].playerID!=activePlayerID) && (playersData[i].realPlayerType==REAL_PLAYER_TYPE_PLAYER) ){
			outStr+=playersData[i].name();
			outStr+="&FFFFFF\n";
		}
	}
}

void MissionDescription::getPlayerName(int _playerID, std::string& outStr)
{
	for(int i=0; i<playerAmountScenarioMax; i++){
		if( (playersData[i].playerID==_playerID) && (playersData[i].realPlayerType==REAL_PLAYER_TYPE_PLAYER) ){
			outStr+=playersData[i].name();
			outStr+='\n';
			break;
		}
	}
}


int MissionDescription::findPlayer(NETID netid)
{
	int result=-1;
	for(int i=0; i<playerAmountScenarioMax; i++){
		if(playersData[i].netid==netid){
			result=i;
			break;
		}
	}
	return result;
}


bool MissionDescription::changePlayerBelligerent(int playerIdx, terBelligerent newBelligerent)
{
	setChanged();
	if(playersData[playerIdx].realPlayerType==REAL_PLAYER_TYPE_AI || playersData[playerIdx].realPlayerType==REAL_PLAYER_TYPE_PLAYER){
		playersData[playerIdx].belligerent=newBelligerent;
		return 1;
	}
	return 0;
}

bool MissionDescription::changePlayerBelligerent(NETID netid, terBelligerent newBelligerent)
{
	setChanged();
	for(int i=0; i<playerAmountScenarioMax; i++){
		if(playersData[i].netid==netid){
			return changePlayerBelligerent(i, newBelligerent);
		}
	}
	return 0;
}

bool MissionDescription::changePlayerColor(int playerIdx, int color, bool dirBack)
{
	setChanged();
	if(playersData[playerIdx].realPlayerType==REAL_PLAYER_TYPE_AI || playersData[playerIdx].realPlayerType==REAL_PLAYER_TYPE_PLAYER){
		int colorNew=getUniquePlayerColor(color, dirBack);
		if(colorNew!=-1) {
			playersData[playerIdx].colorIndex=colorNew;
			return 1;
		}
	}
	return 0;
}

bool MissionDescription::changePlayerColor(NETID netid, int color, bool dirBack)
{
	setChanged();
	for(int i=0; i<playerAmountScenarioMax; i++){
		if(playersData[i].netid==netid){
			return changePlayerColor(i, color, dirBack);
		}
	}
	return 0;
}

bool MissionDescription::changePlayerDifficulty(int playerIdx, Difficulty difficulty)
{
	setChanged();
	if(playersData[playerIdx].realPlayerType==REAL_PLAYER_TYPE_AI || playersData[playerIdx].realPlayerType==REAL_PLAYER_TYPE_PLAYER){
		playersData[playerIdx].difficulty=difficulty;
		return 1;
	}
	return 0;
}

bool MissionDescription::changePlayerDifficulty(NETID netid, Difficulty difficulty)
{
	setChanged();
	for(int i=0; i<playerAmountScenarioMax; i++){
		if(playersData[i].netid==netid){
			return changePlayerDifficulty(i, difficulty);
		}
	}
	return 0;
}

bool MissionDescription::changePlayerClan(int playerIdx, int clan)
{
	setChanged();
	if(playersData[playerIdx].realPlayerType==REAL_PLAYER_TYPE_AI || playersData[playerIdx].realPlayerType==REAL_PLAYER_TYPE_PLAYER){
		playersData[playerIdx].clan=clan;
		return 1;
	}
	return 0;
}

bool MissionDescription::changePlayerClan(NETID netid, int clan)
{
	setChanged();
	for(int i=0; i<playerAmountScenarioMax; i++){
		if(playersData[i].netid==netid){
			return changePlayerClan(i, clan);
		}
	}
	return 0;
}

bool MissionDescription::changePlayerHandicap(int playerIdx, int handicap)
{
	setChanged();
	if(playersData[playerIdx].realPlayerType==REAL_PLAYER_TYPE_AI || playersData[playerIdx].realPlayerType==REAL_PLAYER_TYPE_PLAYER){
		playersData[playerIdx].handicap=handicap;
		return 1;
	}
	return 0;
}

bool MissionDescription::changePlayerHandicap(NETID netid, int handicap)
{
	setChanged();
	for(int i=0; i<playerAmountScenarioMax; i++){
		if(playersData[i].netid==netid){
			return changePlayerHandicap(i, handicap);
		}
	}
	return 0;
}

PlayerData& MissionDescription::getActivePlayerData() 
{
	for (int i = 0; i < playerAmountScenarioMax; i++) {
		if (playersData[i].playerID == activePlayerID) {
			return playersData[i];
		}
	}
	return playersData[0];
}

void MissionDescription::packPlayerIDs()
{
	setChanged();
	int id = 0;
	for(int i = 0; i < NETWORK_PLAYERS_MAX; i++)
		if(playersData[i].realPlayerType != REAL_PLAYER_TYPE_CLOSE && playersData[i].realPlayerType != REAL_PLAYER_TYPE_OPEN)
			playersData[i].playerID = id++;
	for(int i = 0; i < NETWORK_PLAYERS_MAX; i++)
		if(playersData[i].realPlayerType == REAL_PLAYER_TYPE_CLOSE || playersData[i].realPlayerType == REAL_PLAYER_TYPE_OPEN)
			playersData[i].playerID = id++;
}

void MissionDescription::setSinglePlayerDifficulty(Difficulty difficutyIn)
{
	difficulty = difficutyIn;
	for(int i = 0; i < NETWORK_PLAYERS_MAX; i++)
		playersData[i].difficulty = difficutyIn;
}

void MissionDescription::shufflePlayers()
{
	shuffle(&playersShufflingIndices[0], &playersShufflingIndices[0] + playerAmountScenarioMax, std::default_random_engine(clocki()));
}
