#ifndef PERIMETER_NETCONNECTION_H
#define PERIMETER_NETCONNECTION_H

#define PERIMETER_IP_PORT_DEFAULT 12987
#define PERIMETER_IP_HOST_DEFAULT "127.0.0.1"
const uint32_t PERIMETER_MESSAGE_MAX_SIZE = 32 * 1024 * 1024;
const uint32_t PERIMETER_MESSAGE_COMPRESSION_SIZE = 128*1024;
const uint16_t PERIMETER_MESSAGE_FLAG_COMPRESSED = 1 << 0;
///How many milliseconds extra to wait for the data part once getting header
const int RECV_DATA_AFTER_HEADER_TIMEOUT = 10000;
///How many milliseconds to wait for handshake to be sent/recv
const int CONNECTION_HANDSHAKE_TIMEOUT = 10000;

#include <SDL_net.h>

//Special IDs
#define NETID_NONE 0
#define NETID_HOST 1
#define NETID_ALL 0xFF

//Used to identify player connection
typedef uint64_t NETID;

/**
 * Contains remote address data for connection
 */
class NetAddress {
private:
public:
    IPaddress addr;

    NetAddress& operator=(const NetAddress& other) {
        this->addr.host = other.addr.host;
        this->addr.port = other.addr.port;
        return *this;
    }

    NetAddress();
    ~NetAddress();

    static bool resolve(NetAddress& address, const std::string& host);

    uint32_t crc() const;
    uint16_t port() const;
    std::string getString() const;
    std::string getStringIP() const;
};

/**
 * Connection states
 */
enum NetConnectionState {
    NC_STATE_SETUP,
    NC_STATE_ACTIVE,
    
    //Socket deleted
    NC_STATE_ERROR, //Error occurred
    NC_STATE_ERROR_PENDING, //Error occurred and pending for calling DeleteClient
    NC_STATE_CLOSE_PENDING, //Closed and pending for calling DeleteClient
    NC_STATE_CLOSED //Ready to be reused by another socket
};

/**
 * Encapsulates a socket
 */
class NetConnection {
private:
    SDLNet_SocketSet socket_set = nullptr;
    TCPsocket socket = nullptr;

    /**
     * Receives TCP data from connection if any
     * Closes connection upon error
     * 
     * @param buffer pointer of data to write the received data
     * @param maxlen max amount of data expected to read 
     * @param timeout 0 for no wait, amount of ms to wait until data is available
     * @return amount of bytes received, 0 if none, <0 if error or closed
     */
    int receive_raw(void* buffer, uint32_t maxlen, int timeout);
    
public:
    NetConnectionState state = NC_STATE_SETUP;
    NETID netid = NETID_NONE;
    
    explicit NetConnection(TCPsocket socket);
    ~NetConnection();

    /** @return true if connection has errored */
    FORCEINLINE bool is_error() const {
        return state == NC_STATE_ERROR || state == NC_STATE_ERROR_PENDING;
    }

    /** @return true if connection is active */
    FORCEINLINE bool is_active() const {
        return state == NC_STATE_ACTIVE;
    }

    /** @return true if connection is closed */
    FORCEINLINE bool is_closed() const {
        return state == NC_STATE_CLOSED;
    }
    
    /** @return true if socket is closed */ 
    FORCEINLINE bool has_socket() const {
        return socket != nullptr;
    }
    
    /**
     * Sets the socket for this connection
     * If one is already set the connection is closed before setting new socket
     */
    void set_socket(TCPsocket socket);
    
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
     * @return amount of bytes sent, <0 if error or closed
     */
    int send(const XBuffer& data);

    /**
     * Writes data to connection
     * Closes connection upon error
     * 
     * @param buffer pointer of data to send into connection
     * @param len amount of data to send 
     * @return amount of bytes sent, <0 if error or closed
     */
    int send(const void* buffer, uint32_t len);

    /**
     * Receives data from connection if any
     * Closes connection upon error
     * If no bytes were received the buffer is not allocated
     * 
     * @param buffer ref of data buffer to store the received data
     * @param timeout 0 for no wait, amount of ms to wait until data is available
     * @param expectedlen expected data length, if data doesn't match then is discarded, 0< for no restriction
     * @return amount of bytes received, 0 if none, <0 if error or closed
     */
    int receive(XBuffer& buffer, int timeout = 0);
};

class PNetCenter;

/**
 * Bundles several connections and also handles incoming connections in accept socket
 */
class NetConnectionHandler {
private:
    size_t max_connections = 0;
    PNetCenter* net_center = nullptr;
    TCPsocket accept_socket = nullptr;
    typedef std::unordered_map<NETID, NetConnection*> NetConnectionMap;
    NetConnectionMap connections;

    NetConnection* newConnectionFromSocket(TCPsocket socket, bool host);
        
public:    
    explicit NetConnectionHandler(PNetCenter* center);
    ~NetConnectionHandler();

    /** Resets all state and closes everything */
    void reset();
    
    /** 
     * Checks if there is a new connection to accept
     * @return new connection id that was accepted or NETID_NONE
     */    
    NETID acceptConnection();
    
    /** Polls the connections */
    void pollConnections();
    
    //Connection stuff
    void stopConnections();
    const NetConnectionMap& getConnections() const;
    NetConnection* getConnection(NETID netid) const;

    //Listening functions
    bool startListening(uint16_t port);
    void stopListening();
    
    //Single client connection
    NetConnection* startConnection(NetAddress* address);
};

#endif //PERIMETER_NETCONNECTION_H
