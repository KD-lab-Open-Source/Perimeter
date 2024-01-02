#ifndef PERIMETER_SERVERLIST_H
#define PERIMETER_SERVERLIST_H

//Contains info about host/room and game itself
struct GameInfo {
    ///Address of game server or relay server to connect
    std::string gameHost = {};
    ///Room ID for relay, if 0 then gameHost is considered a normal game server
    NetRoomID gameRoomID = 0;
    
    //Game info
    std::string gameName = {};
    std::string gameVersion = {};
    uint8_t maximumPlayers = 0;
    uint8_t currentPlayers = 0;
    bool hasPassword = false;
    bool gameStarted = false;
    bool gameClosed = false;
    uint32_t ping = 0;
    std::string scenario = {};
    GAME_CONTENT gameContent = CONTENT_NONE;
};

class ServerList {
private:
    bool findingHosts = false;
    NetConnection* relayConnection = nullptr;
    ///List updated from 2nd thread with data from relay
    std::vector<GameInfo> lastRelayGameInfoList = {};
    uint32_t lastRelayFetch = 0;
    ///List updated from other lists in 1st thread
    std::vector<GameInfo> gameInfoList = {};
    ///Flag to know if gameHostInfoList needs updating
    bool listNeedUpdate = false;

    bool checkRelayConnection();

public:
    ServerList();
    ~ServerList();
    void startFind();
    void stopFind();
    void fetchRelayHostInfoList();
    void refreshHostInfoList();
    
    const std::vector<GameInfo>& getList() const { return gameInfoList; };
};

#endif //PERIMETER_SERVERLIST_H
