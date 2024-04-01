#ifndef PERIMETER_NETCONNECTION_H
#define PERIMETER_NETCONNECTION_H

#define PERIMETER_IP_PORT_DEFAULT 12987
#define PERIMETER_IP_HOST_DEFAULT "127.0.0.1"
const uint32_t PERIMETER_MESSAGE_MAX_SIZE = 32 * 1024 * 1024;
const uint32_t PERIMETER_MESSAGE_COMPRESSION_SIZE = 128 * 1024;
///Specifies this message contains compressed payload
const uint16_t PERIMETER_MESSAGE_FLAG_COMPRESSED = 1 << 0;
///How many milliseconds extra to wait for the data part once getting header
const int32_t RECV_DATA_AFTER_HEADER_TIMEOUT = 10000;
///How many milliseconds to wait for handshake to be sent/recv
const int32_t CONNECTION_HANDSHAKE_TIMEOUT = 10000;
///How many milliseconds to wait for relay connection
const int32_t CONNECTION_RELAY_TIMEOUT = 10000;
///How many milliseconds to wait for active connection
const int32_t CONNECTION_ACTIVE_TIMEOUT = 10000;

#include <SDL_net.h>

//Used to identify player connection
typedef uint64_t NETID;

//Used to identify a room in server list
typedef uint64_t NetRoomID;

//Special IDs
const NETID NETID_NONE = 0;
const NETID NETID_HOST = 1;
const NETID NETID_RELAY = 2;
const NETID NETID_ALL = 3;
const NETID NETID_CLIENTS_START = 0x100;

/**
 * NetConnectionMessage, holds message content and connection sending and intended destination
 */
struct NetConnectionMessage : public XBuffer {
public:
    NETID source;
    NETID destination;

    explicit NetConnectionMessage(size_t _size, NETID _source, NETID _destination)
    : XBuffer(_size), source(_source), destination(_destination) {
    }
};

/**
 * Contains remote address data for connection
 */
class NetAddress {
private:
#ifndef EMSCRIPTEN
    IPaddress addr;
#else
    std::string address;
#endif
    
public:
    static bool resolve(NetAddress& address, const std::string& host, uint16_t default_port = 0);

    NetAddress();
    ~NetAddress();

    NetAddress& operator=(const NetAddress& other);
    bool operator==(const NetAddress& other) const;

#ifndef EMSCRIPTEN
    NetAddress(uint32_t host, uint16_t port);
    TCPsocket openTCP() const;
#endif

    void reset();
    uint16_t port() const;

    std::string getString() const;
};

/**
 * Generic transport
 */
class NetTransport {
protected:
    virtual int32_t send_raw(const uint8_t* buffer, uint32_t len, int32_t timeout) = 0;
    virtual int32_t receive_raw(uint8_t* buffer, uint32_t len, int32_t timeout) = 0;
    
public:
    static const int32_t NT_STATUS_NO_DATA = -0x1000001;
    static const int32_t NT_STATUS_TIMEOUT = -0x1000002;
    static const int32_t NT_STATUS_CLOSED  = -0x1000003;
    static const int32_t NT_STATUS_ERROR   = -0x1000004;

    static NetTransport* create(const NetAddress&);

    NetTransport() = default;
    virtual ~NetTransport() {
        close();
    };
    
    /** Close the transport, must be called on subclasses's destructors */
    virtual void close() {};
    
    /** @return true if transport is closed */
    virtual bool is_closed() const { return true; }

    /**
     * Sends data using internal send_raw
     * Closes connection upon error
     * 
     * @param buffer pointer of data to send over wire
     * @param len amount of data to read from pointer
     * @param timeout 0 to wait indefinitelly, amount of ms allow before giving up sending data
     * @return amount of bytes sent, 0 if none, <0 if error or closed
     */
    int32_t send(const void* buffer, uint32_t len, int32_t timeout);

