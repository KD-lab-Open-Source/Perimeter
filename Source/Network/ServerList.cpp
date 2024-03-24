#include "NetIncludes.h"
#include "NetRelay.h"
#include "ServerList.h"

const int SERVER_LIST_INTERVAL = 3000;

struct NetRelay_RoomInfoExtraData {
    std::string scenario = {};
    std::string game_content = {};

    SERIALIZE(ar) {
        ar & WRAP_OBJECT(scenario);
        ar & WRAP_OBJECT(game_content);
    }
};

struct NetRelay_RoomInfo {
    uint64_t room_id = 0;
    std::string game_type = {};
    std::string game_version = {};
    std::string room_name = {};
    bool room_password = false;
    bool room_closed = false;
    bool game_started = false;
    uint16_t players_count = 0;
    uint16_t players_max = 0;
    uint32_t ping = 0;
    NetRelay_RoomInfoExtraData extra_data = {};

    SERIALIZE(ar) {
        ar & WRAP_OBJECT(room_id);
        ar & WRAP_OBJECT(game_type);
        ar & WRAP_OBJECT(game_version);
        ar & WRAP_OBJECT(room_name);
        ar & WRAP_OBJECT(room_password);
        ar & WRAP_OBJECT(room_closed);
        ar & WRAP_OBJECT(game_started);
        ar & WRAP_OBJECT(players_count);
        ar & WRAP_OBJECT(players_max);
        ar & WRAP_OBJECT(ping);
        ar & WRAP_OBJECT(extra_data);
    }
};

struct NetRelay_LobbyWithRooms {
    std::string host_tcp = {};
    std::string host_ws = {};
    std::vector<NetRelay_RoomInfo> rooms = {};

    SERIALIZE(ar) {
        ar & WRAP_OBJECT(host_tcp);
        ar & WRAP_OBJECT(host_ws);
        ar & WRAP_OBJECT(rooms);
    }
};

ServerList::ServerList() {
    relayConnection = new NetConnection(nullptr, NETID_NONE);
}

ServerList::~ServerList() {
    delete relayConnection;
}

void ServerList::startFind() {
    if (findingHosts) return;
#ifdef PERIMETER_DEBUG
    printf("ServerList::startFind\n");
#endif
    findingHosts = true;
}

void ServerList::stopFind() {
    if (!findingHosts) return;
#ifdef PERIMETER_DEBUG
    printf("ServerList::stopFind\n");
#endif
    findingHosts = false;
    listNeedUpdate = false;
    lastRelayGameInfoList.clear();
    lastRelayFetch = 0;
    closeNetRelay(relayConnection);
}

bool ServerList::checkRelayConnection() {
    if (relayConnection->hasTransport()) {
        return true;
    }
    
    NetAddress conn;
    if (!getNetRelayAddress(conn)) {
        return false;
    }
    
    TCPsocket socket = conn.openTCP();
    if (!socket) {
#if !defined(PERIMETER_DEBUG) || 1
        stopFind();
#endif
        return false;
    }
    
    NetTransportTCP* transport = new NetTransportTCP(socket);
    relayConnection->set_transport(transport, NETID_RELAY);
    return relayConnection->hasTransport();
}

void ServerList::fetchRelayHostInfoList() {
    
    if (!findingHosts || (lastRelayFetch + SERVER_LIST_INTERVAL > clocki())) return;
    lastRelayFetch = clocki();
    lastRelayGameInfoList.clear();
    
    //Do the request to relay
    static NetRelayMessage_PeerListRooms msg;
    static NetRelayMessage_RelayListLobbies response;
    bool ok = checkRelayConnection();
    if (ok) ok = sendNetRelayMessage(relayConnection, &msg, NETID_NONE);
    if (ok) ok = receiveNetRelayMessage(relayConnection, NETID_NONE, &response, RELAY_MSG_RELAY_LIST_LOBBIES);
    
    if (ok) {
        XPrmIArchive ia;
        std::swap(ia.buffer(), response.data);
        ia.reset();
        std::vector<NetRelay_LobbyWithRooms> lobbies = {};
        ia >> lobbies;
        //ia & WRAP_NAME(lobbies, nullptr);
        for (auto& lobby : lobbies) {
            for (auto& room : lobby.rooms) {
                GameInfo info;
#ifdef GPX
                info.gameHost = lobby.host_ws;
#else
                info.gameHost = lobby.host_tcp;
#endif
                info.gameRoomID = room.room_id;
                info.gameName = room.room_name;
                info.gameVersion = room.game_version;
                info.maximumPlayers = room.players_max;
                info.currentPlayers = room.players_count;
                info.hasPassword = room.room_password;
                info.gameStarted = room.game_started;
                info.gameClosed = room.room_closed;
                info.ping = room.ping;
                info.scenario = room.extra_data.scenario;
                uint32_t game_content = strtoul(room.extra_data.game_content.c_str(), nullptr, 0);
                info.gameContent = static_cast<GAME_CONTENT>(game_content);
                lastRelayGameInfoList.emplace_back(info);
            }
        }
    }
    
    //Cleanup
    if (!ok) {
        closeNetRelay(relayConnection);
    }
    listNeedUpdate = true;
}

void ServerList::refreshHostInfoList() {
    if (!findingHosts || !listNeedUpdate) return;
    listNeedUpdate = false;
    gameInfoList.clear();
    for (auto& host : lastRelayGameInfoList) {
        gameInfoList.emplace_back(host);
    }
}