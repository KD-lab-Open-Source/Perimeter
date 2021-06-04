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
	REAL_PLAYER_TYPE_WORLD
};

struct PlayerData {
	enum  {
		PLAYER_ID_NONE = -1
	};
	
	int playerID;
	EnumWrapper<RealPlayerType> realPlayerType;
	EnumWrapper<terBelligerent> belligerent;
	int colorIndex;
	int clan;
	EnumWrapper<Difficulty> difficulty;
	int handicap;
	bool flag_playerStartReady;
	bool flag_playerGameReady;
	int compAndUserID;
	unsigned int gameVersion;
	char playerName[64];
	DPNID dpnid;
	
	PlayerData();
	PlayerData(int playerIDIn, const char* name, terBelligerent belligerentIn, int colorIndexIn, RealPlayerType realPlayerTypeIn = REAL_PLAYER_TYPE_PLAYER);

	void set(int playerIDIn, const char* name, terBelligerent belligerentIn, int colorIndexIn, RealPlayerType realPlayerTypeIn = REAL_PLAYER_TYPE_PLAYER);
	
	void setCompAndUserID(const char* computerName, const char* playerName){
		compAndUserID = calcCompAndUserID(computerName, playerName);
	}
	unsigned int calcCompAndUserID(const char* computerName, const char* playerName);

	const char* name() const { return playerName; } 
	void setName(const char* name) { strcpy(playerName, name); }

	void read(XBuffer& in);
	void write(XBuffer& out) const;

	template<class Archive>
	void serialize(Archive& ar) {
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
		string name = playerName;
		ar & WRAP_OBJECT(name);
		setName(name.c_str());
		//ar & WRAP_OBJECT(playerName);

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
	MissionDescription(const char* save_name, GameType gameType = GT_SINGLE_PLAYER);
	void init();

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

	bool setPlayerStartReady(DPNID dpnid);
	bool isAllRealPlayerStartReady(void);
	void clearAllPlayerGameReady(void);

	void disconnect2PlayerData(int idxPlayerData);
	void connectAI2PlayersData(int idxPlayerData);
	int connectNewPlayer2PlayersData(PlayerData& pd, DPNID dpnid);
	int connectLoadPlayer2PlayersData(PlayerData& pd, DPNID dpnid);
	bool disconnectPlayer2PlayerDataByIndex(unsigned int idx);
	bool disconnectPlayer2PlayerDataByDPNID(DPNID dpnid);
	bool setPlayerDPNID(unsigned int idx, DPNID dpnid);

	int getUniquePlayerColor(int begColor=0, bool dirBack=0);
	bool changePlayerColor(int playerIdx, int color, bool dirBack=0);
	bool changePlayerColor(DPNID dpnid, int color, bool dirBack=0);

	bool changePlayerDifficulty(int playerIdx, Difficulty difficulty);
	bool changePlayerDifficulty(DPNID dpnid, Difficulty difficulty);

	int getUniquePlayerClan();
	bool changePlayerClan(int playerIdx, int clan);
	bool changePlayerClan(DPNID dpnid, int clan);

	bool changePlayerHandicap(int playerIdx, int handicap);
	bool changePlayerHandicap(DPNID dpnid, int handicap);

	void getAllOtherPlayerName(string& outStr);
	void getPlayerName(int _playerID, string& outStr);

	int findPlayer(DPNID dpnid);

	bool changePlayerBelligerent(int playerIdx, terBelligerent newBelligerent);
	bool changePlayerBelligerent(DPNID dpnid, terBelligerent newBelligerent);

	bool isChanged(void){return flag_missionDescriptionUpdate;}
	void setChanged(void){flag_missionDescriptionUpdate=true;}
	void clearChanged(void){flag_missionDescriptionUpdate=0;}

	void setSinglePlayerDifficulty(Difficulty difficuty);

	void shufflePlayers();

	PlayerData& getActivePlayerData();

	template<class Archive>
	void serialize(Archive& ar) {
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
	PrmString originalSaveName;

	unsigned int quantAmountInPlayReel;
	string fileNamePlayReelGame;
	string missionNamePlayReelGame;
	GameType gameType_;
	bool flag_missionDescriptionUpdate;

private:
	int worldID_;
	string saveName_;
	string saveNameBinary_;
	string missionName_;
	string missionDescriptionStr_;
	unsigned int serverRnd_;
};

#endif
