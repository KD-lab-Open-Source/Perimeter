#include "NetIncludes.h"
#include "NetConnection.h"
#include "crc.h"
#include "P2P_interface.h"
#include "NetConnectionAux.h"

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
    reset();
    
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
    reset();
    
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


size_t SendBufferToConnection(const uint8_t* buffer, size_t size, NetConnection* connection) {
    int retries = 5;
    if (!connection || !connection->is_active()) {
        return 0;
    }
    while (0 < retries) {
        int sent = connection->send(buffer, size);
        if (0 < sent) {
            return size;
        } else if (!connection->is_active()) {
            fprintf(stderr, "SendBufferToConnection error sending %lu sent %d to %lu closed\n", size, sent, connection->netid);
            break;
        } else {
            fprintf(stderr, "SendBufferToConnection error sending %lu sent %d to %lu retry %d\n", size, sent, connection->netid, retries);
            retries--;
        }
    }
    return 0;
}

size_t NetConnectionHandler::sendToNETID(const uint8_t* buffer, size_t size, NETID destination) {
    size_t sent = 0;
    if (destination == NETID_NONE) {
        fprintf(stderr, "Discarding sending to NETID_NONE\n");
    } else if (destination == NETID_ALL) {
        for (auto& conn : connections) {
            sent += SendBufferToConnection(buffer, size, conn.second);
        }
    } else {
        sent = SendBufferToConnection(buffer, size, getConnection(destination));
    }

    return sent;
}
