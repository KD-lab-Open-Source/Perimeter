#ifndef __PERIMETER_PLAYER_H__
#define __PERIMETER_PLAYER_H__

#include "UnitAttribute.h"
#include "Serialization.h"

struct SavePrm;

enum RealPlayerType {
	REAL_PLAYER_TYPE_CLOSE,
	REAL_PLAYER_TYPE_OPEN,
	REAL_PLAYER_TYPE_PLAYER,
    REAL_PLAYER_TYPE_AI,
    REAL_PLAYER_TYPE_PLAYER_AI, //Player that is absent and AI takes over until comes back
	REAL_PLAYER_TYPE_WORLD
};

DECLARE_ENUM_DESCRIPTOR(RealPlayerType)

struct PlayerData {
private:
    char playerName[PLAYER_MAX_NAME_LEN] = "";
    char playerNameInitial[PLAYER_MAX_NAME_LEN] = "";
public:
	enum  {
		PLAYER_ID_NONE = -1
	};
	
	int playerID = PLAYER_ID_NONE;
	EnumWrapper<RealPlayerType> realPlayerType = REAL_PLAYER_TYPE_CLOSE;
	EnumWrapper<terBelligerent> belligerent = BELLIGERENT_EXODUS0;
	int colorIndex = 0;
	int clan = -1;
	EnumWrapper<Difficulty> difficulty = DIFFICULTY_HARD;
	int handicap = 100;
	bool flag_playerStartReady = false;
	bool flag_playerGameReady = false;
	NETID netid = NETID_NONE;
	
	PlayerData();

	void set(const std::string& name = "", NETID netid = NETID_NONE, int playerIDIn = PLAYER_ID_NONE, terBelligerent belligerentIn = BELLIGERENT_EXODUS0, int colorIndexIn = 0, RealPlayerType realPlayerTypeIn = REAL_PLAYER_TYPE_PLAYER);

    const char* name() const { return playerName; }
    const char* nameInitial() const { return playerNameInitial; }
    void setName(const std::string& name);
    void setNameInitial(const std::string& name);

	void read(XBuffer& in);
	void write(XBuffer& out) const;

    SERIALIZE(ar) {
        //These are not used anymore but we need to load existing saves
        int compAndUserID = 0;
        unsigned int gameVersion = 0;
        
        ar & WRAP_OBJECT(playerID);
        ar & WRAP_OBJECT(realPlayerType);
        ar & TRANSLATE_OBJECT(belligerent, "Сторона");
        ar & TRANSLATE_OBJECT(colorIndex, "Цвет");
        ar & TRANSLATE_OBJECT(clan, "Клан");
        ar & TRANSLATE_OBJECT(difficulty, "Сложность");
        ar & TRANSLATE_OBJECT(handicap, "Гандикап");

        ar & WRAP_OBJECT(flag_playerStartReady);
        ar & WRAP_OBJECT(flag_playerGameReady);
        ar & WRAP_OBJECT(compAndUserID);
        ar & WRAP_OBJECT(gameVersion);
        std::string name = playerName;
        ar & WRAP_OBJECT(name);

        if(ar.isInput()) {
            setName(name);
            
            if(!handicap) {
                handicap = 100;
            }
        }
    }
};

enum GameType {
	GT_SINGLE_PLAYER,
	GT_SINGLE_PLAYER_NO_AI,
	GT_SINGLE_PLAYER_ALL_AI,
	GT_MULTI_PLAYER_CREATE,
	GT_MULTI_PLAYER_LOAD,
	GT_PLAY_RELL,
    GT_MULTI_PLAYER_RESTORE_PARTIAL,
    GT_MULTI_PLAYER_RESTORE_FULL
};

class MissionDescription 
{
public:
	MissionDescription();
	explicit MissionDescription(const char* save_name, GameType gameType = GT_SINGLE_PLAYER);
    void init();
    void refresh();
    void loadDescription();

	bool loadMission(SavePrm& savePrm) const;
    void loadIntoMemory();
	bool saveMission(const SavePrm& savePrm, bool userSave) const; 
	void restart();

	void setSaveName(const char* name);
	void setReelName(const char* name);

	void read(XBuffer& in);
	void write(XBuffer& out) const;

	void simpleRead(XBuffer& in);
	void simpleWrite(XBuffer& out) const;

    const std::string& worldName() const;
    const std::string& savePathKey() const;
    const std::string& savePathContent() const;
	const std::string& playReelPath() const;
	const std::string& missionName() const;
	const std::string& missionDescription() const;

	int worldID() const { return worldID_; }

