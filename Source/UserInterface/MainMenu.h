#ifndef __MAINMENU_H__
#define __MAINMENU_H__

enum CreateNetCenterWayType {
	COMMAND_LINE_CREATE_GAME,
	COMMAND_LINE_LOBBY,
	MENU_LAN,
	MENU_ONLINE
};

STARFORCE_API void switchToMultiplayer(CreateNetCenterWayType way);

string getItemTextFromBase(const char *keyStr);

STARFORCE_API void loadMapVector(vector<MissionDescription>& mapVector, const string& path, const string& mask, bool replay = false);

void checkMissionDescription(int index, vector<MissionDescription>& mVect);

void setupMapDescWnd(int index, vector<MissionDescription>& mVect, int mapWndID, int mapDescrWndID, int inputWndID = -1);

void clearMapDescWnd(int mapWndID, int mapDescrWndID, int inputWndID = -1);

STARFORCE_API void fillList(int listID, vector<MissionDescription>& mVect, int mapWndID, int mapDescrWndID, int inputWndID = -1);

STARFORCE_API void loadBattleList();

STARFORCE_API void onBattleMenuOpening();

STARFORCE_API void StartSpace();

bool intfCanHandleInput();

#endif //__MAINMENU_H__
