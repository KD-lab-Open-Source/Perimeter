#include "NetIncludes.h"
#include "NetConnection.h"
#include "crc.h"
#include "P2P_interface.h"
#include "NetConnectionAux.h"

///////// NetAddress //////////////

NetAddress::NetAddress(uint32_t host, uint16_t port): addr() {
    addr.host = host;
    setPort(port);
}

NetAddress::NetAddress(): NetAddress(INADDR_NONE, 0) {}

NetAddress::~NetAddress() = default;

uint32_t NetAddress::crc() const {
    uint32_t crc = startCRC32;
    crc=crc32((unsigned char*)&addr, sizeof(addr), crc);
    return crc;
}

uint16_t NetAddress::port() const {
    return SDLNet_Read16(&addr.port);
}

void NetAddress::setPort(uint16_t port) {
    SDLNet_Write16(port, &addr.port);
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
    for (size_t i = 0; i < 4; ++i) {
        if (i > 0) text += ".";
        uint8_t v = (addr.host >> (8 * i)) & 0xff;
        text += std::to_string(v);
    }
    return text;
}

bool NetAddress::resolve(NetAddress& address, const std::string& host, uint16_t default_port) {
    std::string ip;
    uint16_t port;
    if (default_port == 0) {
        default_port = PERIMETER_IP_PORT_DEFAULT;
    }
    size_t pos = host.find(':');
    if (pos == std::string::npos) {
        ip = host;
        port = default_port;
    } else {
        ip = host.substr(0, pos);
        std::string port_str = host.substr(pos + 1);
        char* end;
        port = static_cast<uint16_t>(strtol(port_str.c_str(), &end, 10));
        if (!port) port = default_port;
    }
    int32_t ret = SDLNet_ResolveHost(&address.addr, ip.c_str(), port) == 0;
    if (ret < 0 || address.addr.host == INADDR_NONE) {
        fprintf(stderr, "Error resolving host %s: %s\n", host.c_str(), SDLNet_GetError());
        return false;
    }
    return true;
}

TCPsocket NetAddress::openTCP() const {
    TCPsocket socket = SDLNet_TCP_Open(const_cast<IPaddress*>(&addr));
    if (socket == nullptr) {
        fprintf(stderr, "NetAddress::openTCP failed address %s error %s\n", getString().c_str(), SDLNet_GetError());
    }
    return socket;
}

///////// NetTransport //////////////

int32_t NetTransport::send(const void* buffer, uint32_t len, int32_t timeout) {
    if (is_closed()) {
        return NT_STATUS_CLOSED;
    }
    if (buffer == nullptr) {
        ErrH.Abort("NetTransport::send got null buffer");
    }
    
    int32_t sent = 0;
    int32_t start_time = clocki();
    while (sent < len) {
        if (0 < timeout && start_time + timeout < clocki()) {
            return NT_STATUS_TIMEOUT;
        }
        int32_t amount = send_raw(static_cast<const uint8_t*>(buffer) + sent, static_cast<int32_t>(len) - sent, timeout);
        if (amount < 0) {
            if (amount != NT_STATUS_TIMEOUT) {
                fprintf(stderr, "NetTransport::send data failed result %d sent %d len %d\n", amount, sent, len);
            }
            return amount;
        }
        sent += amount;
    }

    return sent;
}

