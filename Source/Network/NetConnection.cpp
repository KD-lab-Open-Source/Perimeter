#include "NetIncludes.h"
#include "NetConnection.h"
#include "crc.h"
#include "P2P_interface.h"
#include "NetConnectionAux.h"

///////// NetAddress //////////////

NetAddress::NetAddress(): addr() {
    addr.host = INADDR_NONE;
    addr.port = 0;
}

NetAddress::~NetAddress() = default;

uint32_t NetAddress::crc() const {
    uint32_t crc = startCRC32;
    crc=crc32((unsigned char*)&addr, sizeof(addr), crc);
    return crc;
}

uint16_t NetAddress::port() const {
#if SDL_DATA_ALIGNED
    //In SDL2_net When SDL_DATA_ALIGNED is set it uses SDLNet_Read16 without const, probably a mistake
    return SDLNet_Read16(const_cast<Uint16*>(&addr.port));
#else
    return SDLNet_Read16(&addr.port);
#endif
}

std::string NetAddress::getString() const {
    std::string text;

    if (addr.host != INADDR_NONE) {
        text = getStringIP();
        if (addr.port) {
            text += ":" + std::to_string(port());
        }
    } else {
        text = "none";
    }

    return text;
}

std::string NetAddress::getStringIP() const {
    std::string text;
    for (int i = 0; i < 4; ++i) {
        if (i > 0) text += ".";
        uint8_t v = (addr.host >> (8 * i)) & 0xff;
        text += std::to_string(v);
    }
    return text;
}

bool NetAddress::resolve(NetAddress& address, const std::string& host) {
    std::string ip;
    uint16_t port;
    size_t pos = host.find(':');
    if (pos == std::string::npos) {
        ip = host;
        port = PERIMETER_IP_PORT_DEFAULT;
    } else {
        ip = host.substr(0, pos);
        std::string port_str = host.substr(pos + 1);
        char* end;
        port = static_cast<uint16_t>(strtol(port_str.c_str(), &end, 10));
        if (!port) port = PERIMETER_IP_PORT_DEFAULT;
    }
    int ret = SDLNet_ResolveHost(&address.addr, ip.c_str(), port) == 0;
    if (ret < 0 || address.addr.host == INADDR_NONE) {
        fprintf(stderr, "Error resolving host %s: %s\n", host.c_str(), SDLNet_GetError());
        return false;
    }
    return true;
}

///////// NetConnection //////////////

const uint64_t NC_HEADER_MAGIC = 0xDE000000000000CA;
const uint64_t NC_HEADER_MASK  = 0xFF000000000000FF;

NetConnection::NetConnection(TCPsocket socket) {
    set_socket(socket);
}

NetConnection::~NetConnection() {
    close();
}

void NetConnection::set_socket(TCPsocket socket_) {
    close();
    state = NC_STATE_SETUP;
    socket_set = SDLNet_AllocSocketSet(1);
    socket = socket_;
    SDLNet_TCP_AddSocket(socket_set, socket);
}

void NetConnection::close(bool error) {
    switch (state) {
        case NC_STATE_SETUP:
            state = error ? NC_STATE_ERROR : NC_STATE_CLOSED;
            break;
        case NC_STATE_ACTIVE:
            state = error ? NC_STATE_ERROR_PENDING : NC_STATE_CLOSE_PENDING;
            break;
        case NC_STATE_ERROR:
            if (!error) {
                state = NC_STATE_CLOSED;
            }
            break;
        case NC_STATE_ERROR_PENDING:
        case NC_STATE_CLOSE_PENDING:
        case NC_STATE_CLOSED:
            break;
    }
    if (socket) {
        //printf("TCP close %lu\n", id);
        SDLNet_TCP_DelSocket(socket_set, socket);
        SDLNet_TCP_Close(socket);
        socket = nullptr;
    }
    if (socket_set) {
        SDLNet_FreeSocketSet(socket_set);
        socket_set = nullptr;
    }
}

int NetConnection::send(const XBuffer& data) {
    return send(data.buf, data.length());
}

