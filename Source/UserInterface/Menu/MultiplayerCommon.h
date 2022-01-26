#pragma once

extern bool menuChangingDone;
extern char _bCursorVisible;

extern std::vector<MissionDescription> multiplayerMaps;
extern std::vector<MissionDescription> multiplayerSaves;

void loadMultiplayerList();

int getMultiplayerMapNumber(const std::string& missionName);
