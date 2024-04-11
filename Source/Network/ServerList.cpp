#include "NetIncludes.h"
#include "NetRelaySerialization.h"
#include "NetRelay.h"
#include "ServerList.h"
#include "codepages/codepages.h"
#include "MainMenu.h"

const int SERVER_LIST_INTERVAL = 3000;

struct NetRelay_RoomInfoExtraData {
    std::string scenario = {};
    std::string game_content = {};
    std::string locale = {};

    SERIALIZE(ar) {
        ar & WRAP_OBJECT(scenario);
        ar & WRAP_OBJECT(game_content);
        ar & WRAP_OBJECT(locale);
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
    NetRelay_LobbyHost host = {};
    std::vector<NetRelay_RoomInfo> rooms = {};

    SERIALIZE(ar) {
        ar & WRAP_OBJECT(host);
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
    
    NetAddress address;
    const char* primary_relay = getPrimaryNetRelayAddress();
    if (!primary_relay || !NetAddress::resolve(address, primary_relay, NET_RELAY_DEFAULT_PORT)) {
        return false;
    }

    NetTransport *transport = NetTransport::create(address);
    if (!transport) {
#if !defined(PERIMETER_DEBUG) || 1
        stopFind();
#endif
        return false;
    }
    
    relayConnection->set_transport(transport, NETID_RELAY);
    return relayConnection->hasTransport();
}

void ServerList::fetchRelayHostInfoList() {
    if (!findingHosts || (lastRelayFetch + SERVER_LIST_INTERVAL > clocki())) return;
    MTL();
    lastRelayFetch = clocki();
    
    //Do the request to list lobbies to relay
    static NetRelayMessage_PeerListLobbies msg;
    static NetRelayMessage_RelayListLobbies response;
    bool ok = checkRelayConnection();
    if (ok) ok = sendNetRelayMessage(relayConnection, &msg, NETID_NONE);
    if (ok) ok = receiveNetRelayMessage(relayConnection, NETID_NONE, &response, RELAY_MSG_RELAY_LIST_LOBBIES);

    if (ok) {
        std::vector<GameInfo> gameList;
        XPrmIArchive ia;
        std::swap(ia.buffer(), response.data);
        ia.reset();
        std::vector<NetRelay_LobbyWithRooms> lobbies = {};
        ia >> lobbies;
        for (auto& lobby : lobbies) {
            for (auto& room : lobby.rooms) {
                std::string locale = room.extra_data.locale;
                GameInfo info;
                info.gameHost = lobby.host.getAddress();
                info.gameRoomID = room.room_id;
                info.gameName = LocalizedText(room.room_name.c_str(), locale);
                info.gameVersion = room.game_version;
                info.maximumPlayers = room.players_max;
                info.currentPlayers = room.players_count;
                info.hasPassword = room.room_password;
                info.gameStarted = room.game_started;
                info.gameClosed = room.room_closed;
                info.ping = room.ping;
                info.scenario = getMapName(room.extra_data.scenario.c_str());
                uint32_t game_content = strtoul(room.extra_data.game_content.c_str(), nullptr, 0);
                info.gameContent = static_cast<GAME_CONTENT>(game_content);
                gameList.emplace_back(info);
            }
        }
        lastRelayGameInfoList.clear();
        lastRelayGameInfoList = gameList;
        listNeedUpdate = true;
    }
    
    //Cleanup
    if (!ok) {
        closeNetRelay(relayConnection);
    }
}

void ServerList::refreshHostInfoList() {
    if (!findingHosts || !listNeedUpdate) return;
    listNeedUpdate = false;
    gameInfoList.clear();
    for (auto& host : lastRelayGameInfoList) {
        gameInfoList.emplace_back(host);
    }
}