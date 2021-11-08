#include <set>
#include "StdAfx.h"
#include "NetPlayer.h"
#include "EditFunctions.h"
#include "Scripts/Config.hi"
#include "GameContent.h"
#include "Runtime.h"
#include "files/files.h"

PlayerData::PlayerData()
{
    strcpy(playerName, "");
}

void PlayerData::set(const std::string& name, NETID netid_, int playerIDIn, terBelligerent belligerentIn, int colorIndexIn, RealPlayerType realPlayerTypeIn) 
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
	netid=netid_;
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
    out.write(playerName, sizeof(playerName));
	out.write(netid);
}

//-------------------------------------------------
void MissionDescription::init()
{
	version = currentShortVersion;
	worldName_ = "";
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
    refresh();
}

MissionDescription::MissionDescription(const char* fname, GameType gameType)
        : missionDescriptionID(editMissionDescriptionID)
{
    setChanged();
    xassert(fname);

    gameType_ = gameType;

    if(gameType_ == GT_playRellGame){
        setReelName(fname);
    } else {
        setSaveName(fname);
    }
    
    loadDescription();
}

void MissionDescription::setSaveName(const char* fname)
{
    //Extract the resource relative path for this file, so we can save/send it without breaking compat
    std::string tmp = fname;
    
    //First try full path as resource (existing file)
    filesystem_entry* entry = get_content_entry(tmp);
    
    if (entry) {
        savePathKey_ = entry->key;
    } else {
        //Otherwise try only parent path (new file)
        std::string parent;
        std::string filename;
        split_path_parent(tmp, parent, &filename);
        entry = get_content_entry(parent);
        if (entry) {
            savePathKey_ = entry->key + PATH_SEP + filename;
        } else {
            //Not found? use provided path
            savePathKey_ = convert_path_native(fname);
        }
    }
    
    //Set extension to spg unless is spb (was this ever used?)
    std::string extension = getExtension(savePathKey_, true);
    if (extension != "spb" && extension != "spg") {
        savePathKey_ = setExtension(savePathKey_, "spg");
    }

    //Store resolved path too
    savePathContent_ = resolve_mission_path(savePathKey_);
    
    //Remove difficulty postfix (is this used?)
    std::string savePathBase = setExtension(savePathContent_, nullptr);
    for(int i = 0; i < DIFFICULTY_MAX; i++) {
        const char* str = missionDifficultyPostfix[i];
        if(!strlen(str)) {
            continue;
        }
        size_t pos = savePathBase.rfind(str);
        if(pos != std::string::npos) {
            savePathBase.erase(pos, savePathBase.size() - pos);
        }
    }

    //Get filename of saved binary
    split_path_parent(savePathBase, tmp, &missionName_);

    //Lowercase the key just in case now that we got the filename case stored in savePathContent_
    strlwr(savePathKey_.data());
}

void MissionDescription::setReelName(const char* name)
{
    playReelPath_ = resolve_mission_path(name);

    //Get filename of reel
    std::string tmp;
    split_path_parent(playReelPath_, tmp, &missionNamePlayReelGame);
}

void MissionDescription::read(XBuffer& in) 
{
    in > StringInWrapper(worldName_.value()) > StringInWrapper(missionName_) > StringInWrapper(missionDescriptionID.value()) > StringInWrapper(savePathKey_); 
	in.read(difficulty);
	for(int i = 0; i < NETWORK_PLAYERS_MAX; i++)
		playersData[i].read(in);
	in.read(&playersShufflingIndices[0], sizeof(playersShufflingIndices[0])*NETWORK_PLAYERS_MAX);
	in.read(&playerAmountScenarioMax, sizeof(playerAmountScenarioMax));
	in.read(&gameType_,sizeof(gameType_));
	in.read(&activePlayerID, sizeof(activePlayerID));
    in.read(&missionNumber,sizeof(missionNumber));
    in.read(&gameContent,sizeof(gameContent));
    refresh();
}

void MissionDescription::write(XBuffer& out) const 
{
    out < StringOutWrapper(worldName_.value()) < StringOutWrapper(missionName_) < StringOutWrapper(missionDescriptionID.value()) < StringOutWrapper(savePathKey_); 
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
    in > StringInWrapper(worldName_.value()) > StringInWrapper(missionName_) > StringInWrapper(savePathKey_);
	unsigned char tmp;
	int i;
    std::string tmp_str;
	for(i=0; i<NETWORK_PLAYERS_MAX; i++){
		in.read(&tmp, sizeof(tmp)); playersData[i].playerID=(int)tmp;
		in.read(&tmp, sizeof(tmp)); playersData[i].realPlayerType=(RealPlayerType)tmp;
		in.read(&tmp, sizeof(tmp)); playersData[i].belligerent=(terBelligerent)tmp;
		in.read(&tmp, sizeof(tmp)); playersData[i].colorIndex=(int)tmp;
		in.read(&tmp, sizeof(tmp)); playersData[i].clan=(int)tmp;
		in.read(&tmp, sizeof(tmp)); playersData[i].difficulty=(Difficulty)tmp;
		in.read(&tmp, sizeof(tmp)); playersData[i].handicap=(int)tmp;
		in.read(&playersData[i].flag_playerStartReady, sizeof(playersData[i].flag_playerStartReady) );
        in > StringInWrapper(tmp_str);
        playersData[i].setName(tmp_str);
	}
	in.read(&tmp, sizeof(tmp)); playerAmountScenarioMax=(int)tmp;
	in.read(&tmp, sizeof(tmp)); gameType_=(GameType)tmp;
	in.read(&tmp, sizeof(tmp)); activePlayerID=(int)tmp;
    in.read(&tmp, sizeof(tmp)); missionNumber=(int)tmp;
    in.read(&tmp, sizeof(tmp)); gameContent=(int)tmp;
    refresh();
}

