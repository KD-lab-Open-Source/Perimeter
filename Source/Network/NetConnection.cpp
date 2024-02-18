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
        //printf("TCP close 0x%" PRIX64 "\n", id);
        SDLNet_TCP_DelSocket(socket_set, socket);
        SDLNet_TCP_Close(socket);
        socket = nullptr;
    }
    if (socket_set) {
        SDLNet_FreeSocketSet(socket_set);
        socket_set = nullptr;
    }
}

int NetConnection::send_raw(const void* buffer, uint32_t len) {
    if (!has_socket()) {
        return -1;
    }
    if (buffer == nullptr) {
        ErrH.Abort("Got null buffer in send_raw");
    }

    int sent = 0;
    while (sent < len) {
        int amount = SDLNet_TCP_Send(socket, static_cast<const uint8_t*>(buffer) + sent, static_cast<int>(len) - sent);
        if (amount < 0) {
            fprintf(stderr, "TCP send data failed result %d sent %d len %d %s\n", amount, sent, len, SDLNet_GetError());
            return -3;
        }
        sent += amount;
    }

    return sent;
}

int NetConnection::send(const XBuffer& data) {
    return send(reinterpret_cast<const uint8_t*>(data.buf), data.length());
}

int NetConnection::send(const uint8_t* buffer, uint32_t len) {
    if (buffer == nullptr) {
        ErrH.Abort("Got null buffer in send");
    }
    uint16_t flags = 0;
    XBuffer sending_buffer(const_cast<uint8_t*>(buffer), len);
    sending_buffer.set(len);
    
    //Compression, first thing to do to calculate actual length
    if (len > PERIMETER_MESSAGE_COMPRESSION_SIZE) {
        XBuffer compress_buffer(len, true);
        if (sending_buffer.compress(compress_buffer) == 0)  {
            sending_buffer = compress_buffer;
            len = sending_buffer.tell();
            flags |= PERIMETER_MESSAGE_FLAG_COMPRESSED;
        }
    }

    //Calculate message size
    int msg_size = static_cast<int>(len + sizeof(NC_HEADER_MAGIC));
    if (msg_size > PERIMETER_MESSAGE_MAX_SIZE) {
        xassert(0);
        fprintf(stderr, "TCP send data too big len %d\n", msg_size);
        return -2;
    }

    //Assemble header and data
    XBuffer xbuf(msg_size);
    xbuf < NC_HEADER_MAGIC;
    xbuf.set(1);
    xbuf < flags;
    xbuf.set(3);
    xbuf < len;
    xbuf.set(8);
    xbuf.write(sending_buffer, len);
    
    int sent = send_raw(xbuf.buf, msg_size);

    if (sent != msg_size) {
        fprintf(stderr, "TCP send length mismatch sent %d msg %d len %d %s\n", sent, msg_size, len, SDLNet_GetError());
        close_error();
        return -4;
    }
    
    return sent;
}

int NetConnection::receive_raw(void* buffer, uint32_t maxlen, int timeout) {
    //Internal receive    
    if (!has_socket()) {
        return -1;
    }
    if (buffer == nullptr) {
        ErrH.Abort("Got null buffer in receive_raw");
    }
    
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

    int received = 0;
    while (received < maxlen) {
        int amount = SDLNet_TCP_Recv(socket, static_cast<uint8_t*>(buffer) + received, static_cast<int>(maxlen) - received);
        if (amount <= 0) {
            fprintf(stderr, "TCP recv failed amount %d maxlen %d %s\n", amount, maxlen, SDLNet_GetError());
            close_error();
            return -2;
        }
        received += amount;
    }
    if (received != maxlen) {
        fprintf(stderr, "TCP recv length mismatch received %d maxlen %d %s\n", received, maxlen, SDLNet_GetError());
        close_error();
        return -4;
    }
    
    return received;
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
        fprintf(stderr, "TCP recv header failed magic mismatch 0x%" PRIX64 " %s\n", header, SDLNet_GetError());
        return -2;
    }

    //Extract header stuff
    uint16_t flags = (header >> 8) & 0xFFFF;
    uint32_t data_size = (header >> 24) & 0xFFFFFFFF;

    //Ensure is not too big
    if ((data_size + sizeof(NC_HEADER_MAGIC)) >= PERIMETER_MESSAGE_MAX_SIZE) {
        xassert(0);
        fprintf(stderr, "TCP recv header failed too long 0x%" PRIX64 " len %" PRIu32 "\n", header, data_size);
        return -2;
    }
    
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
