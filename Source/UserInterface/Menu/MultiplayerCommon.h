#pragma once

extern bool menuChangingDone;
extern char _bCursorVisible;

extern std::vector<MissionDescription> multiplayerMaps;

int getMultiplayerMapNumber(const std::string& saveName);