	bool isMultiPlayer() const { return gameType_ == GT_MULTI_PLAYER_CREATE || gameType_ == GT_MULTI_PLAYER_LOAD;	}
    bool isCampaign() const { return 0 <= missionNumber; }

	int playersAmountScenarioMax() const { return playerAmountScenarioMax; }
	int playersAmount() const;
	int playersMaxEasily() const;

	void packPlayerIDs();

	void clearAllPlayerStartReady(void);

	bool setPlayerStartReady(NETID netid, bool state);
	bool isAllRealPlayerStartReady(void);
	void clearAllPlayerGameReady(void);

	void disconnect2PlayerData(int idxPlayerData);
	void connectAI2PlayersData(int idxPlayerData);
	int connectNewPlayer2PlayersData(PlayerData& pd);
	int connectLoadPlayer2PlayersData(PlayerData& pd);
	bool disconnectPlayer2PlayerDataByIndex(unsigned int idx);
	bool disconnectPlayer2PlayerDataByNETID(NETID netid);

	int getUniquePlayerColor(int playerIdx, int begColor=0, bool direction=true);
	bool changePlayerColor(int playerIdx, int color, bool direction=true);

	bool changePlayerDifficulty(int playerIdx, Difficulty difficulty);

	int getUniquePlayerClan();
	bool changePlayerClan(int playerIdx, int clan);

	bool changePlayerHandicap(int playerIdx, int handicap);

	void getAllOtherPlayerName(std::string& outStr);
	void getPlayerName(int _playerID, std::string& outStr);

	int findPlayer(NETID netid);

	bool changePlayerBelligerent(int playerIdx, terBelligerent newBelligerent);

	bool isChanged() const { return flag_missionDescriptionUpdate; }
	void setChanged() { flag_missionDescriptionUpdate=true; }
	void clearChanged() { flag_missionDescriptionUpdate=false; }

	void setSinglePlayerDifficulty(Difficulty difficuty);

	void shufflePlayers();

    void fitPlayerArrays();

	PlayerData& getActivePlayerData();

    void clearData();

    SERIALIZE(ar) {
        if (ar.isOutput()) {
            //Always output current version which is serialized with
            extern const char* currentShortVersion;
            PrmString versionOutput = currentShortVersion;
            ar & WRAP_OBJECT(versionOutput);
        } else if (ar.isInput()) {
            ar & WRAP_OBJECT(version);
        }
        ar & TRANSLATE_NAME(worldName_, "worldName", "Имя мира");
        ar & TRANSLATE_NAME(missionDescriptionID, "missionDescription", "Описание миссии");
        ar & TRANSLATE_OBJECT(difficulty, "Уровень сложности");
        ar & TRANSLATE_OBJECT(playersData, "Игроки");
        ar & WRAP_OBJECT(missionNumber);

        ar & TRANSLATE_OBJECT(playerAmountScenarioMax, "Максимальное количество игроков");
        ar & WRAP_OBJECT(playersShufflingIndices);
        ar & WRAP_OBJECT(activePlayerID);

        ar & WRAP_OBJECT(globalTime);
        ar & WRAP_OBJECT(gameSpeed);
        ar & WRAP_OBJECT(gamePaused);
        ar & WRAP_OBJECT(gameContent);

        ar & WRAP_OBJECT(originalSaveName);
    }

	PrmString version;
	CustomString missionDescriptionID;
	EnumWrapper<Difficulty> difficulty;
    std::vector<PlayerData> playersData;
	int missionNumber;
	int playerAmountScenarioMax;
	std::vector<int> playersShufflingIndices;
	int activePlayerID;
	int globalTime;
	float gameSpeed;
	bool gamePaused;
    BitVector<GAME_CONTENT> gameContent;
	PrmString originalSaveName;

	std::string missionNamePlayReelGame;
	GameType gameType_;
	bool flag_missionDescriptionUpdate;

    //These may be empty, used for bundling extra data along with MissionDescription
    XBuffer saveData = XBuffer(0, true); //Contains SavePrm content usually present in .spg
    XBuffer binaryData = XBuffer(0, true); //Contains compressed binary data (.bin, previously .gmp and .dat)
    XBuffer scriptsData = XBuffer(0, true); //Contains Scripts attributes

private:
    std::string resolve_mission_path(const std::string& path);
    
	int worldID_;
    PrmString worldName_;
	std::string savePathKey_;
    std::string savePathContent_;
	std::string missionName_;
	std::string missionDescriptionStr_;
    std::string playReelPath_;
};

#endif
