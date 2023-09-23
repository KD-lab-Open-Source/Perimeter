#ifndef __MAINMENU_H__
#define __MAINMENU_H__

std::string getItemTextFromBase(const char *keyStr);

const char* getMapName(const char* keyStr);

void loadMapVector(std::vector<MissionDescription>& mapVector, const std::string& path, const std::string& mask, bool replay = false);

void switchGameContent(GAME_CONTENT selected, const std::string& initial_menu);

void checkMissionDescription(int index, std::vector<MissionDescription>& mVect, GameType gameType);

std::string checkMissingContent(const MissionDescription& mission);

void setupMapDescWnd(int index, std::vector<MissionDescription>& mVect, int mapWndID, int mapDescrWndID, int inputWndID = -1, GameType gameType = GT_SINGLE_PLAYER);

void clearMapDescWnd(int mapWndID, int mapDescrWndID, int inputWndID = -1);

void fillList(int listID, std::vector<MissionDescription>& mVect, int mapWndID, int mapDescrWndID, int inputWndID = -1);

void loadBattleList();

void onBattleMenuOpening();

void StartSpace();

bool intfCanHandleInput();

#endif //__MAINMENU_H__
