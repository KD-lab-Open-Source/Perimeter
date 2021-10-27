#ifndef PERIMETER_SERVERLIST_H
#define PERIMETER_SERVERLIST_H

//Contains game related parameters
struct GameStatusInfo {
    uint8_t maximumPlayers = 0;
    uint8_t currrentPlayers = 0;
    bool flag_gameRun = false;
    int ping = 0;
    std::string world;

    GameStatusInfo() = default;

    GameStatusInfo(uint8_t _maxPlayers, uint8_t _curPlayers, bool _flag_gameRun, int _ping, const std::string& world): GameStatusInfo() {
        set(_maxPlayers, _curPlayers, _flag_gameRun, _ping, world);
    }

    void set(uint8_t _maxPlayers, uint8_t _curPlayers, bool _flag_gameRun, int _ping, const std::string& _world) {
        maximumPlayers=_maxPlayers;
        currrentPlayers=_curPlayers;
        flag_gameRun=_flag_gameRun;
        ping=_ping;
        world=_world;
    }
};

//Contains info about host and game inside host
struct GameHostInfo {
    NetAddress gameHost;
    std::string hostName;
    std::string gameName;
    GameStatusInfo gameStatusInfo;

    GameHostInfo() = default;

    GameHostInfo(const NetAddress* _gameHost, const char * _hostName, const char * _gameName, const GameStatusInfo* gsi): GameHostInfo() {
        set(_gameHost, _hostName, _gameName, gsi);
    }

    void set(const NetAddress* _gameHost, const char * _hostName, const char * _gameName, const GameStatusInfo* gsi){
        if (_gameHost) gameHost = *_gameHost;
        if (_hostName) hostName = _hostName;
        if (_gameName) gameName = _gameName;
        if (gsi) gameStatusInfo = *gsi;
    }
};

class ServerList {
private:
    bool findingHosts = false;

public:
    std::vector<GameHostInfo> gameHostInfoList{};
    
    ServerList();
    ~ServerList();
    void startHostFind();
    void stopHostFind();
    void clearHostInfoList();
    void refreshHostInfoList();
};

#endif //PERIMETER_SERVERLIST_H