void MissionDescription::simpleWrite(XBuffer& out) const 
{
    out < StringOutWrapper(worldName_.value()) < StringOutWrapper(missionName_) < StringOutWrapper(savePathKey_);
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
        out < StringOutWrapper(playersData[i].name());
	}
	tmp=(unsigned char)playerAmountScenarioMax;		out.write(&tmp, sizeof(tmp));
	tmp=(unsigned char)gameType_;					out.write(&tmp, sizeof(tmp));
    tmp=(unsigned char)activePlayerID;				out.write(&tmp, sizeof(tmp));
    tmp=(unsigned char)missionNumber;				out.write(&tmp, sizeof(tmp));
    tmp=(unsigned char)gameContent;					out.write(&tmp, sizeof(tmp));
}

std::string MissionDescription::resolve_mission_path(const std::string& path) {
    //First try full path as resource (existing file)
    std::string conv = convert_path_content(path);
    //Otherwise try only parent path (new file)
    if (conv.empty()) conv = convert_path_content(path, true);
    //Otherwise just use provided path
    if (conv.empty()) conv = convert_path_native(path);
    return conv;
}

const std::string& MissionDescription::worldName() const {
    return worldName_.value();
}

const std::string& MissionDescription::savePathKey() const {
    return savePathKey_;
}

const std::string& MissionDescription::savePathContent() const {
    return savePathContent_;
}

const std::string& MissionDescription::playReelPath() const {
    return playReelPath_;
}

const std::string& MissionDescription::missionName() const {
    return missionName_;
}

const std::string& MissionDescription::missionDescription() const {
    return missionDescriptionStr_;
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

int MissionDescription::getUniquePlayerColor(int playerIdx, int begColor, bool direction)
{
    std::set<int> used;
    for(int i=0; i<playerAmountScenarioMax; i++){
        if (i == playerIdx) continue;
        if (playersData[i].realPlayerType==REAL_PLAYER_TYPE_AI || playersData[i].realPlayerType==REAL_PLAYER_TYPE_PLAYER){
            used.emplace(playersData[i].colorIndex);
        }
    }
    for (int i = 0; i < playerAllowedColorSize; ++i) {
        int curColor=(begColor + (direction ? i : playerAllowedColorSize-i)) % playerAllowedColorSize;
        if (!used.count(curColor)) {
            return curColor;
        }
    }
	return -1;
}

int MissionDescription::getUniquePlayerClan()
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
    int colorNewPlayer=getUniquePlayerColor(idxPlayerData);
    playersData[idxPlayerData].set("", NETID_NONE, idxPlayerData, BELLIGERENT_EXODUS0, colorNewPlayer, REAL_PLAYER_TYPE_AI);
	//int newClan=getUniquePlayerClan();
	//if(newClan!=-1) playersData[idxPlayerData].clan=newClan;
    playersData[idxPlayerData].clan=idxPlayerData;
}

int MissionDescription::connectNewPlayer2PlayersData(PlayerData& pd)
{
	setChanged();
	int result=-1;
	for(int pidx=0; pidx < playerAmountScenarioMax; pidx++){
		if(playersData[pidx].realPlayerType==REAL_PLAYER_TYPE_OPEN){
			playersData[pidx].set(pd.name(), pd.netid, pidx);
			playersData[pidx].realPlayerType=REAL_PLAYER_TYPE_PLAYER;
			int colorNewPlayer=getUniquePlayerColor(pidx);
			if(colorNewPlayer!=-1) playersData[pidx].colorIndex=colorNewPlayer;
			//int newClan=getUniquePlayerClan();
			//if(newClan!=-1) playersData[pidx].clan=newClan;
            playersData[pidx].clan=pidx;
			result=pidx;
			break;
		}
	}
	return result;
}

int MissionDescription::connectLoadPlayer2PlayersData(PlayerData& pd)
{
	setChanged();
	int result=-1;
	for (int i=0; i<playerAmountScenarioMax; i++) {
        //TODO revisit this, do we need to match name? we should just and use colors as matching
		if (playersData[i].name()==pd.name()) {
			if(playersData[i].realPlayerType==REAL_PLAYER_TYPE_OPEN){
                playersData[i].setName(pd.name());
				playersData[i].realPlayerType=REAL_PLAYER_TYPE_PLAYER;
				playersData[i].netid=pd.netid;
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
		}
        playersData[idx].netid=NETID_NONE;
        return 1;
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

bool MissionDescription::changePlayerColor(int playerIdx, int color, bool direction)
{
	setChanged();
	if(playersData[playerIdx].realPlayerType==REAL_PLAYER_TYPE_AI || playersData[playerIdx].realPlayerType==REAL_PLAYER_TYPE_PLAYER){
		int colorNew=getUniquePlayerColor(playerIdx, color, direction);
		if(colorNew!=-1) {
			playersData[playerIdx].colorIndex=colorNew;
			return 1;
		}
	}
	return 0;
}

bool MissionDescription::changePlayerColor(NETID netid, int color, bool direction)
{
	setChanged();
	for(int i=0; i<playerAmountScenarioMax; i++){
		if(playersData[i].netid==netid){
			return changePlayerColor(i, color, direction);
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