    /**
     * Receives data from internal receive_raw
     * Closes connection upon error
     * 
     * @param buffer pointer of data to write the received data
     * @param maxlen min amount of data expected to read into buffer or 0 to get what's available
     * @param maxlen max amount of data expected to read into buffer or buffer max length
     * @param timeout 0 for no wait, amount of ms to wait until data is available
     * @return amount of bytes received, 0 if none, <0 if error or closed
     */
    int32_t receive(void* buffer, uint32_t minlen, uint32_t maxlen, int32_t timeout);
};

/**
 * Encapsulates TCP transport
 */
class NetTransportTCP: public NetTransport{
private:
    SDLNet_SocketSet socket_set = nullptr;
    TCPsocket socket = nullptr;
    
protected:
    int32_t send_raw(const uint8_t* buffer, uint32_t len, int32_t timeout) override;
    int32_t receive_raw(uint8_t* buffer, uint32_t len, int32_t timeout) override;
    
public:
    explicit NetTransportTCP(TCPsocket socket);
    ~NetTransportTCP() override {
        close();
    };

    void close() override;

    bool is_closed() const override {
        return socket == nullptr;
    }

    TCPsocket getSocket();
};

/**
 * Encapsulates WebSocket transport
 */
class NetTransportWS: public NetTransport{
private:
    int32_t handle = -1;

protected:
    int32_t send_raw(const uint8_t* buffer, uint32_t len, int32_t timeout) override;
    int32_t receive_raw(uint8_t* buffer, uint32_t len, int32_t timeout) override;

public:
    explicit NetTransportWS(int32_t handle);
    ~NetTransportWS() override { 
        close();
    };

    void close() override;
    bool is_closed() const override;
};

/**
 * Connection states
 */
enum NetConnectionState {
    NC_STATE_HAS_TRANSPORT, //Connection transport is active
    NC_STATE_HAS_CLIENT, //Connection has transport and is associated with a PNet Client
    
    //Socket deleted
    NC_STATE_ERROR, //Error occurred
    NC_STATE_ERROR_PENDING, //Error occurred and pending for calling DeleteClient
    NC_STATE_CLOSE_PENDING, //Closed and pending for calling DeleteClient
    NC_STATE_CLOSED //Ready to be reused by another socket
};

/**
 * Encapsulates a game connection
 */
class NetConnection {
private:
    friend class NetConnectionHandler;
    NetTransport* transport = nullptr;
    NETID netid = NETID_NONE;
    uint64_t time_contact = 0;
    NetConnectionState state = NC_STATE_CLOSED;
    bool is_relay = false;
    
public:
    explicit NetConnection(NetTransport* transport, NETID netid);
    ~NetConnection();

    /** @return true if connection is closed */
    FORCEINLINE bool isClosed() const {
        return state == NC_STATE_CLOSED;
    }
    
    /** @return true if socket is closed */ 
    FORCEINLINE bool hasTransport() const {
        return transport != nullptr && !transport->is_closed();
    }
    
    FORCEINLINE NETID getNETID() const {
        return netid;
    }
    
    FORCEINLINE bool isRelay() const {
        return is_relay;
    }
    
    /**
     * Sets the transport for this connection
     * If one is already set the transport is closed before setting new transport
     */
    void set_transport(NetTransport* transport, NETID netid);
    
    /** Closes the connection with error state */
    void close_error() {
        close(true);
    }

    /**
     * Closes the connection
     * @param error sets state to error
     */
    void close(bool error = false);

    /**
     * Writes data to connection
     * Closes connection upon error
     * 
     * @param buffer data to send into connection
     * @param source source NETID that has sent this data
     * @param destination destination NETID that will receive this data, can be NETID_NONE to assign connection's NETID
     * @return amount of bytes sent, <0 if error or closed
     */
    int32_t send(const XBuffer* data, NETID source, NETID destination, int32_t timeout);

    /**
     * Receives data from connection if any
     * Closes connection upon error
     * If no bytes were received the buffer is not allocated
     * 
     * @param packet_ptr pointer for input packet ptr to store the incoming data
     * @param timeout 0 for no wait, amount of ms to wait until data is available
     * @param expectedlen expected data length, if data doesn't match then is discarded, 0< for no restriction
     * @return amount of bytes received, 0 if none, <0 if error or closed
     */
    int32_t receive(NetConnectionMessage** packet_ptr, int32_t timeout);
};

