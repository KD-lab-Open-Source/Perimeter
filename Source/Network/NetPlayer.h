#ifndef __PERIMETER_PLAYER_H__
#define __PERIMETER_PLAYER_H__

#include "UnitAttribute.h"
#include "Serialization.h"
#include "NetID.h"

const int PLAYER_MAX_NAME_LEN = 64;

struct SavePrm;

enum RealPlayerType {
	REAL_PLAYER_TYPE_CLOSE,
	REAL_PLAYER_TYPE_OPEN,
	REAL_PLAYER_TYPE_PLAYER,
	REAL_PLAYER_TYPE_AI,
	REAL_PLAYER_TYPE_WORLD
};

DECLARE_ENUM_DESCRIPTOR(RealPlayerType)

struct PlayerData {
private:
    char playerName[PLAYER_MAX_NAME_LEN] = "";
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
	unsigned int gameVersion = 0;
	NETID netid = 0;
	
	PlayerData();

	void set(int playerIDIn, const std::string& name, terBelligerent belligerentIn = BELLIGERENT_EXODUS0, int colorIndexIn = 0, RealPlayerType realPlayerTypeIn = REAL_PLAYER_TYPE_PLAYER);

    const char* name() const { return playerName; }
    void setName(const std::string& name);

	void read(XBuffer& in);
	void write(XBuffer& out) const;

    SERIALIZE(ar) {
        int compAndUserID = 0; //Not used anymore but we need to load existing saves
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
        setName(name);

        if(ar.isInput() && !handicap)
            handicap = 100;
    }
};


enum GameType {
	GT_SINGLE_PLAYER,
	GT_SINGLE_PLAYER_NO_AI,
	GT_SINGLE_PLAYER_ALL_AI,
	GT_createMPGame,
	GT_loadMPGame,
	GT_playRellGame
};

class MissionDescription 
{
public:
	MissionDescription();
	explicit MissionDescription(const char* save_name, GameType gameType = GT_SINGLE_PLAYER);
    void init();
    void load();

	bool loadMission(SavePrm& savePrm) const; 
	bool saveMission(const SavePrm& savePrm, bool userSave) const; 
	void restart();

	void setSaveName(const char* name);
	void setReelName(const char* name);

	void read(XBuffer& in);
	void write(XBuffer& out) const;

	void simpleRead(XBuffer& in);
	void simpleWrite(XBuffer& out) const;

	const char* saveName() const { return saveName_.c_str(); }
	const char* saveNameBinary() const { return saveNameBinary_.c_str(); }
	const char* missionName() const { return missionName_.c_str(); }
	const char* missionDescription() const { return missionDescriptionStr_.c_str(); }

	int worldID() const { return worldID_; }

	bool isMultiPlayer() const { return gameType_ == GT_createMPGame || gameType_ == GT_loadMPGame;	}

	int playersAmountScenarioMax() const { return playerAmountScenarioMax; }
	int playersAmount() const;
	int playersMaxEasily() const;

	void packPlayerIDs();

	void clearAllPlayerStartReady(void);

	bool setPlayerStartReady(NETID netid);
	bool isAllRealPlayerStartReady(void);
	void clearAllPlayerGameReady(void);

	void disconnect2PlayerData(int idxPlayerData);
	void connectAI2PlayersData(int idxPlayerData);
	int connectNewPlayer2PlayersData(PlayerData& pd, NETID netid);
	int connectLoadPlayer2PlayersData(PlayerData& pd, NETID netid);
	bool disconnectPlayer2PlayerDataByIndex(unsigned int idx);
	bool disconnectPlayer2PlayerDataByNETID(NETID netid);
	bool setPlayerNETID(unsigned int idx, NETID netid);

	int getUniquePlayerColor(int begColor=0, bool dirBack=0);
	bool changePlayerColor(int playerIdx, int color, bool dirBack=0);
	bool changePlayerColor(NETID netid, int color, bool dirBack=0);

	bool changePlayerDifficulty(int playerIdx, Difficulty difficulty);
	bool changePlayerDifficulty(NETID netid, Difficulty difficulty);

	int getUniquePlayerClan();
	bool changePlayerClan(int playerIdx, int clan);
	bool changePlayerClan(NETID netid, int clan);

	bool changePlayerHandicap(int playerIdx, int handicap);
	bool changePlayerHandicap(NETID netid, int handicap);

	void getAllOtherPlayerName(std::string& outStr);
	void getPlayerName(int _playerID, std::string& outStr);

	int findPlayer(NETID netid);

	bool changePlayerBelligerent(int playerIdx, terBelligerent newBelligerent);
	bool changePlayerBelligerent(NETID netid, terBelligerent newBelligerent);

	bool isChanged(void){return flag_missionDescriptionUpdate;}
	void setChanged(void){flag_missionDescriptionUpdate=true;}
	void clearChanged(void){flag_missionDescriptionUpdate=0;}

	void setSinglePlayerDifficulty(Difficulty difficuty);

	void shufflePlayers();

	PlayerData& getActivePlayerData();

    SERIALIZE(ar) {
        ar & WRAP_OBJECT(version);
        ar & TRANSLATE_OBJECT(worldName, "Имя мира");
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
	PrmString worldName;
	CustomString missionDescriptionID;
	EnumWrapper<Difficulty> difficulty;
	PlayerData playersData[4];
	int missionNumber;
	int playerAmountScenarioMax;
	int playersShufflingIndices[4];
	int activePlayerID;
	int globalTime;
	float gameSpeed;
	bool gamePaused;
    BitVector<GAME_CONTENT> gameContent;
	PrmString originalSaveName;

	std::string fileNamePlayReelGame;
	std::string missionNamePlayReelGame;
	GameType gameType_;
	bool flag_missionDescriptionUpdate;

private:
	int worldID_;
	std::string saveName_;
	std::string saveNameBinary_;
	std::string missionName_;
	std::string missionDescriptionStr_;
	unsigned int serverRnd_;
};

#endif
