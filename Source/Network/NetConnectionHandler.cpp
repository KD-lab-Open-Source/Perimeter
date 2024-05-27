#include "NetIncludes.h"
#include "NetConnection.h"
#include "crc.h"
#include "P2P_interface.h"
#include "NetConnectionAux.h"
#include "NetRelay.h"
#include "codepages/codepages.h"
#include "NetRelaySerialization.h"

NetConnectionHandler::NetConnectionHandler(PNetCenter* center): net_center(center) {
    stopConnections();
}

NetConnectionHandler::~NetConnectionHandler() {
    reset();
    net_center = nullptr;
}

void NetConnectionHandler::reset() {
    stopListening();
    stopRelay();
    stopConnections();
}

void NetConnectionHandler::readConnectionMessages(NetConnection* connection, size_t max_packets) {
    size_t total_recv = 0;
    size_t i = 0;
    while (total_recv < PERIMETER_MESSAGE_MAX_SIZE * max_packets && i < max_packets) {
        i += 1;
        //Packet ptr will be set if received one successfully
        NetConnectionMessage* packet = nullptr;
        int len = connection->receive(&packet, 0);
        if (!packet) {
            break;
        }
        total_recv += len;
        if (0 < len) {
            if (connection->is_relay && connection->netid == packet->source) {
                this->receivedRelayMessage(connection, packet);
            } else {
                net_center->ReceivedNetConnectionMessage(connection, packet);
            }
        } else {
            delete packet;
        }
    }
}

void NetConnectionHandler::acceptConnection() {
#ifndef EMSCRIPTEN
    if (accept_transport) {
        TCPsocket incoming_socket = SDLNet_TCP_Accept(accept_transport->getSocket());
        if(!incoming_socket) {
            SDLNet_SetError(nullptr);
        } else {
            NetConnection* incoming = nullptr;
            
            //Find any closed connection in array
            for (auto& entry : connections) {
                if (entry.second->isClosed()) {
                    incoming = entry.second; 
                    NetTransportTCP* transport = new NetTransportTCP(incoming_socket);
                    incoming->set_transport(transport, entry.first);
                    //Set initial time of contact
                    incoming->time_contact = clock_us();
                    break;
                }
            }

            //Couldn't find any connection to reuse, create new
            if (incoming == nullptr) {
                incoming = newConnectionFromTransport(
                    new NetTransportTCP(incoming_socket),
                    NETID_CLIENTS_START + connections.size()
                );
            }

            if (incoming) {
                if (incoming->getNETID() == NETID_NONE) {
                    //If netid is NONE then it wasnt allocated into pool
                    fprintf(stderr, "Incoming connection couldn't be allocated\n");
                    //Send the reply that game is full and close it without reading connection info
                    net_center->SendClientHandshakeResponse(incoming, incoming->getNETID(),
                                                            e_ConnectResult::CR_ERR_GAME_FULL);
                    //Delete connection since is not stored anywhere
                    delete incoming;
                }
            }
        }
    }
#endif
}

void NetConnectionHandler::setHasClient(NETID netid, bool state) {
    if (netid == NETID_NONE || netid == NETID_ALL) {
        xassert(0);
        return;
    }
    
    //Check relay
    if (relayPeers.count(netid)) {
        NetRelayPeerInfo& info = relayPeers.at(netid);
        xassert(!(state && info.has_client));
        info.has_client = state;
        return;
    }
    
    //Check local
    NetConnection* connection = getConnection(netid);
    if (connection) {
        if (connection->is_relay) {
            xassert(0);
            return;
        }
        if (state) {
            switch (connection->state) {
                case NC_STATE_HAS_TRANSPORT:
                    connection->state = NC_STATE_HAS_CLIENT;
                    break;
                case NC_STATE_HAS_CLIENT:
                case NC_STATE_ERROR_PENDING:
                case NC_STATE_CLOSE_PENDING:
                case NC_STATE_ERROR:
                case NC_STATE_CLOSED:
                    xassert(0);
                    break;
            }
        } else {
            switch (connection->state) {
                case NC_STATE_HAS_CLIENT:
                    connection->state = NC_STATE_HAS_TRANSPORT;
                    break;
                case NC_STATE_ERROR_PENDING:
                case NC_STATE_CLOSE_PENDING:
                case NC_STATE_HAS_TRANSPORT:
                case NC_STATE_ERROR:
                case NC_STATE_CLOSED:
                    break;
            }
        }
    }
}

