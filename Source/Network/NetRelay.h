#ifndef PERIMETER_NETRELAY_H
#define PERIMETER_NETRELAY_H

#ifdef EMSCRIPTEN
static const char* NET_RELAY_DEFAULT_ADDRESS = "127.0.0.1";
static const uint16_t NET_RELAY_DEFAULT_PORT = 11655;
#else
static const char* NET_RELAY_DEFAULT_ADDRESS = "relay.kdlab.com";
static const uint16_t NET_RELAY_DEFAULT_PORT = 11654;
#endif

static const uint16_t NET_RELAY_MAX_STRING_LENGTH = 256;
static const uint16_t NET_RELAY_MAX_LIST_ELEMENTS = 128;
static const uint16_t NET_RELAY_MAX_MAP_ELEMENTS = 64;
static const uint8_t NET_RELAY_PROTOCOL_VERSION = 1;
static const uint16_t NET_RELAY_FORMAT_XPRM = 1;

///IDs for messages from/to relay, keep it in sync with relay
enum NetRelayMessageType {
    //Common
    RELAY_MSG_UNKNOWN = 0,
    RELAY_MSG_CLOSE,

    //Sent by peer to relay
    RELAY_MSG_PEER_START [[maybe_unused]] = 0x10000,
    RELAY_MSG_PEER_LIST_LOBBIES,
    RELAY_MSG_PEER_SETUP_ROOM,
    RELAY_MSG_PEER_JOIN_ROOM,
    RELAY_MSG_PEER_LEAVE_ROOM [[maybe_unused]],
    RELAY_MSG_PEER_PING_RESPONSE,
    RELAY_MSG_PEER_CLOSE_PEER,
    RELAY_MSG_PEER_LIST_LOBBY_HOSTS,

    //Sent by relay to peer
    RELAY_MSG_RELAY_START [[maybe_unused]] = 0x20000,
    RELAY_MSG_RELAY_LIST_LOBBIES,
    RELAY_MSG_RELAY_PING,
    RELAY_MSG_RELAY_LIST_PEERS,
    RELAY_MSG_RELAY_ADD_PEER,
    RELAY_MSG_RELAY_REMOVE_PEER,
    RELAY_MSG_RELAY_LIST_LOBBY_HOSTS,
};

struct NetRelayMessage {
    uint8_t protocol_version;
    NetRelayMessageType msg_type;

    explicit NetRelayMessage(NetRelayMessageType msg_type)
        : protocol_version(NET_RELAY_PROTOCOL_VERSION), msg_type(msg_type) {};

    virtual ~NetRelayMessage() = default;

    NO_COPY_CONSTRUCTOR(NetRelayMessage);
    
    virtual void read(XBuffer& in) {};
    virtual void write(XBuffer& out) const {};

    void read_relay_header(XBuffer& in);
    void write_relay_header(XBuffer& out) const;
    
protected:
    static void write_string(XBuffer& out, const std::string& str, uint16_t max_len);
    static void write_list(XBuffer& out, const std::vector<std::string>& list, uint16_t max_elements, uint16_t max_element_len);
    static void write_map(XBuffer& out, const std::unordered_map<std::string, std::string>& list, uint16_t max_elements, uint16_t max_key_len, uint16_t max_value_len);
};

///////// Common /////////

struct NetRelayMessage_Close : NetRelayMessage {
    uint32_t code = 0;
    
    NetRelayMessage_Close()
            : NetRelayMessage(RELAY_MSG_CLOSE) {
    }

    void read(XBuffer& in) override;
    void write(XBuffer& out) const override;
};

///////// Peer to Relay /////////

struct NetRelayMessage_PeerListLobbyHosts : NetRelayMessage {
    NetRelayMessage_PeerListLobbyHosts()
            : NetRelayMessage(RELAY_MSG_PEER_LIST_LOBBY_HOSTS) {
    }

    void write(XBuffer& out) const override;
};

struct NetRelayMessage_PeerListLobbies : NetRelayMessage {
    
    NetRelayMessage_PeerListLobbies()
            : NetRelayMessage(RELAY_MSG_PEER_LIST_LOBBIES) {
    }

    void write(XBuffer& out) const override;
};

struct NetRelayMessage_PeerSetupRoom : NetRelayMessage {
    static const uint16_t RELAY_TOPOLOGY_SERVER_CLIENT = 1;