class PNetCenter;

struct NetRelayPeerInfo {
    ///Has a client assigned in NetCenter?
    bool has_client = false;
    ///If host: this peer has been rejected already and is pending removal by relay?
    ///If client: mark for removal
    bool rejected = false;
    ///If peer has been already terminated
    bool terminated = false;
    ///The time this peer was first seen
    uint64_t time_contact = 0;
    ///The relay this peer belongs to
    NETID relay_netid = NETID_NONE;
};

/**
 * Bundles several connections and also handles incoming connections in accept socket
 */
class NetConnectionHandler {
private:
    size_t max_connections = 0;
    PNetCenter* net_center = nullptr;
    NetTransportTCP* accept_transport = nullptr;
    std::unordered_map<NETID, NetConnection*> connections;
    std::unordered_map<NETID, NetRelayPeerInfo> relayPeers;
    bool has_relay_connection = false;

    void stopListening();
    void stopRelay();
    void stopConnections();
    void handleRelayDisconnected();
    
    ///Creates a new NetConnection instance from transport and assign's to netid
    ///If connection couldn't be allocated the connection->getNETID() will return NETID_NONE
    ///and must be free'd
    NetConnection* newConnectionFromTransport(NetTransport* transport, NETID netid);
    
    /// Reads messages from connection until empty or max_packets reached
    void readConnectionMessages(NetConnection* connection, size_t max_packets);

    ///Creates a new room in relay for allowing clients to interact with game host
    bool startRelayRoom();
    
    ///Called to process a relay message
    void receivedRelayMessage(NetConnection* connection, NetConnectionMessage* msg);

public:    
    explicit NetConnectionHandler(PNetCenter* center);
    ~NetConnectionHandler();

    /** Resets all state and closes everything */
    void reset();
    
    //Connection stuff
    
    /** Polls the connections */
    void pollConnections();

    /** 
     * Sends buffer data to connection by NETID
     * @return amount of data sent in total, this can be several times if is NETID_ALL
     */
    size_t sendToNETID(const XBuffer* buffer, NETID source, NETID destination);

    /** 
     * Changes the NETID of connection
     */
    bool reassignConnectionNETID(NetConnection* connection, NETID netid);
    
    /** Obtain a connection from NETID */
    NetConnection* getConnection(NETID netid) const;

    /** Get list of connections */
    const std::unordered_map<NETID, NetConnection*>& getConnections() const { return connections; }

    /**
     * Returns true if connection is no longer active for relay peer or direct connection
     */
    bool isConnectionClosed(NETID i);

    /**
     * @param not_closed don't take into account if netid client exists but is closed
     * @return true if NETID has a client
     */
    bool hasClient(NETID netid, bool not_closed);

    /**
     * Terminates any connection with provided NETID
     * If peer is behind relay the relay is notified
     */
    void terminateNETID(NETID netid);
    
    /**
     * @return if there is at least one relay connection 
     */
    bool hasRelayConnection() const {
        return has_relay_connection;
    };

    //Host related functions
    
    ///Game opens a listening port directly on the system that is running on and also a public room in relay
    bool startHost(uint16_t listen_port, bool start_public_room);

    /** 
     * Checks if there is a new connection to accept from listening port or relay
     * @return new connection id that was accepted or NETID_NONE
     */
    void acceptConnection();

    ///Sets connection has having client
    void setHasClient(NETID netid, bool state);
    
    //Client related functions
    
    /** Connects to a game acting as host directly */
    NetConnection* startDirectConnection(const NetAddress& address);
    
    /**
     * Connects to a game using relay server as intermediary
     * the relay address and room for the desired game from server list must be provided
     */
    NetConnection* startRelayRoomConnection(const NetAddress& address, NetRoomID room_id);
};

#endif //PERIMETER_NETCONNECTION_H