void NetConnectionHandler::terminateNETID(NETID netid) {
    if (netid == NETID_NONE || netid == NETID_ALL) {
        xassert(0);
        return;
    }

    //Check relay
    if (relayPeers.count(netid)) {
        LogMsg("terminateNETID 0x%" PRIX64 " relay peer\n", netid);
        NetRelayPeerInfo& info = relayPeers.at(netid);
        if (info.rejected) {
            return;
        }
        info.rejected = true;
        if (net_center->isHost()) {
            //Notify relay to remove this peer connection,
            //relay will tell back when happens so the game can remove from relayPeers
            static NetRelayMessage_PeerClosePeer msg;
            msg.netid = netid;
            NetConnection* relay = getConnection(info.relay_netid);
            xassert(relay);
            if (relay) {
                sendNetRelayMessage(relay, &msg, net_center->m_localNETID);
            }
        }
        return;
    }

    //Check local
    NetConnection* conn = getConnection(netid);
    if (conn && !conn->isClosed()) {
        LogMsg("terminateNETID 0x%" PRIX64 " connection\n", netid);
        conn->close();
    }
}

void NetConnectionHandler::pollConnections() {
    uint64_t now = clock_us();
    for (auto& entry : connections) {
        NetConnection* connection = entry.second;
        if (connection->is_relay) {
            switch (connection->state) {
                case NC_STATE_HAS_TRANSPORT: {
                    readConnectionMessages(connection, 10);
                    break;
                }
                default:
                    LogMsg("Relay connection unknown state %d\n", connection->state);
                    [[fallthrough]];
                case NC_STATE_CLOSED:
                case NC_STATE_ERROR: {
                    //Mark it as closed, relay is in non-functional state
                    if (has_relay_connection) {
                        handleRelayDisconnected();
                    }
                    stopRelay();
                    break;
                }
            }
        } else {
            switch (connection->state) {
                case NC_STATE_HAS_TRANSPORT:
                    //Check if stayed too long without having a client assigned
                    if ((now - connection->time_contact) > (CONNECTION_HANDSHAKE_TIMEOUT * 1000)) {
                        fprintf(stderr, "Connection without client for too long! 0x%" PRIX64 "\n", connection->netid);
                        connection->close();
                        break;
                    }
                    [[fallthrough]];
                case NC_STATE_HAS_CLIENT: {
                    readConnectionMessages(connection, 10);
                    break;
                }
                case NC_STATE_ERROR_PENDING:
                case NC_STATE_CLOSE_PENDING: {
                    net_center->DeleteClient(connection->netid, false);
                    //Mark it as closed, since we processed the client
                    [[fallthrough]];
                }
                case NC_STATE_ERROR: {
                    connection->state = NC_STATE_CLOSED;
                    connection->close();
                    break;
                }
                case NC_STATE_CLOSED:
                    //We ignore closed ones as they will be reused
                    break;
            }
        }
    }

    std::map<NETID, bool> relayPeerTerminate;
    for (auto& pair : this->relayPeers) {
        NETID netid = pair.first;
        auto& info = pair.second;
        if (info.terminated) {
            continue;
        }
        if (!info.rejected && !info.has_client) {
            //Terminate peers that spent too much time to get a client
            if ((clock_us() - info.time_contact) > (CONNECTION_HANDSHAKE_TIMEOUT * 1000)) {
                terminateNETID(netid);
            }
        }
        if (info.rejected) {
            info.terminated = true;
            relayPeerTerminate.emplace(netid, info.has_client);
        }
    }
    for (auto& pair : relayPeerTerminate) {
        NETID netid = pair.first;
        if (pair.second) {
            net_center->DeleteClient(netid, false);
        } else {
            //If is the host and is not yet client (we are still in handshake) just stop relay
            if (netid == net_center->m_hostNETID) {
                if (has_relay_connection) {
                    handleRelayDisconnected();
                }
                stopRelay();
            }
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
    if (netid == NETID_NONE || netid == NETID_ALL) {
        xassert(0);
        return nullptr;
    }
    NetConnection* conn = nullptr;
    if (connections.count(netid)) {
        NetConnection* candidate = connections.at(netid);
        if (!candidate->isClosed()) {
            conn = candidate;
        }
    }
    return conn;
}

bool NetConnectionHandler::startHost(uint16_t listen_port, bool start_public_room) {
    reset();
    
    //Remove one since host is player too
    max_connections = NETWORK_PLAYERS_MAX - 1;
    
    bool ok = true;
    if (start_public_room) {
        if (!startRelayRoom()) {
            ok = false;
        }
    }

#ifdef EMSCRIPTEN
    ok = ok && start_public_room;
#else
    if (ok && 0 < listen_port) {
        NetAddress addr(INADDR_ANY, listen_port);
        NetTransport* transport = NetTransport::create(addr);
        if (transport == nullptr || dynamic_cast<NetTransportTCP*>(transport) == nullptr) {
            ok = false;
            delete transport;
        } else {
            accept_transport = dynamic_cast<NetTransportTCP*>(transport);
            LogMsg("TCP listening on port %d\n", listen_port);
        }
    }
#endif

    if (!ok && start_public_room) {
        stopRelay();
    }
    
    return ok;
}

void NetConnectionHandler::stopListening() {
    if (accept_transport) {
        LogMsg("TCP listen socket closed\n");
        delete accept_transport;
        accept_transport = nullptr;
    }
}

bool NetConnectionHandler::startRelayRoom() {
    //Create relay connection
    NetAddress address;
    std::string primary_relay;
    getPrimaryNetRelayAddress(primary_relay);
    if (primary_relay.empty() || !NetAddress::resolve(address, primary_relay, NET_RELAY_DEFAULT_PORT)) {
        return false;
    }
    NetTransport* transport = NetTransport::create(address);
    if (!transport) {
        return false;
    }
    NetConnection* connection = newConnectionFromTransport(transport, NETID_RELAY);
    if (!connection || connection->netid == NETID_NONE) {
        delete connection;
        return false;
    }
    connection->is_relay = true;

    //Do the request to list relays we can use to create the room
    static NetRelayMessage_PeerListLobbyHosts msg_lobby_hosts;
    static NetRelayMessage_RelayListLobbies response_lobby_hosts;
    bool ok = sendNetRelayMessage(connection, &msg_lobby_hosts, NETID_NONE);
    if (ok) ok = receiveNetRelayMessage(connection, NETID_NONE, &response_lobby_hosts, RELAY_MSG_RELAY_LIST_LOBBY_HOSTS);
    std::vector<NetRelay_LobbyHost> hosts = {};
    if (ok) {
        XPrmIArchive ia;
        std::swap(ia.buffer(), response_lobby_hosts.data);
        ia.reset();
        ia >> hosts;
        if (hosts.empty()) {
            ok = false;
        }
    }
    if (!ok) {
        fprintf(stderr, "No relay host available to connect at\n");
        return false;
    }

    //Pick a host if necessary
    bool use_current_relay = false;
    NetAddress host_address;
    for (NetRelay_LobbyHost& host: hosts) {
        std::string host_address_str = host.getAddress();
        if (host_address_str == primary_relay) {
            LogMsg("Current primary relay selected based on host string address\n");
            use_current_relay = true;                
            break;
        }
        if (!NetAddress::resolve(
            host_address,
            host_address_str,
            NET_RELAY_DEFAULT_PORT
        )) {
            continue;
        }
        if (host_address == address) {
            LogMsg("Current primary relay selected based on host resolved address\n");
            use_current_relay = true;
            break;
        }
    }

    //Use last host we got from list and establish connection        
    if (!use_current_relay) {
        connection->close();
        LogMsg("Connecting to secondary relay for room creation\n");

        address = host_address;
        transport = NetTransport::create(address);
        if (!transport) {
            return false;
        } else {
            connection = newConnectionFromTransport(transport, NETID_RELAY);
            if (!connection || connection->netid == NETID_NONE) {
                delete connection;
                return false;
            }
            connection->is_relay = true;
        }
    }

    //Send our room info to create it
    const NetRelayMessage_PeerSetupRoom& msg_setup_room = net_center->GenerateRelaySetupRoom();
    ok = sendNetRelayMessage(connection, &msg_setup_room, NETID_HOST);

    //Receive list of peers and our own netid
    if (ok) {
        has_relay_connection = true;
        NetConnectionMessage* msg_response = nullptr;
        connection->receive(&msg_response, CONNECTION_RELAY_TIMEOUT);
        if (!msg_response) {
            ok = false;
        } else {
            receivedRelayMessage(connection, msg_response);
            ok = net_center->m_localNETID != NETID_NONE
              && net_center->m_hostNETID != NETID_NONE;
        }
    }
    
    return ok;
}

NetConnection* NetConnectionHandler::startRelayRoomConnection(const NetAddress& address, NetRoomID room_id) {
    reset();

    max_connections = 1;

    //Start relay connection and assign as host since it will act as one after sending join room
    NetTransport *transport = NetTransport::create(address);
    if (!transport) {
        return nullptr;
    }
    NetConnection* connection = newConnectionFromTransport(transport, NETID_RELAY);
    if (!connection || connection->netid == NETID_NONE) {
        delete connection;
        stopRelay();
        return nullptr;
    }

    connection->is_relay = true;
    has_relay_connection = true;

    //Send the request to join room
    static XBuffer buf(1024, true);
    buf.init();
    static NetRelayMessage_PeerJoinRoom msg;
    msg.room = room_id;
    msg.write(buf);

    bool ok = sendNetRelayMessage(connection, &msg, NETID_NONE);

    //Receive list of peers and our own netid
    if (ok) {
        NetConnectionMessage* msg_response = nullptr;
        connection->receive(&msg_response, CONNECTION_RELAY_TIMEOUT);
        if (!msg_response) {
            ok = false;
        } else {
            receivedRelayMessage(connection, msg_response);
            ok = net_center->m_localNETID != NETID_NONE
              && net_center->m_hostNETID != NETID_NONE;
        }
    }

    if (!ok) {
        stopRelay();
    }

    //Now that we are in room we can start talking to game host
    return connection;
}

void NetConnectionHandler::stopRelay() {
    has_relay_connection = false;
    relayPeers.clear();
    for (auto& pair : connections) {
        NetConnection* connection = pair.second;
        if (connection && connection->is_relay && !connection->isClosed()) {
            LogMsg("Relay connection 0x%" PRIX64 " closed\n", connection->netid);
            connection->close();
        }
    }
}

void NetConnectionHandler::handleRelayDisconnected() {
    if (net_center && net_center->isConnected()) {
        net_center->ExecuteInternalCommand(PNC_COMMAND__RESET, false);
        if (net_center->isHost()) {
            net_center->ExecuteInterfaceCommand(PNC_INTERFACE_COMMAND_CONNECTION_FAILED);
        } else {
            net_center->ExecuteInterfaceCommand(PNC_INTERFACE_COMMAND_HOST_TERMINATED_GAME);
        }
    }
}

NetConnection* NetConnectionHandler::startDirectConnection(const NetAddress& address) {
    max_connections = 1;

    NetTransport *transport = NetTransport::create(address);
	if (!transport) {
        return nullptr;
	}
    
    NetConnection* connection = newConnectionFromTransport(transport, NETID_HOST);
    if (connection && connection->netid != NETID_NONE) {
        return connection;
    } else {
        fprintf(stderr, "Error allocating new connection\n");
        delete connection;
        return nullptr;
    }
}

NetConnection* NetConnectionHandler::newConnectionFromTransport(NetTransport* transport, NETID netid) {
    NetConnection* connection = nullptr;
    if (connections.size() < max_connections) {
        if (connections.count(netid)) {
            //Already exists, reuse
            connection = connections.at(netid);
            if (!connection->isClosed()) {
                fprintf(stderr, "Connection to " PRIX64 " already allocated!\n");
                xassert(0);
            }
        } else {
            connection = new NetConnection(nullptr, netid);
            auto result = connections.try_emplace(netid, connection);
            if (!result.second) {
                xassert(0);
                delete connection;
                connection = nullptr;
            }
        }
        if (connection && connection->getNETID() != netid) {
            xassert(0);
            connection->netid = netid;
        }
    }
    if (!connection) {
        //Connection couldn't be assigned, return empty one so we can put socket on it
        connection = new NetConnection(nullptr, NETID_NONE);
    }
    connection->set_transport(transport, netid);
    //Set initial time of contact
    connection->time_contact = clock_us();
    return connection;
}

size_t SendBufferToConnection(
        const XBuffer* buffer,
        NetConnection* connection,
        NETID source, NETID destination,
        int32_t timeout
) {
    int retries = 5;
    if (!connection || !connection->hasTransport() || connection->isClosed()) {
        return 0;
    }
    while (0 < retries) {
        int sent = connection->send(buffer, source, destination, timeout);
        if (0 < sent) {
            return buffer->tell();
        } else {
            fprintf(stderr, "SendBufferToConnection error sending %" PRIsize " sent %d to 0x%" PRIX64 " retry %d\n", buffer->tell(), sent, connection->getNETID(), retries);
            retries--;
        }
    }
    return 0;
}

size_t NetConnectionHandler::sendToNETID(const XBuffer* buffer, NETID source, NETID destination) {
    if (destination == NETID_NONE) {
        xassert(0);
        fprintf(stderr, "Discarding sending from 0x%" PRIX64 " to NETID_NONE\n", source);
        return 0;
    }
    size_t sent = 0;
    if (destination == NETID_ALL) {
        //Send both to relay if any and to each peer
        for (auto& conn : connections) {
            sent += SendBufferToConnection(
                    buffer, conn.second,
                    source, conn.second->is_relay ? destination : NETID_NONE,
                    CONNECTION_ACTIVE_TIMEOUT
            );
        }
    } else if (has_relay_connection && 0 != relayPeers.count(destination)) {
        //Message is for relay or a peer behind relay
        NetRelayPeerInfo& info = relayPeers.at(destination);
        if (!info.rejected) {
            sent = SendBufferToConnection(
                    buffer, getConnection(info.relay_netid),
                    source, destination,
                    CONNECTION_RELAY_TIMEOUT
            );
        }
    } else {
        //Message is for connection
        sent = SendBufferToConnection(
                buffer, getConnection(destination),
                source, NETID_NONE,
                CONNECTION_ACTIVE_TIMEOUT
        );
    }

    return sent;
}

bool NetConnectionHandler::reassignConnectionNETID(NetConnection* connection, NETID netid) {
    if (connection->netid != netid) {
        auto result = connections.try_emplace(netid, connection);
        if (result.second) {
            //Inserted, remove from current position and set new netid to connection
            auto it = connections.begin();
            auto end = connections.end();
            for (; it != end; ++it) {
                if ((*it).first == netid) {
                    connections.erase(it);
                    break;
                }
            }
            connection->netid = netid;
        }
    }
    
    if (connection->netid == netid) {
        return true;
    }
    
    //Couldn't be reassigned
    xassert(0);
    fprintf(stderr, "Couldn't reassign connection 0x%" PRIX64 " to 0x%" PRIX64 "\n", connection->netid, netid);
    return false;
}

void NetConnectionHandler::receivedRelayMessage(NetConnection* connection, NetConnectionMessage* msg) {
    NETID localNETID = net_center->m_localNETID;
    bool is_host = net_center->isHost();
    //Parse header to know what message was sent
    msg->set(0);
    static NetRelayMessage msg_header(RELAY_MSG_UNKNOWN);
    msg_header.read_relay_header(*msg);
    bool terminate = false;
    if (msg_header.protocol_version != NET_RELAY_PROTOCOL_VERSION) {
        xassert(0);
        fprintf(stderr, "[Relay] Unexpected protocol 0x%" PRIX32 "\n", msg_header.protocol_version);
        terminate = true;
    } else {
        bool wrong_destination;
        switch (msg_header.msg_type) {
            case RELAY_MSG_CLOSE:
                wrong_destination = msg->destination != NETID_NONE;
                break;
            default:
                wrong_destination = msg->destination != localNETID;
                break;
            case RELAY_MSG_RELAY_LIST_PEERS:
                wrong_destination = msg->destination == NETID_NONE;
                break;
        }
        if (wrong_destination) {
            fprintf(stderr, "[Relay] Received msg for someone else! for 0x%" PRIX64 "\n",
                    msg->destination);
            delete msg;
            return;
        }
        XBuffer buffer(128, true);
        switch (msg_header.msg_type) {
            default: {
                xassert(0);
                fprintf(stderr, "[Relay] connection sent unknown msg type: 0x%" PRIX32 "\n", msg_header.msg_type);
                break;
            }
            case RELAY_MSG_CLOSE: {
                terminate = true;
                static NetRelayMessage_Close message;
                message.read(*msg);
                LogMsg("[Relay] connection sent close msg: 0x%" PRIX32 "\n", message.code);
                break;
            }
            case RELAY_MSG_RELAY_LIST_LOBBIES: {
                LogMsg("[Relay] connection sent result msg, discarding\n");
                break;
            }
            case RELAY_MSG_RELAY_PING: {
                static NetRelayMessage_RelayPing message;
                message.read(*msg);
                static NetRelayMessage_PeerPingResponse response;
                response.secs = message.secs;
                response.subsecs = message.subsecs;
                sendNetRelayMessage(connection, &response, localNETID);
                break;
            }
            case RELAY_MSG_RELAY_LIST_PEERS: {
                static NetRelayMessage_RelayListPeers message;
                message.read(*msg);
                if (is_host) {
                    net_center->m_hostNETID = localNETID = msg->destination;
                } else {
                    localNETID = msg->destination;
                    if (message.peers.empty()) {
                        net_center->m_hostNETID = NETID_NONE;
                    } else {
                        net_center->m_hostNETID = message.peers[0];
                    }
                }
                net_center->m_localNETID = localNETID;
                
                //Register any peer that isn't already present
                for (auto& netid : message.peers) {
                    if (localNETID == netid) {
                        xassert(0);
                        LogMsg("Got local NETID as peer? 0x%" PRIX64 "\n", netid);
                        continue;
                    }
                    if (0 == relayPeers.count(netid)) {
                        NetRelayPeerInfo info;
                        info.relay_netid = msg->source;
                        info.time_contact = clock_us();
                        relayPeers.try_emplace(netid, info);
                    }
                }

                LogMsg("[Relay] room list destination 0x%" PRIX64 " peers: %" PRIsize " [",
                       msg->destination, message.peers.size());
                for (NETID netid : message.peers) {
                    LogMsg(" 0x%" PRIX64, netid);
                }
                LogMsg(" ]\nNETIDs local: 0x%" PRIX64 " host: 0x%" PRIX64 "\n", net_center->m_localNETID, net_center->m_hostNETID);
                break;
            }
            case RELAY_MSG_RELAY_ADD_PEER: {
                static NetRelayMessage_RelayAddPeer message;
                message.read(*msg);
                LogMsg("[Relay] room add peer 0x%" PRIX64 "\n", message.netid);
                if (localNETID == message.netid) {
                    xassert(0);
                    LogMsg("Got local NETID as peer? 0x%" PRIX64 "\n", message.netid);
                } else {
                    NetRelayPeerInfo info;
                    info.relay_netid = msg->source;
                    info.time_contact = clock_us();
                    relayPeers.try_emplace(message.netid, info);
                }
                break;
            }
            case RELAY_MSG_RELAY_REMOVE_PEER: {
                static NetRelayMessage_RelayRemovePeer message;
                message.read(*msg);
                LogMsg("[Relay] room remove peer 0x%" PRIX64 "\n", message.netid);
                auto it = relayPeers.begin();
                auto end = relayPeers.end();
                for (; it != end; ++it) {
                    if ((*it).first == message.netid) {
                        NetRelayPeerInfo& info = (*it).second;
                        if (info.has_client) {
                            net_center->DeleteClient(message.netid, false);
                        }
                        relayPeers.erase(it);
                        break;
                    }
                }
                break;
            }
        }
    }

    //Cleanup
    delete msg;
    if (terminate) {
        if (has_relay_connection) {
            handleRelayDisconnected();
        }
        stopRelay();
    }
}

bool NetConnectionHandler::isConnectionClosed(NETID netid) {
    //Behind relay?
    if (0 < relayPeers.count(netid)) {
        const NetRelayPeerInfo& info = relayPeers.at(netid);
        return info.rejected || info.terminated;
    }
    
    //Is a normal connection?
    NetConnection* connection = getConnection(netid);
    if (connection) {
        return connection->state != NC_STATE_HAS_TRANSPORT
            && connection->state != NC_STATE_HAS_CLIENT;
    }
    
    return true;
}

bool NetConnectionHandler::hasClient(NETID netid, bool not_closed) {
    //Behind relay?
    if (0 < relayPeers.count(netid)) {
        const NetRelayPeerInfo& info = relayPeers.at(netid);
        if (not_closed && (info.rejected || info.terminated)) {
            return false;
        }
        return info.has_client;
    }

    //Is a normal connection?
    NetConnection* connection = getConnection(netid);
    if (connection) {
        switch (connection->state) {
            case NC_STATE_ERROR:
            case NC_STATE_HAS_TRANSPORT:
            case NC_STATE_CLOSED:
                break;
            case NC_STATE_HAS_CLIENT:
                if (not_closed) {
                    return connection->hasTransport();
                } else {
                    return true;
                }
            case NC_STATE_ERROR_PENDING:
            case NC_STATE_CLOSE_PENDING:
                if (!not_closed) {
                    return true;
                } else {
                    return false;
                }
        }
    }

    return false;
}
