#include <unordered_map>
#include "NetIncludes.h"
#include "NetRelaySerialization.h"
#include "NetRelay.h"
#include "ServerList.h"
#include "codepages/codepages.h"
#include "MainMenu.h"
#include "crc.h"

const int SERVER_LIST_INTERVAL = 3000;
const float SERVER_LIST_ATTEMPT_BACKOFF_RATE = 2.0f;
const int SERVER_LIST_ATTEMPT_BACKOFF_AMOUNT = 1500;

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
    uint64_t room_created_at = 0;
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
        ar & WRAP_OBJECT(room_created_at);
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

GameInfoKey GameInfo::computeKey() const {
    GameInfoKey key = crc32(reinterpret_cast<const unsigned char*>(gameName.text.c_str()), gameName.text.size(), startCRC32);
    if (gameHostCreatedAt != 0) {
        key = crc32(reinterpret_cast<const unsigned char*>(&gameHostCreatedAt), sizeof(gameHostCreatedAt), key);
    }
    return key;
}

ServerList::ServerList() {
    std::vector<std::string> relay_addresses;
    getPrimaryNetRelayAddresses(relay_addresses);
    if (relay_addresses.empty()) {
        fprintf(stderr, "Error connecting to relay: no relay addresses found\n");
    } else {
        for (auto& addr: relay_addresses) {
            ServerListRelay relay;
            relay.address = addr;
            relays.push_back(relay);
        }
    }
}

ServerList::~ServerList() {
    closeRelayConnections(true);
}

void ServerList::startFind() {
    if (findingHosts) return;
    printf("ServerList::startFind\n");
    findingHosts = true;
}

void ServerList::stopFind() {
    if (!findingHosts) return;
    printf("ServerList::stopFind\n");
    findingHosts = false;
    listNeedUpdate = false;
    lastRelayGameInfoList.clear();
    lastRelayFetch = 0;
    closeRelayConnections(false);
}

void ServerList::closeRelayConnections(bool only_destroy) {
    for (auto& relay : relays) {
        relay.ping = 0;
        if (relay.connection) {
            if (!only_destroy) {
                closeNetRelay(relay.connection);
            }
            delete relay.connection;
            relay.connection = nullptr;
        }
    }
}

bool ServerList::processRelayConnection(ServerListRelay& relay) {
    //Check if we are attempting too often, do a backoff
    int steps = static_cast<int>(xm::ceil(static_cast<float>(relay.attempts) / SERVER_LIST_ATTEMPT_BACKOFF_RATE));
    steps = steps * steps * SERVER_LIST_ATTEMPT_BACKOFF_AMOUNT;
    if (relay.last_attempt + steps > clocki()) {
        return false;
    }
    if (!relay.operational) {
        relay.attempts += 1;
    }
    
    //Prepare connection
    if (!relay.connection) {
        relay.connection = new NetConnection(nullptr, NETID_NONE);        
    }
    relay.last_attempt = clocki();
    if (!relay.connection->hasTransport()) {
        if (!NetAddress::resolve(relay.net_address, relay.address, NET_RELAY_DEFAULT_PORT)) {
            fprintf(stderr, "Error connecting to relay: couldn't resolve address '%s'\n", relay.address.c_str());
            return false;
        }

        NetTransport *transport = NetTransport::create(relay.net_address);
        if (!transport) {
            fprintf(stderr, "Error connecting to relay '%s': no transport\n", relay.address.c_str());
            return false;
        }
        relay.connection->set_transport(transport, NETID_RELAY);
        if (!relay.connection->hasTransport()) {
            fprintf(stderr, "Error connecting to relay '%s': error setting transport\n", relay.address.c_str());
            return false;
        }
    }

    //Do the request to list lobbies to relay
    static NetRelayMessage_PeerListLobbies msg;
    static NetRelayMessage_RelayListLobbies response;
    uint64_t sent_time = clock_us();
    bool ok = sendNetRelayMessage(relay.connection, &msg, NETID_NONE);
    if (ok) ok = receiveNetRelayMessage(relay.connection, NETID_NONE, &response, RELAY_MSG_RELAY_LIST_LOBBIES);

    if (ok) {
        relay.ping = (clock_us() - sent_time) / 1000;
        XPrmIArchive ia;
        std::swap(ia.buffer(), response.data);
        ia.reset();
        std::vector<NetRelay_LobbyWithRooms> lobbies = {};
        ia >> lobbies;
        for (auto& lobby : lobbies) {
            for (auto& room : lobby.rooms) {
                std::string gameName;
                for (char c : room.room_name) {
                    if (c < ' ') continue;
                    gameName += c;
                }
                std::string locale = room.extra_data.locale;
                GameInfo info;
                info.gameHost = lobby.host.getAddress();
                info.gameRelayPing = relay.ping;
                info.gameRoomID = room.room_id;
                info.gameHostCreatedAt = room.room_created_at;
                info.gameName = LocalizedText(gameName, locale);
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
                
                GameInfoKey info_key = info.computeKey();
                if (lastRelayGameInfoList.count(info_key)) {
                    const GameInfo& info_existing = lastRelayGameInfoList[info_key];
                    if (info_existing.gameRelayPing > info.gameRelayPing) {
                        //We found the same room with lower ping, use this relay instead
                        lastRelayGameInfoList[info_key] = info;
                    }
                } else {
                    lastRelayGameInfoList.insert(std::pair(info_key, info));
                }
            }
        }
    }
    
    return ok;
}

void ServerList::fetchRelayHostInfoList() {
    if (!findingHosts || (lastRelayFetch + SERVER_LIST_INTERVAL > clocki())) return;
    MTL();
    lastRelayFetch = clocki();
    lastRelayGameInfoList.clear();

    for (auto& relay : relays) {
        bool operational_before = relay.operational;
        relay.operational = processRelayConnection(relay);
        if (!relay.operational) {
            relay.ping = 0;
            closeNetRelay(relay.connection);
        }
        if (operational_before != relay.operational) {
            printf(
                    "Relay '%s' status: %s ping: %" PRIu64 " attempts: %" PRIu32 "\n",
                    relay.address.c_str(),
                    relay.operational ? "OK" : "Error",
                    relay.ping,
                    relay.attempts
            );
        }
    }

    //printf("ServerList::fetchRelayHostInfoList: found %" PRIsize "\n", gameList.size());
    listNeedUpdate = true;
}

void ServerList::refreshHostInfoList() {
    if (!findingHosts || !listNeedUpdate) return;
    listNeedUpdate = false;
    gameInfoList.clear();
    for (auto& host : lastRelayGameInfoList) {
        gameInfoList.emplace_back(host.second);
    }
    //printf("ServerList::refreshHostInfoList: %" PRIsize " -> %" PRIsize "\n", lastRelayGameInfoList.size(), gameInfoList.size());
}