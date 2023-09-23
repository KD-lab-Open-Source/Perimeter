#include "NetIncludes.h"
#include "P2P_interface.h"
#include "GameContent.h"
#include "NetConnectionAux.h"
#include "GameShell.h"
#include "Universe.h"

arch_flags server_arch_mask = 0;
uint32_t server_content_crc = 0;

bool PNetCenter::Init()
{
    connectionHandler.reset();
	
    SetConnectionTimeout(TIMEOUT_DISCONNECT);
    
    m_hostNETID = m_localNETID = NETID_NONE;
    
    server_arch_mask = 0xFFFE; //Allow different OSes and compilers
    const char* server_arch_mask_str = check_command_line("ServerArchMask");
    if (server_arch_mask_str) {
        server_arch_mask = strtoull(server_arch_mask_str, nullptr, 16);
    }

    //Reset our attributes in case we played before
    loadUnitAttributes(false, nullptr);
    
    server_content_crc = get_content_crc();
    
#if defined(PERIMETER_DEBUG) && defined(PERIMETER_EXODUS) && 0
    //Dump current attrs
    XPrmOArchive ar("/tmp/test");
    XBuffer& buf = ar.buffer();
    ar << makeObjectWrapper(rigidBodyPrmLibrary(), nullptr, nullptr);
    ar.close();
#endif

	return true;
}

bool PNetCenter::ServerStart()
{    
    m_hostNETID = m_localNETID = NETID_NONE;
    flag_connected = connectionHandler.startListening(hostConnection.port());
    if (!flag_connected) {
        fprintf(stderr, "Error listening on port %d\n", hostConnection.port());
    } else {
        m_hostNETID = m_localNETID = NETID_HOST;
    }
    return flag_connected;
}

void PNetCenter::SetConnectionTimeout(int _ms) {
    //Unused since SDL_net doesn't have any way to set connect or send timeouts 
    //connectionHandler->connection_timeout = ms;
}

void PNetCenter::RemovePlayer(NETID netid)
{
    fprintf(stdout, "RemovePlayer: %lu\n", netid);
    if(isHost() && netid==m_localNETID && netid==m_hostNETID){
        ExecuteInternalCommand(PNC_COMMAND__END_GAME, false);
        ExecuteInterfaceCommand(PNC_INTERFACE_COMMAND_HOST_TERMINATED_GAME);
    } else {
        NetConnection* connection = connectionHandler.getConnection(netid);
        xassert(connection);
        if (connection) {
            connection->close();
        }
    }
}

void PNetCenter::Close(bool flag_immediate)
{    
    connectionHandler.reset();
	flag_connected=false;
}

bool PNetCenter::Connect() {
    LogMsg("Connect %s\n", hostConnection.getString().c_str());
    m_hostNETID = m_localNETID = NETID_NONE;
    std::string extraInfo;

    GameShell::e_JoinGameReturnCode rc = GameShell::JG_RC_CONNECTION_ERR;
    NetConnection* connection = connectionHandler.startConnection(&hostConnection);
    if (connection) {        
        //Send the connection info to server, this provides the client data and other connection info
        NetConnectionInfo connectInfo;
        connectInfo.set(currentShortVersion, gamePassword.c_str(), terGameContentSelect, m_PlayerName.c_str());
        XBuffer buffer(256, true);
        connectInfo.write(buffer);
        connection->send(buffer);
        
        //Get the response and pass it to interface command
        int ret = connection->receive(buffer, CONNECTION_HANDSHAKE_TIMEOUT);
        if (0 < ret) {
            buffer.set(0);
            NetConnectionInfoResponse response;
            response.read(buffer);
            if(response.checkOwnCorrect()){
                switch (response.connectResult) {
                    case NetConnectionInfoResponse::CR_NONE:
                        break;
                    case NetConnectionInfoResponse::CR_OK:
                        connection->state = NC_STATE_ACTIVE;
                        m_hostNETID = response.hostID;
                        m_localNETID = response.clientID;
                        m_GameName = response.gameName;
                        rc = GameShell::JG_RC_OK;
                        break;
                    case NetConnectionInfoResponse::CR_ERR_INCORRECT_SIGNATURE:
                        rc = GameShell::JG_RC_SIGNATURE_ERR;
                        break;
                    case NetConnectionInfoResponse::CR_ERR_INCORRECT_ARCH:
                        rc = GameShell::JG_RC_GAME_NOT_EQUAL_ARCH_ERR;
                        break;
                    case NetConnectionInfoResponse::CR_ERR_INCORRECT_VERSION:
                        rc = GameShell::JG_RC_GAME_NOT_EQUAL_VERSION_ERR;
                        break;
                    case NetConnectionInfoResponse::CR_ERR_INCORRECT_CONTENT:
                        rc = GameShell::JG_RC_GAME_NOT_EQUAL_CONTENT_ERR;
                        break;
                    case NetConnectionInfoResponse::CR_ERR_INCORRECT_CONTENT_FILES:
                        rc = GameShell::JG_RC_GAME_NOT_EQUAL_CONTENT_ERR;
                        if (!response.gameContentList.empty()) {
                            extraInfo += "RMM=resource/models/main\n";
                            std::string differ = "";
                            std::string missing = "";
                            const auto& ourList = get_content_list();
                            for (const auto& entry : response.gameContentList) {
                                std::string shorted = entry.first;
                                string_replace_all(shorted, "resource/models/main", "RMM");
                                if (ourList.count(entry.first)) {
                                    uint32_t ourCRC = ourList.at(entry.first);
                                    if (ourCRC != entry.second) {
                                        if (!differ.empty()) differ += "\n";
                                        differ += shorted;
                                    }
                                } else {
                                    //We don't have this file
                                    if (!missing.empty()) missing += "\n";
                                    missing += shorted;
                                }
                            }
                            if (!differ.empty()) extraInfo += "Differ:\n" + differ + "\n";
                            if (!missing.empty()) extraInfo += "Missing:\n" + missing + "\n";
                        }
                        break;
                    case NetConnectionInfoResponse::CR_ERR_GAME_STARTED:
                        rc = GameShell::JG_RC_GAME_IS_RUN_ERR;
                        break;
                    case NetConnectionInfoResponse::CR_ERR_GAME_FULL:
                        rc = GameShell::JG_RC_GAME_IS_FULL_ERR;
                        break;
                    case NetConnectionInfoResponse::CR_ERR_INCORRECT_PASWORD:
                        rc = GameShell::JG_RC_PASSWORD_ERR;
                        break;
                }
            }
        } else if (ret == 0) {
            fprintf(stderr, "Client connection handshake reply timeout\n");
        } else {
            fprintf(stderr, "Client connection handshake reply error\n");
        }
    } else {
        fprintf(stderr, "Client connection not available\n");
    }

    flag_connected = rc == GameShell::JG_RC_OK;

    gameShell->callBack_JoinGameReturnCode(rc, extraInfo);

	return flag_connected;
}

