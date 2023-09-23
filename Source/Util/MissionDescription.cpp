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

	belligerent = belligerentIn;
	colorIndex = colorIndexIn;
    setName(name);
	netid=netid_;
}

void PlayerData::setName(const std::string& name) {
    strncpy(playerName, name.c_str(), PLAYER_MAX_NAME_LEN);
}

void PlayerData::setNameInitial(const std::string& name) {
    strncpy(playerNameInitial, name.c_str(), PLAYER_MAX_NAME_LEN);
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
    in.read(playerNameInitial, sizeof(playerNameInitial));
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
    out.write(playerNameInitial, sizeof(playerNameInitial));
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
	originalSaveName = "";
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
    init();
    setChanged();
    xassert(fname);

    gameType_ = gameType;

    if(gameType_ == GT_PLAY_RELL){
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
    
    if (tmp.empty()) {
        savePathKey_.clear();
    } else {
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
    savePathKey_ = string_to_lower(savePathKey_.c_str());
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
    in > StringInWrapper(version.value());
    in > StringInWrapper(worldName_.value());
    in > StringInWrapper(missionName_);
    in > StringInWrapper(missionDescriptionID.value());
    in > StringInWrapper(savePathKey_);
    playersData.clear();
    playersShufflingIndices.clear();
    const uint16_t ver_307[]  = {3, 0, 7};
    if (0 < compare_versions(ver_307, version.value().c_str())) {
        //In 3.0.8+ replays have original save path and allow dynamic sized players data and indices
        in > StringInWrapper(originalSaveName.value());
        uint32_t playersDataLen = 0;
        in > playersDataLen;
        for (uint32_t i = 0; i < playersDataLen; i++) {
            playersData.emplace_back();
            playersShufflingIndices.emplace_back();
            playersData[i].read(in);
            in.read(&playersShufflingIndices[i], sizeof(playersShufflingIndices[0]));
        }
    } else {
        //Older format have always 4 players data and indices in non interleaved way
        for(int i = 0; i < 4; i++) {
            playersData.emplace_back();
            playersData[i].read(in);
        }
        for(int i = 0; i < 4; i++) {
            playersShufflingIndices.emplace_back();
            in.read(&playersShufflingIndices[i], sizeof(playersShufflingIndices[0]));
        }
    }
    GameType tmp_type;
    in.read(&tmp_type, sizeof(tmp_type));
    if (gameType_ != GT_PLAY_RELL) {
        gameType_ = tmp_type;
    }
    in.read(&gameContent,sizeof(gameContent));
    uint32_t difficultyVal = 0;
    in > difficultyVal; difficulty.value() = static_cast<Difficulty>(difficultyVal);
    in > playerAmountScenarioMax;
    in > activePlayerID;
    in > missionNumber;
    in > globalTime;
    in > saveData;
    in > binaryData;
    in > scriptsData;
    if (saveData.length()) {
        saveData.realloc(saveData.tell());
        saveData.set(0);
    }
    if (binaryData.length()) {
        binaryData.realloc(binaryData.tell());
        binaryData.set(0);
    }
    if (scriptsData.length()) {
        scriptsData.realloc(scriptsData.tell());
        scriptsData.set(0);
    }
    refresh();
}

void MissionDescription::write(XBuffer& out) const 
{
    out < StringOutWrapper(currentShortVersion);
    out < StringOutWrapper(worldName_.value());
    out < StringOutWrapper(missionName_);
    out < StringOutWrapper(missionDescriptionID.value());
    out < StringOutWrapper(savePathKey_);
    out < StringOutWrapper(originalSaveName.value());
    uint32_t playersDataLen = static_cast<uint32_t>(playersData.size());
    out < playersDataLen;
	for (int i = 0; i < playersDataLen; i++) {
        playersData[i].write(out);
        out.write(&playersShufflingIndices[i], sizeof(playersShufflingIndices[0]));
    }
    out.write(&gameType_,sizeof(gameType_));
    out.write(&gameContent,sizeof(gameContent));
    uint32_t difficultyVal = difficulty.value();
    out < difficultyVal;
    out < playerAmountScenarioMax;
    out < activePlayerID;
    out < missionNumber;
    out < globalTime;
    out < saveData;
    out < binaryData;
    out < scriptsData;
}

void MissionDescription::simpleRead(XBuffer& in) 
{
    in > StringInWrapper(version.value());
    in > StringInWrapper(worldName_.value());
    in > StringInWrapper(missionName_);
    in > StringInWrapper(savePathKey_);
    in > StringInWrapper(originalSaveName.value());
    uint8_t tu8;
    int16_t ti16;
    in.read(&tu8, sizeof(tu8)); uint32_t playersDataLen = tu8;
    std::string tmp_str;
    playersData.clear();
	for (uint32_t i=0; i < playersDataLen; i++) {
        playersData.emplace_back();
		in.read(&ti16, sizeof(ti16)); playersData[i].playerID = static_cast<int>(ti16);
		in.read(&tu8, sizeof(tu8)); playersData[i].realPlayerType = static_cast<RealPlayerType>(tu8);
		in.read(&tu8, sizeof(tu8)); playersData[i].belligerent = static_cast<terBelligerent>(tu8);
		in.read(&tu8, sizeof(tu8)); playersData[i].colorIndex = static_cast<int>(tu8);
		in.read(&tu8, sizeof(tu8)); playersData[i].clan = static_cast<int>(tu8);
		in.read(&tu8, sizeof(tu8)); playersData[i].difficulty = static_cast<Difficulty>(tu8);
		in.read(&tu8, sizeof(tu8)); playersData[i].handicap = static_cast<int>(tu8);
		in.read(&playersData[i].flag_playerStartReady, sizeof(playersData[i].flag_playerStartReady) );
        in > StringInWrapper(tmp_str);
        playersData[i].setName(tmp_str);
        in > StringInWrapper(tmp_str);
        playersData[i].setNameInitial(tmp_str);
	}
	in.read(&tu8, sizeof(tu8)); playerAmountScenarioMax = static_cast<int>(tu8);
	in.read(&tu8, sizeof(tu8)); gameType_ = static_cast<GameType>(tu8);
	in.read(&tu8, sizeof(tu8)); activePlayerID = static_cast<int>(tu8);
    in.read(&ti16, sizeof(ti16)); missionNumber = static_cast<int>(ti16);
    in.read(&tu8, sizeof(tu8)); gameContent = static_cast<int>(tu8);
    refresh();
}

void MissionDescription::simpleWrite(XBuffer& out) const 
{
    out < StringOutWrapper(currentShortVersion);
    out < StringOutWrapper(worldName_.value());
    out < StringOutWrapper(missionName_);
    out < StringOutWrapper(savePathKey_);
    out < StringOutWrapper(originalSaveName.value());
    uint8_t tu8;
    int16_t ti16;
    tu8=static_cast<uint8_t>(playersData.size());
    out.write(&tu8, sizeof(tu8));
	for (int i=0; i<playersData.size(); i++){
        ti16=static_cast<int16_t>(playersData[i].playerID);			out.write(&ti16, sizeof(ti16));
		tu8=static_cast<uint8_t>(playersData[i].realPlayerType);	out.write(&tu8, sizeof(tu8));
		tu8=static_cast<uint8_t>(playersData[i].belligerent);		out.write(&tu8, sizeof(tu8));
		tu8=static_cast<uint8_t>(playersData[i].colorIndex);		out.write(&tu8, sizeof(tu8));
		tu8=static_cast<uint8_t>(playersData[i].clan);				out.write(&tu8, sizeof(tu8));
		tu8=static_cast<uint8_t>(playersData[i].difficulty);		out.write(&tu8, sizeof(tu8));
		tu8=static_cast<uint8_t>(playersData[i].handicap);			out.write(&tu8, sizeof(tu8));
		out.write(&playersData[i].flag_playerStartReady, sizeof(playersData[i].flag_playerStartReady) );
        out < StringOutWrapper(playersData[i].name());
        out < StringOutWrapper(playersData[i].nameInitial());
	}
	tu8=static_cast<uint8_t>(playerAmountScenarioMax);		out.write(&tu8, sizeof(tu8));
	tu8=static_cast<uint8_t>(gameType_);					out.write(&tu8, sizeof(tu8));
    tu8=static_cast<uint8_t>(activePlayerID);				out.write(&tu8, sizeof(tu8));
    ti16=static_cast<int16_t >(missionNumber);				out.write(&ti16, sizeof(ti16));
    tu8=static_cast<uint8_t>(gameContent);					out.write(&tu8, sizeof(tu8));
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
		playersData[i].flag_playerStartReady = false;
	}
}

bool MissionDescription::setPlayerStartReady(NETID netid, bool state)
{
	setChanged();
	for(int i=0; i<playerAmountScenarioMax; i++){
		if(playersData[i].netid==netid){
			playersData[i].flag_playerStartReady=state;
		}
	}
	return false;
}

bool MissionDescription::isAllRealPlayerStartReady()
{
    int players = 0;
	for (unsigned int i=0; i<playerAmountScenarioMax; i++) {
        switch (playersData[i].realPlayerType) {
            case REAL_PLAYER_TYPE_PLAYER:
                if (!playersData[i].flag_playerStartReady) {
                    return false;
                }
            case REAL_PLAYER_TYPE_AI:
            case REAL_PLAYER_TYPE_PLAYER_AI:
                players++;
                break;
            default:
                break;
        }
	}
	return 1 < players;
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
	for (int c=0; c < playersData.size(); c++) {
		bool error=0;
		for (int i=0; i<playerAmountScenarioMax; i++) {
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
    PlayerData& pd = playersData[idxPlayerData];
	if (pd.realPlayerType==REAL_PLAYER_TYPE_PLAYER || pd.realPlayerType==REAL_PLAYER_TYPE_AI || pd.realPlayerType==REAL_PLAYER_TYPE_PLAYER_AI) {
        pd.setName(pd.nameInitial());
		pd.realPlayerType = REAL_PLAYER_TYPE_OPEN;
        pd.flag_playerStartReady = false;
        pd.netid = NETID_NONE;
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
    
    //Try match by name first
    std::string pd_name = pd.name(); //TODO use string_to_lower once names are UTF8
    for (int i=0; i<playerAmountScenarioMax; i++) {
        PlayerData& player = playersData[i];
        if (player.realPlayerType!=REAL_PLAYER_TYPE_OPEN) {
            continue;
        }
        std::string name = player.nameInitial(); //TODO use string_to_lower once names are UTF8
        if (name==pd_name) {
            player.setName(pd.name());
            player.realPlayerType=REAL_PLAYER_TYPE_PLAYER;
            player.netid=pd.netid;
            result=i;
            break;
        }
    }

    //Allocate first empty player if there is no player with same name
    if (result == -1) {
        for (int i = 0; i < playerAmountScenarioMax; i++) {
            PlayerData& player = playersData[i];
            if (player.realPlayerType == REAL_PLAYER_TYPE_OPEN) {
                player.setName(pd.name());
                player.realPlayerType = REAL_PLAYER_TYPE_PLAYER;
                player.netid = pd.netid;
                result = i;
                break;
            }
        }
    }
	return result;
}

bool MissionDescription::disconnectPlayer2PlayerDataByIndex(unsigned int idx)
{
	setChanged();
	xassert(idx < playerAmountScenarioMax);
	if (idx < playerAmountScenarioMax) {
		if (playersData[idx].realPlayerType == REAL_PLAYER_TYPE_PLAYER) {
            playersData[idx].realPlayerType = REAL_PLAYER_TYPE_OPEN;
        }
        playersData[idx].netid=NETID_NONE;
        return true;
	}
	return false;
}

bool MissionDescription::disconnectPlayer2PlayerDataByNETID(NETID netid)
{
	setChanged();
	for(int i=0; i<playerAmountScenarioMax; i++){
		if(playersData[i].netid==netid){
			return disconnectPlayer2PlayerDataByIndex(i);
		}
	}
	return false;
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
    if (playerIdx < 0 || playerAmountScenarioMax <= playerIdx) return false;
    if(playersData[playerIdx].realPlayerType==REAL_PLAYER_TYPE_AI || playersData[playerIdx].realPlayerType==REAL_PLAYER_TYPE_PLAYER){
		playersData[playerIdx].belligerent=newBelligerent;
		return true;
	}
    return false;
}

bool MissionDescription::changePlayerColor(int playerIdx, int color, bool direction)
{
	setChanged();
    if (playerIdx < 0 || playerAmountScenarioMax <= playerIdx) return false; 
	if (playersData[playerIdx].realPlayerType==REAL_PLAYER_TYPE_AI || playersData[playerIdx].realPlayerType==REAL_PLAYER_TYPE_PLAYER) {
		int colorNew=getUniquePlayerColor(playerIdx, color, direction);
		if(colorNew!=-1) {
			playersData[playerIdx].colorIndex=colorNew;
			return true;
		}
    }
    return false;
}

bool MissionDescription::changePlayerDifficulty(int playerIdx, Difficulty difficulty)
{
	setChanged();
    if (playerIdx < 0 || playerAmountScenarioMax <= playerIdx) return false; 
	if (playersData[playerIdx].realPlayerType==REAL_PLAYER_TYPE_AI || playersData[playerIdx].realPlayerType==REAL_PLAYER_TYPE_PLAYER) {
		playersData[playerIdx].difficulty=difficulty;
		return true;
	}
    return false;
}

bool MissionDescription::changePlayerClan(int playerIdx, int clan)
{
	setChanged();
    if (playerIdx < 0 || playerAmountScenarioMax <= playerIdx) return false; 
	if (playersData[playerIdx].realPlayerType==REAL_PLAYER_TYPE_AI || playersData[playerIdx].realPlayerType==REAL_PLAYER_TYPE_PLAYER) {
		playersData[playerIdx].clan=clan;
		return true;
	}
    return false;
}

bool MissionDescription::changePlayerHandicap(int playerIdx, int handicap)
{
	setChanged();
    if (playerIdx < 0 || playerAmountScenarioMax <= playerIdx) return false; 
	if (playersData[playerIdx].realPlayerType==REAL_PLAYER_TYPE_AI || playersData[playerIdx].realPlayerType==REAL_PLAYER_TYPE_PLAYER) {
		playersData[playerIdx].handicap=handicap;
		return true;
	}
    return false;
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
	for(int i = 0; i < playersData.size(); i++)
		if(playersData[i].realPlayerType != REAL_PLAYER_TYPE_CLOSE && playersData[i].realPlayerType != REAL_PLAYER_TYPE_OPEN)
			playersData[i].playerID = id++;
	for(int i = 0; i < playersData.size(); i++)
		if(playersData[i].realPlayerType == REAL_PLAYER_TYPE_CLOSE || playersData[i].realPlayerType == REAL_PLAYER_TYPE_OPEN)
			playersData[i].playerID = id++;
}

void MissionDescription::setSinglePlayerDifficulty(Difficulty difficutyIn)
{
	difficulty = difficutyIn;
	for (int i = 0; i < playersData.size(); i++) {
        playersData[i].difficulty = difficutyIn;
    }
}

void MissionDescription::shufflePlayers()
{
	shuffle(&playersShufflingIndices[0], &playersShufflingIndices[0] + playerAmountScenarioMax, std::default_random_engine(clocki()));
}

void MissionDescription::clearData() {
    saveData.alloc(0);
    binaryData.alloc(0);
    scriptsData.alloc(0);
}

void MissionDescription::fitPlayerArrays() {
    if (playerAmountScenarioMax < 0) return;
    //This ensures there can't be more player's than possible colors
    playerAmountScenarioMax = std::min(playerAmountScenarioMax, playerAllowedColorSize);
    while (playersData.size() < playerAmountScenarioMax) {
        playersData.emplace_back();
    }
    while (playersShufflingIndices.size() != playersData.size()) {
        if (playersShufflingIndices.size() < playersData.size()) {
            playersShufflingIndices.emplace_back(playersShufflingIndices.size());
        } else {
            playersShufflingIndices.pop_back();
        }
    }
}