int NetConnection::send(const void* buffer, uint32_t len) {
    if (!has_socket()) {
        return -1;
    }
    uint16_t flags = 0;
    XBuffer sending_buffer(const_cast<void*>(buffer), len);
    sending_buffer.set(len);
    
    //Compression
    if (len > PERIMETER_MESSAGE_COMPRESSION_SIZE) {
        XBuffer compress_buffer(len, true);
        if (sending_buffer.compress(compress_buffer) == 0)  {
            sending_buffer = std::move(compress_buffer);
            len = sending_buffer.tell();
            flags |= PERIMETER_MESSAGE_FLAG_COMPRESSED;
        }
    }

    //Check len
    if (len > PERIMETER_MESSAGE_MAX_SIZE) {
        xassert(0);
        fprintf(stderr, "TCP send data too big len %d\n", len);
        return -2;
    }
    
    //Assemble header and data
    int msg_size = static_cast<int>(len + sizeof(NC_HEADER_MAGIC));
    XBuffer xbuf(msg_size);
    xbuf < NC_HEADER_MAGIC;
    xbuf.set(1);
    xbuf < flags;
    xbuf.set(3);
    xbuf < len;
    xbuf.set(8);
    xbuf.write(sending_buffer, len);
    
    //Send buffer
    int amount = SDLNet_TCP_Send(socket, xbuf.buf, msg_size);
    if (amount != msg_size) {
        fprintf(stderr, "TCP send data failed amount %d msg %d len %d %s\n", amount, msg_size, len, SDLNet_GetError());
        close_error();
        return -3;
    }

    return static_cast<int>(len);
}

int NetConnection::receive_raw(void* buffer, uint32_t maxlen, int timeout) {
    //Internal receive    
    if (!has_socket()) {
        return -1;
    }
    xassert(buffer != nullptr);
    
    if (0 <= timeout) {
        int n = SDLNet_CheckSockets(socket_set, timeout);
        if (n == -1) {
            fprintf(stderr, "CheckSockets error: %s\n", SDLNet_GetError());
            // most of the time this is a system error, where perror might help you.
            perror("SDLNet_CheckSockets");
        } else if (n == 0) {
            return 0;
        }

        /*
        if (!SDLNet_SocketReady(socket)) {
            return 0;
        }
        */
    }

    int amount = SDLNet_TCP_Recv(socket, buffer, static_cast<int>(maxlen));
    if (amount <= 0) {
        fprintf(stderr, "TCP recv failed amount %d maxlen %d %s\n", amount, maxlen, SDLNet_GetError());
        close_error();
        return -2;
    }
    return amount;
}

int NetConnection::receive(XBuffer& buffer, int timeout) {
    if (!has_socket()) {
        return -1;
    }

    //Get header first
    uint64_t header;
    int amount = receive_raw(&header, sizeof(header), timeout);
    if (amount <= 0) {
        return amount;
    }
    if (amount != sizeof(header)) {
        fprintf(stderr, "TCP recv header failed amount %d %s\n", amount, SDLNet_GetError());
        return -2;
    }
    
    //Check magic
    if ((header & NC_HEADER_MASK) != NC_HEADER_MAGIC) {
        fprintf(stderr, "TCP recv header failed magic mismatch %lx %s\n", header, SDLNet_GetError());
        return -2;
    }

    //Extract header stuff
    uint16_t flags = (header >> 8) & 0xFFFF;
    uint32_t data_size = (header >> 24) & 0xFFFFFFFF;

    //Ensure is not too big
    xassert(data_size < PERIMETER_MESSAGE_MAX_SIZE);
    data_size = std::min(data_size, PERIMETER_MESSAGE_MAX_SIZE);
    
    //(re)allocate it to fit our data
    buffer.alloc(data_size);
    
    //Read data until all is received
    amount = 0;
    while (amount < data_size) {
        uint32_t left = data_size - amount;
        int received = receive_raw(buffer.buf + buffer.tell(), left, std::max(timeout, 0) + RECV_DATA_AFTER_HEADER_TIMEOUT);
        if (received == 0) {
            fprintf(stderr, "TCP recv data chunk failed left %d amount %d size %d %s\n", left, amount, data_size, SDLNet_GetError());
            amount = -5;
            break;
        }
        buffer.set(received, XB_CUR);
        amount += received;
    }
    
    if (amount != data_size) {
        fprintf(stderr, "TCP recv data failed amount %d size %d %s\n", amount, data_size, SDLNet_GetError());
        if (amount > 0) {
            amount = -6;
        }
    }

    //Decompression
    if (0 < amount && flags & PERIMETER_MESSAGE_FLAG_COMPRESSED) {
        buffer.set(0);
        XBuffer output(data_size, true);
        int ret = buffer.uncompress(output);
        if (ret != 0) {
            return -7;
        }
        amount = static_cast<int>(output.tell());
        std::swap(buffer, output);
    }

    buffer.set(amount);
    return amount;
}

///////// NetConnectionHandler //////////////

NetConnectionHandler::NetConnectionHandler(PNetCenter* center): net_center(center) {
    stopConnections();
}

NetConnectionHandler::~NetConnectionHandler() {
    reset();
    net_center = nullptr;
}

