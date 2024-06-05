#ifndef PERIMETER_SERVERLIST_H
#define PERIMETER_SERVERLIST_H

#include "Localization.h"

//Contains info about relay
struct ServerListRelay {
    std::string address = {};
    NetAddress net_address = {};
    NetConnection* connection = nullptr;
    bool operational = false;
    uint64_t ping = 0;
    uint32_t last_attempt = 0;
    uint32_t attempts = 0;
};

using GameInfoKey = uint32_t;

//Contains info about host/room and game itself
struct GameInfo {
    ///Address of game server or relay server to connect
    std::string gameHost = {};
    ///Ping from the host
    uint64_t gameRelayPing = 0;
    ///Room ID for relay, if 0 then gameHost is considered a normal game server
    NetRoomID gameRoomID = 0;
    ///When was room created in unix epoch
    uint64_t gameHostCreatedAt = 0;
    
    //Game info
    LocalizedText gameName = {};
    std::string gameVersion = {};
    uint8_t maximumPlayers = 0;
    uint8_t currentPlayers = 0;
    bool hasPassword = false;
    bool gameStarted = false;
    bool gameClosed = false;
    uint32_t ping = 0;
    std::string scenario = {};
    GAME_CONTENT gameContent = CONTENT_NONE;
    
    GameInfoKey computeKey() const;
};

class ServerList {
private:
    bool findingHosts = false;
    std::vector<ServerListRelay> relays = {};
    ///List updated from 2nd thread with data from relay
    std::unordered_map<GameInfoKey, GameInfo> lastRelayGameInfoList = {};
    uint32_t lastRelayFetch = 0;
    ///List updated from other lists in 1st thread
    std::vector<GameInfo> gameInfoList = {};
    ///Flag to know if gameHostInfoList needs updating
    bool listNeedUpdate = false;

    bool processRelayConnection(ServerListRelay& relay);
    void closeRelayConnections(bool only_destroy);

public:
    ServerList();
    ~ServerList();
    void startFind();
    void stopFind();
    void fetchRelayHostInfoList();
    void refreshHostInfoList();
    
    const std::vector<ServerListRelay>& getRelays() const { return relays; }
    const std::vector<GameInfo>& getList() const { return gameInfoList; }
};

#endif //PERIMETER_SERVERLIST_H