int32_t NetTransport::receive(void* buffer, uint32_t minlen, uint32_t maxlen, int32_t timeout) {
    if (is_closed()) {
        return NT_STATUS_CLOSED;
    }
    if (buffer == nullptr) {
        ErrH.Abort("NetTransport::receive got null buffer");
    }

    int32_t received = 0;
    int32_t start_time = clocki();
    bool has_timeout = 0 < timeout;
    while (received < maxlen
    && (0 == minlen || received < minlen)) {
        if (has_timeout && start_time + timeout < clocki()) {
            return NT_STATUS_TIMEOUT;
        }
        int32_t amount = receive_raw(static_cast<uint8_t*>(buffer) + received, static_cast<int32_t>(maxlen) - received, timeout);
        if (has_timeout && amount == NT_STATUS_NO_DATA) {
            //Keep waiting
            continue;
        }
        if (amount < 0) {
            if (amount != NT_STATUS_TIMEOUT && amount != NT_STATUS_NO_DATA) {
                fprintf(stderr, "NetTransport::receive failed amount %" PRIi32
                                " minlen %" PRIu32 " maxlen %" PRIu32 "\n", amount, minlen, maxlen);
                close();
            }
            return amount;
        }
        received += amount;
    }
    if (received < minlen || maxlen < received) {
        fprintf(stderr, "NetTransport::receive length mismatch received %" PRIi32
                " minlen %" PRIu32 " maxlen %" PRIu32 "\n", received, minlen, maxlen);
        close();
        return NT_STATUS_ERROR;
    }

    return received;
}

///////// NetTransportTCP //////////////

NetTransportTCP::NetTransportTCP(TCPsocket socket_) {
    socket = socket_;
    socket_set = SDLNet_AllocSocketSet(1);
    SDLNet_TCP_AddSocket(socket_set, socket);
}

void NetTransportTCP::close() {
    if (socket_set) {
        if (socket) {
            SDLNet_TCP_DelSocket(socket_set, socket);
        }
        SDLNet_FreeSocketSet(socket_set);
        socket_set = nullptr;
    }
    if (socket) {
        SDLNet_TCP_Close(socket);
        socket = nullptr;
    }
}

int32_t NetTransportTCP::send_raw(const uint8_t* buffer, uint32_t len, int32_t _timeout) {
    //May return 0 if closed
    int32_t amount = SDLNet_TCP_Send(socket, buffer, static_cast<int32_t>(len));
    if (amount == 0) {
        return NT_STATUS_CLOSED;
    } else if (amount <= 0) {
        fprintf(stderr, "NetTransportTCP::send data failed result %" PRIi32 " len %" PRIu32 " %s\n", amount, len, SDLNet_GetError());
        return NT_STATUS_ERROR;
    }
    return amount;
}

int32_t NetTransportTCP::receive_raw(uint8_t* buffer, uint32_t len, int32_t _timeout) {
    int32_t n = SDLNet_CheckSockets(socket_set, 0);
    if (n == -1) {
        fprintf(stderr, "CheckSockets error: %s\n", SDLNet_GetError());
        // most of the time this is a system error, where perror might help you.
        perror("SDLNet_CheckSockets");
    } else if (n == 0) {
        return NT_STATUS_NO_DATA;
    }
    if (SDLNet_SocketReady(socket) == 0) {
        return NT_STATUS_NO_DATA;
    }

    //May return 0 if closed
    int32_t amount = SDLNet_TCP_Recv(socket, buffer, static_cast<int32_t>(len));
    if (amount == 0) {
        return NT_STATUS_CLOSED;
    } else if (amount <= 0) {
        fprintf(stderr, "NetTransportTCP::receive failed amount %" PRIi32 " len %" PRIu32 " %s\n", amount, len, SDLNet_GetError());
        return NT_STATUS_ERROR;
    }
    return amount;
}

///////// NetConnection //////////////

const uint64_t NC_HEADER_MAGIC = 0xDE000000000000CA;
const uint64_t NC_HEADER_MASK  = 0xFF000000000000FF;

NetConnection::NetConnection(NetTransport* _transport, NETID _netid) {
    set_transport(_transport, _netid);
}

NetConnection::~NetConnection() {
    close();
}

void NetConnection::set_transport(NetTransport* _transport, NETID _netid) {
    //Call close to remove any existing transport and reset connectionstate
    close();
    
    //Set new transport
    netid = _netid;
    transport = _transport;
    if (hasTransport()) {
        state = NC_STATE_HAS_TRANSPORT;
    }
}