void NetConnectionHandler::reset() {
    stopListening();
    stopConnections();
}

NETID NetConnectionHandler::acceptConnection() {
    NETID netid = NETID_NONE;
    if (accept_socket) {
        TCPsocket incoming_socket = SDLNet_TCP_Accept(accept_socket);
        if(!incoming_socket) {
            SDLNet_SetError(nullptr);
        } else {
            NetConnection* incoming = nullptr;
            
            //Find any closed connection in array
            bool reused = false;
            for (auto& entry : connections) {
                if (entry.second->is_closed()) {
                    incoming = entry.second; 
                    incoming->set_socket(incoming_socket);
                    reused = true;
                    break;
                }
            }

            //Check if we can add it
            if (!reused) {
                incoming = newConnectionFromSocket(incoming_socket, false);
            }

            //incoming may be deallocated if index is not available, so get it before
            netid = incoming->netid;
            
            net_center->handleIncomingClientConnection(incoming);
        }
    }

    return netid;
}

void NetConnectionHandler::pollConnections() {
    for (auto& entry : connections) {
        NetConnection* connection = entry.second;
        switch (connection->state) {
            case NC_STATE_ACTIVE: {
                size_t total_recv = 0;
                while (total_recv < PERIMETER_MESSAGE_MAX_SIZE * 10) {
                    InputPacket* packet = new InputPacket(connection->netid);
                    int len = connection->receive(*packet);
                    if (0 < len) {
                        net_center->m_InputPacketList.push_back(packet);
                        total_recv += len;
                    } else {
                        delete packet;
                        break;
                    }
                }
                break;
            }
            case NC_STATE_ERROR_PENDING:
            case NC_STATE_CLOSE_PENDING: {
                net_center->DeleteClient(connection->netid, false);
                //Mark it as closed, since we processed the client
                connection->state = NC_STATE_CLOSED;
                break;
            }
            default:
                break;
        }
    }
}

void NetConnectionHandler::stopConnections() {
    for (auto& entry : connections) {
        NetConnection* conn = entry.second;
        conn->close();
        delete conn;
    }
    connections.clear();
}

const NetConnectionHandler::NetConnectionMap& NetConnectionHandler::getConnections() const {
    return connections;
}

NetConnection* NetConnectionHandler::getConnection(NETID netid) const {
    NetConnection* conn = nullptr;
    if (connections.count(netid)) {
        NetConnection* candidate = connections.at(netid);
        if (!candidate->is_closed()) {
            conn = candidate;
        }
    }
    return conn;
}

bool NetConnectionHandler::startListening(uint16_t port) {
    stopListening();
    stopConnections();
    
    if (gb_RenderDevice->GetRenderSelection() == DEVICE_HEADLESS) {
        max_connections = NETWORK_PLAYERS_MAX;
    } else {
        //Remove one since host is player too
        max_connections = NETWORK_PLAYERS_MAX - 1;
    }

    IPaddress addr;
    addr.host = INADDR_ANY;
    SDLNet_Write16(port, &addr.port);

    accept_socket = SDLNet_TCP_Open(&addr);
    if (accept_socket == nullptr) {
        fprintf(stderr, "TCP listen failed on port %d error %s\n", port, SDLNet_GetError());
        return false;
    } else {
        LogMsg("TCP listening on port %d\n", port);
    }
    return true;
}

void NetConnectionHandler::stopListening() {
    if (accept_socket) {
        LogMsg("TCP listen socket closed\n");
        SDLNet_TCP_Close(accept_socket);
        accept_socket = nullptr;
    }
}

NetConnection* NetConnectionHandler::startConnection(NetAddress* address) {
    stopListening();
    stopConnections();
    
    max_connections = 1;

    TCPsocket socket = SDLNet_TCP_Open(&address->addr);
	if (!socket) {
        fprintf(stderr, "TCP socket open failed address %s error %s\n", address->getString().c_str(), SDLNet_GetError());
        return nullptr;
	}
    
    NetConnection* connection = newConnectionFromSocket(socket, true);
    if (connection->netid != NETID_NONE) {
        return connection;
    } else {
        fprintf(stderr, "Error allocating new connection\n");
        connection->close();
        delete connection;
        return nullptr;
    }
}

NetConnection* NetConnectionHandler::newConnectionFromSocket(TCPsocket socket, bool host) {
    NetConnection* connection = new NetConnection(socket);
    if (connections.size() < max_connections) {
        NETID netid;
        if (host) {
            netid = NETID_HOST;
        } else {
            netid = NETID_HOST + connections.size() + 1;
        }
        connection->netid = netid;
        connections.insert_or_assign(netid, connection);
    }
    return connection;
}
