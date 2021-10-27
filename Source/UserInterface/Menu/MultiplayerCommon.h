#pragma once

extern bool menuChangingDone;
extern char _bCursorVisible;

extern std::vector<MissionDescription> multiplayerMaps;

void loadMultiplayerList();

int getMultiplayerMapNumber(const std::string& saveName);