bool PNetCenter::isConnected() const {
	return flag_connected;
}

void sendToConnection(const char* buffer, size_t size, NetConnection* connection) {
    int retries = 5;
    if (!connection || !connection->is_active()) {
        return;
    }
    while (0 < retries) {
        int sent = connection->send(buffer, size);
        if (0 < sent) {
            return;
        } else if (!connection->is_active()) {
            fprintf(stderr, "sendToConnection error sending %lu sent %d to %lu closed\n", size, sent, connection->netid);
            break;
        } else {
            fprintf(stderr, "sendToConnection error sending %lu sent %d to %lu retry %d\n", size, sent, connection->netid, retries);
            retries--;
        }
    }
}

size_t PNetCenter::Send(const char* buffer, size_t size, NETID destination) {
    if (destination == NETID_ALL) {
        for (auto& conn : connectionHandler.getConnections()) {
            sendToConnection(buffer, size, conn.second);
        }
    } else {
        if (destination == m_localNETID
         || (destination != m_hostNETID && !isHost())
         || destination == NETID_NONE
        ) {
            fprintf(stderr, "Discarding sending %lu bytes to %lu from %lu\n", size, destination, m_localNETID);
            //xassert(0);
            return size;
        }
        sendToConnection(buffer, size, connectionHandler.getConnection(destination));
    }

    return size;
}