    ///Public name of room to display in list
    std::string roomName = {};
    ///Is this room password protected?
    bool hasPasword = false;
    ///Is this room closed?
    bool isClosed = false;
    ///Is this game already started?
    bool gameStarted = false;
    ///Current amount of players this room has
    uint16_t playersCount = 0;
    ///Max amount of players this room can hold
    uint16_t playersMax = 0;
    ///Name of mission/scenario to play
    std::string scenarioName = {};
    ///Game contents used
    GAME_CONTENT gameContent = GAME_CONTENT::CONTENT_NONE;

    NetRelayMessage_PeerSetupRoom()
            : NetRelayMessage(RELAY_MSG_PEER_SETUP_ROOM) {
    }

    void write(XBuffer& out) const override;
};

struct NetRelayMessage_PeerJoinRoom : NetRelayMessage {
    NetRoomID room = 0;

    NetRelayMessage_PeerJoinRoom()
            : NetRelayMessage(RELAY_MSG_PEER_JOIN_ROOM) {
    }

    void write(XBuffer& out) const override;
};

struct NetRelayMessage_PeerClosePeer : NetRelayMessage {
    NETID netid = 0;

    NetRelayMessage_PeerClosePeer()
            : NetRelayMessage(RELAY_MSG_PEER_CLOSE_PEER) {
    }

    void write(XBuffer& out) const override;
};

struct NetRelayMessage_PeerPingResponse : NetRelayMessage {
    uint64_t secs = 0;
    uint32_t subsecs = 0;

    NetRelayMessage_PeerPingResponse()
            : NetRelayMessage(RELAY_MSG_PEER_PING_RESPONSE) {
    }

    void write(XBuffer& out) const override;
};

///////// Relay to Peer /////////

struct NetRelayMessage_RelayListLobbyHosts : NetRelayMessage {
    XBuffer data = XBuffer(0, false);

    NetRelayMessage_RelayListLobbyHosts()
            : NetRelayMessage(RELAY_MSG_RELAY_LIST_LOBBY_HOSTS) {
    }

    void read(XBuffer& in) override;
};

struct NetRelayMessage_RelayListLobbies : NetRelayMessage {
    XBuffer data = XBuffer(0, false);
    
    NetRelayMessage_RelayListLobbies()
            : NetRelayMessage(RELAY_MSG_RELAY_LIST_LOBBIES) {
    }

    void read(XBuffer& in) override;
};

struct NetRelayMessage_RelayPing : NetRelayMessage {
    uint64_t secs = 0;
    uint32_t subsecs = 0;

    NetRelayMessage_RelayPing()
            : NetRelayMessage(RELAY_MSG_RELAY_PING) {
    }

    void read(XBuffer& in) override;
};

struct NetRelayMessage_RelayListPeers : NetRelayMessage {
    std::vector<NETID> peers = {};

    NetRelayMessage_RelayListPeers()
            : NetRelayMessage(RELAY_MSG_RELAY_LIST_PEERS) {
    }

    void read(XBuffer& in) override;
};

struct NetRelayMessage_RelayAddPeer : NetRelayMessage {
    NETID netid = 0;

    NetRelayMessage_RelayAddPeer()
            : NetRelayMessage(RELAY_MSG_RELAY_ADD_PEER) {
    }

    void read(XBuffer& in) override;
};

struct NetRelayMessage_RelayRemovePeer : NetRelayMessage {
    NETID netid = 0;

    NetRelayMessage_RelayRemovePeer()
            : NetRelayMessage(RELAY_MSG_RELAY_ADD_PEER) {
    }

    void read(XBuffer& in) override;
};

const char* getPrimaryNetRelayAddress();

bool receiveNetRelayMessage(
        NetConnection* relay,
        NETID source,
        NetRelayMessage* result,
        NetRelayMessageType result_type_expected
);

/**
 * Sends a message to relay, with optional result that will be filled with response
 * 
 * @param relay the coneection of relay
 * @param msg the message to send 
 * @param source the source NETID to set the message
 * @return 
 */
bool sendNetRelayMessage(
        NetConnection* relay,
        const NetRelayMessage* msg,
        NETID source
);

void closeNetRelay(NetConnection* relay);

#endif //PERIMETER_NETRELAY_H