void NetConnection::close(bool error) {
    switch (state) {
        case NC_STATE_HAS_TRANSPORT:
            state = error ? NC_STATE_ERROR : NC_STATE_CLOSED;
            break;
        case NC_STATE_HAS_CLIENT:
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
    if (transport) {
        transport->close();
        delete transport;
        transport = nullptr;
    }
}

int32_t NetConnection::send(const XBuffer* data, NETID source, NETID destination, int32_t timeout) {
    if (!hasTransport()) {
        return -1;
    }
    if (data == nullptr) {
        fprintf(stderr, "NetConnection::send NETID %" PRIX64 " null buffer\n", netid);
        ErrH.Abort("Got null buffer in send");
    }
    if (data->tell() == 0) {
        xassert(0);
        fprintf(stderr, "NetConnection::send NETID %" PRIX64 " data to sent is empty!\n", netid);
        return -2;
    }
    if (destination == NETID_NONE) {
        destination = this->netid;
    }
    xassert(destination != NETID_NONE);
    //Pinky promise that sending_buffer won't modify the data ptr
    XBuffer sending_buffer(const_cast<char*>(data->address()), data->tell());
    sending_buffer.set(data->tell());
    uint16_t flags = 0;
    
    //Compression, first thing to do to calculate actual length
    if (sending_buffer.tell() > PERIMETER_MESSAGE_COMPRESSION_SIZE) {
        XBuffer compress_buffer(sending_buffer.tell(), true);
        if (sending_buffer.compress(compress_buffer) == 0 
        && sending_buffer.tell() > compress_buffer.tell()) {
            sending_buffer = compress_buffer;
            flags |= PERIMETER_MESSAGE_FLAG_COMPRESSED;
        }
    }
    
    //Header size, not accounted in length that goes inside
    uint32_t header_len = sizeof(NC_HEADER_MAGIC);
    
    //Body size, the length of message +
    //Source + destination NETID info 
    uint32_t body_len = sending_buffer.tell() + sizeof(NETID) * 2;

    //Calculate message size
    int32_t msg_size = static_cast<int32_t>(body_len + header_len);
    if (msg_size > PERIMETER_MESSAGE_MAX_SIZE) {
        xassert(0);
        fprintf(stderr, "NetConnection::send NETID %" PRIX64 " data too big len %d\n", netid, msg_size);
        return -2;
    }

    //Assemble header and data
    uint64_t header = NC_HEADER_MAGIC;
    header |= (static_cast<uint64_t>(flags & 0xFFFF) << 8);
    header |= (static_cast<uint64_t>(body_len & 0xFFFFFFFF) << 24);
    XBuffer xbuf(msg_size);
    xbuf < SDL_SwapBE64(header);
    xbuf < SDL_SwapBE64(source);
    xbuf < SDL_SwapBE64(destination);
    xbuf.write(sending_buffer, sending_buffer.tell());

#ifdef PERIMETER_DEBUG
    if (xbuf.tell() != msg_size) {
        fprintf(stderr, "NetConnection::send NETID %" PRIX64 " written buffer mismatch buf %" PRIsize " msg %" PRIi32 " len %" PRIsize " %s\n",
                netid, xbuf.tell(), msg_size, sending_buffer.tell(), SDLNet_GetError());
        close_error();
        return -4;
    }
#endif
    int32_t sent = transport->send(xbuf.buf, xbuf.tell(), timeout);

    if (sent != msg_size) {
        fprintf(stderr, "NetConnection::send NETID %" PRIX64 " length mismatch sent %" PRIi32 " msg %" PRIi32 " len %" PRIsize " %s\n",
                netid, sent, msg_size, sending_buffer.tell(), SDLNet_GetError());
        close_error();
        return -4;
    }
    
    return sent;
}

int32_t NetConnection::receive(NetConnectionMessage** packet_ptr, int32_t timeout) {
    if (!hasTransport()) {
        return -1;
    }

    //Get header first
    uint64_t header;
    int32_t amount = transport->receive(&header, 0, sizeof(header), timeout);
    if (amount == NetTransport::NT_STATUS_NO_DATA) {
        return 0;
    }
    if (amount < 0) {
        close_error();
        return amount;
    }
    if (amount != sizeof(header)) {
        fprintf(stderr, "NetConnection::receive NETID %" PRIX64 " header failed amount %d %s\n", netid, amount, SDLNet_GetError());
        return -2;
    }
    header = SDL_SwapBE64(header);
    
    //Check magic
    if ((header & NC_HEADER_MASK) != NC_HEADER_MAGIC) {
        fprintf(stderr, "NetConnection::receive NETID %" PRIX64 " header failed magic mismatch 0x%" PRIX64 " %s\n", netid, header, SDLNet_GetError());
        return -2;
    }

    //Extract header stuff
    uint16_t flags = (header >> 8) & 0xFFFF;
    amount = static_cast<int32_t>((header >> 24) & 0xFFFFFFFF);

    //Ensure is not too big
    if (amount >= PERIMETER_MESSAGE_MAX_SIZE) {
        xassert(0);
        fprintf(stderr, "NetConnection::receive NETID %" PRIX64 " header failed too long 0x%" PRIX64 " len %" PRIu32 "\n", netid, header, amount);
        return -2;
    }
    
    //Read data until all is received
    NetConnectionMessage* packet = *packet_ptr;
    if (packet == nullptr) {
        //Create new packet
        packet = new NetConnectionMessage(amount, this->netid, NETID_NONE);
    } else {
        //(re)allocate it to fit our data
        packet->alloc(amount);
    }
    int32_t received = transport->receive(
            packet->address() + packet->tell(), amount, amount, 
            std::max(timeout, 0) + RECV_DATA_AFTER_HEADER_TIMEOUT
    );
    if (received <= 0) {
        fprintf(stderr, "NetConnection::receive NETID %" PRIX64 " data chunk failed amount %d received %d %s\n", netid, amount, received, SDLNet_GetError());
        amount = -5;
    } else if (amount != received) {
        fprintf(stderr, "NetConnection::receive NETID %" PRIX64 " data failed amount %d received %d %s\n", netid, amount, received, SDLNet_GetError());
        amount = -6;
    }

    //Extract source and destination netids that is prepended before actual message data
    //If amount goes negative or 0 is fine because it would mark this packet as invalid 
    amount -= sizeof(NETID) * 2;
    if (amount < 0) {
        xassert(0);
        fprintf(stderr, "NetConnection::receive NETID %" PRIX64 " header without message 0x%" PRIX64 " len %" PRIu32 "\n", netid, header, amount);
        amount = -7;
    } else if (amount == 0) {
        fprintf(stderr, "NetConnection::receive NETID %" PRIX64 " message is empty 0x%" PRIX64 "\n", netid, header);
    } else {
        *packet > packet->source;
        *packet > packet->destination;
        packet->source = SDL_SwapBE64(packet->source);
        packet->destination = SDL_SwapBE64(packet->destination);
    }

    //Decompression
    if (0 < amount && flags & PERIMETER_MESSAGE_FLAG_COMPRESSED) {
        XBuffer output(amount, true);
        int32_t ret = packet->uncompress(output);
        if (ret != 0) {
            amount = -9;
        } else {
            amount = static_cast<int32_t>(output.tell());
            std::swap(*static_cast<XBuffer*>(packet), output);
        }
    } else {
        //Move message content that is after source/destination etc to start
        memmove(packet->address(), packet->address() + packet->tell(), amount);
        packet->set(amount);
    }
    
    //Set packet ptr or delete if we created the packet in this function
    if (*packet_ptr == nullptr) {
        if (0 < amount) {
            *packet_ptr = packet;
        } else {
            delete packet;
            close_error();
        }
    }
    return amount;
}