void PNetCenter::handleIncomingClientConnection(NetConnection* connection) {
    NetConnectionInfoResponse response;
    XBuffer buffer;
    int ret = -1;
    
    //If netid is NONE then it wasnt allocated into pool
    if (connection->netid == NETID_NONE) {
        fprintf(stderr, "Incoming connection couldn't be allocated\n");
        //Send the reply that game is full and close it without reading connection info
        response.reject(NetConnectionInfoResponse::CR_ERR_GAME_FULL);
    } else {
        //Read incoming data into buffer
        ret = connection->receive(buffer, CONNECTION_HANDSHAKE_TIMEOUT);
    }

    if (ret == 0) {
        fprintf(stderr, "Incoming connection handshake timeout\n");
        response.reject(NetConnectionInfoResponse::CR_NONE);
    } else if (0 < ret) {
        //Load data
        buffer.set(0);
        NetConnectionInfo clientInfo;
        clientInfo.read_header(buffer);

        //Check header first in case format changed in future
        if (!clientInfo.isIDCorrect()) {
            fprintf(stderr, "Connection info header ID failed\n");
            response.reject(NetConnectionInfoResponse::CR_ERR_INCORRECT_SIGNATURE);
        } else if (!clientInfo.isVersionCorrect(currentShortVersion)) { //Несоответствующая версия игры
            response.reject(NetConnectionInfoResponse::CR_ERR_INCORRECT_VERSION);
        } else {
            //Read all the info if OK
            clientInfo.read_content(buffer);

            //Check rest
            if (!clientInfo.checkCRCCorrect()) {
                fprintf(stderr, "Connection info CRC failed\n");
                response.reject(NetConnectionInfoResponse::CR_ERR_INCORRECT_SIGNATURE);
            } else if (!clientInfo.isArchCompatible(~server_arch_mask)) {
                response.reject(NetConnectionInfoResponse::CR_ERR_INCORRECT_ARCH);
            } else if (!clientInfo.isGameContentCompatible(terGameContentSelect)) {
                response.reject(NetConnectionInfoResponse::CR_ERR_INCORRECT_CONTENT);
            } else if (!clientInfo.isGameContentCRCCorrect(server_content_crc)) {
                response.reject(NetConnectionInfoResponse::CR_ERR_INCORRECT_CONTENT_FILES);
            } else if (m_bStarted) { // Игра запущена
                response.reject(NetConnectionInfoResponse::CR_ERR_GAME_STARTED);
            } else if ((!gamePassword.empty()) && (!clientInfo.isPasswordCorrect(gamePassword.c_str()))) {
                response.reject(NetConnectionInfoResponse::CR_ERR_INCORRECT_PASWORD);
            } else {
                //Print warning if arch is diff but was masked
                if (!clientInfo.isArchCompatible(0)) {
                    fprintf(
                        stderr, "Arch mismatch, desync may happen! Server %llX Client %llX Mask %llX\n",
                        NetConnectionInfo::computeArchFlags(), clientInfo.getArchFlags(), server_arch_mask
                    );
                }

                //All OK
                PlayerData pd;
                pd.set(clientInfo.getPlayerName(), connection->netid);
                int resultIdx = AddClient(pd);
                if (resultIdx == -1) {// Игра полная
                    response.reject(NetConnectionInfoResponse::CR_ERR_GAME_FULL);
                } else {
                    response.accept(NetConnectionInfoResponse::CR_OK, pd.netid, m_hostNETID, m_GameName);
                    connection->state = NC_STATE_ACTIVE;
                }
            }
        }
    }

    //Respond connection if there is result
    if (response.connectResult != NetConnectionInfoResponse::CR_NONE) {
        XBuffer responseBuffer(256, true);
        response.write(responseBuffer);
        ret = connection->send(responseBuffer);
    }

    //Close connection if result was not OK
    if (0 < ret && response.connectResult != NetConnectionInfoResponse::CR_OK) {
        ret = -1;
    }

    //If not OK close it
    if (ret <= 0) {
        fprintf(stderr, "Incoming connection %lu closed response %d ret %d\n", connection->netid, response.connectResult, ret);
        connection->close();
        
        //Delete connection since is not stored anywhere
        if (connection->netid == NETID_NONE) {
            delete connection;
        }
    }
}

void PNetCenter::ExitClient(NETID netid) {
    LogMsg("ExitClient NID %lu\n", netid);

    NetConnection* conn = connectionHandler.getConnection(netid);
    if (conn && !conn->is_closed()) {
        conn->close();
        //Mark it as closed, since we processed the client
        conn->state = NC_STATE_CLOSED;
    }
    if (m_state != PNC_STATE__NONE) {
        DeleteClient(netid, true);
    }
}

void PNetCenter::DeleteClient(NETID netid, bool normalExit) {
    LogMsg("DeleteClient NID %lu normal %d\n", netid, normalExit);
    if(isHost()){
        hostMissionDescription->setChanged();

        if (m_bStarted) {
            //idx == playerID (на всякий случай);
            int idx=hostMissionDescription->findPlayer(netid);
            //xassert(idx!=-1);
            if (normalExit && idx!=-1) {
                int playerID=hostMissionDescription->playersData[idx].playerID;
                netCommand4G_ForcedDefeat* pncfd=new netCommand4G_ForcedDefeat(playerID);
                m_QueuedGameCommands.push_back(pncfd);
            }
        }


        ClientMapType::iterator p;
        for(p=m_clients.begin(); p!= m_clients.end(); p++) {
            if((*p)->netidPlayer==netid) {
                LogMsg("Client NID %lu disconnecting\n", (*p)->netidPlayer);
                delete *p;
                m_clients.erase(p);
                break;
            }
        }

        hostMissionDescription->disconnectPlayer2PlayerDataByNETID(netid);
    }
    else {
        if (netid == NETID_HOST) {
            ExecuteInternalCommand(PNC_COMMAND__END_GAME, false);
            ExecuteInterfaceCommand(PNC_INTERFACE_COMMAND_HOST_TERMINATED_GAME);
        }
    }
    if(m_bStarted){
        int idx=clientMissionDescription->findPlayer(netid);
        //xassert(idx!=-1);
        if(idx!=-1){
            //отсылка сообщения о том, что игрок вышел
            PlayerData& pd = clientMissionDescription->playersData[idx];
            std::unique_ptr<LocalizedText> text = std::make_unique<LocalizedText>(
                pd.name(), getLocale()
            );
            ExecuteInterfaceCommand(
                normalExit ? PNC_INTERFACE_COMMAND_INFO_PLAYER_EXIT : PNC_INTERFACE_COMMAND_INFO_PLAYER_DISCONNECTED,
                std::move(text)
            );
        }
        //Удаление игрока из clientMD
        clientMissionDescription->disconnectPlayer2PlayerDataByNETID(netid);
    }

}


void PNetCenter::LockInputPacket(void)
{
    flag_LockIputPacket++;
}

