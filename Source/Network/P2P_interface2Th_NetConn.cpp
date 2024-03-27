#include "NetIncludes.h"
#include "P2P_interface.h"
#include "GameContent.h"
#include "NetConnectionAux.h"
#include "GameShell.h"
#include "Universe.h"
#include "ServerList.h"

void PNetCenter::SetConnectionTimeout(int _ms) {
    //Unused since SDL_net doesn't have any way to set connect or send timeouts 
    //connectionHandler->connection_timeout = ms;
}

void PNetCenter::RemovePlayer(NETID netid)
{
    fprintf(stdout, "RemovePlayer: 0x%" PRIX64 "\n", netid);
    if(isHost() && netid==m_localNETID && netid==m_hostNETID){
        ExecuteInternalCommand(PNC_COMMAND__RESET, false);
        ExecuteInterfaceCommand(PNC_INTERFACE_COMMAND_HOST_TERMINATED_GAME);
    } else {
        connectionHandler.terminateNETID(netid);
    }
}

void PNetCenter::ReceivedNetConnectionMessage(NetConnection* connection, NetConnectionMessage* msg) {
    bool is_host = isHost();
    if (is_host) {
        if (msg->source == m_hostNETID) {
            fprintf(stderr, "Received msg from host! from 0x%" PRIX64 " for 0x%" PRIX64 "\n",
                    msg->source, msg->destination);
            xassert(0);
            delete msg;
            return;
        }
    } else {
        if (msg->source != m_hostNETID) {
            fprintf(stderr, "Received msg from non-host! from 0x%" PRIX64 " for 0x%" PRIX64 "\n",
                    msg->source, msg->destination);
            xassert(0);
            delete msg;
            return;
        }
    }

    if (msg->tell() < sizeof(uint32_t)) {
        fprintf(stderr, "Received msg that is too small! from 0x%" PRIX64 "\n", msg->source);
        xassert(0);
        delete msg;
        return;
    }

    uint32_t head = *(reinterpret_cast<uint32_t*>(msg->address()));
    if (head == NETCOM_BUFFER_PACKET_ID) {
        if (msg->destination != m_localNETID) {
            fprintf(stderr, "Received msg for someone else! from 0x%" PRIX64 " for 0x%" PRIX64 "\n",
                    msg->source, msg->destination);
            xassert(0);
        } else {
            m_InputPacketList.push_back(msg);
            return; //msg passed
        }
    } else if (head == NC_INFO_ID) {
        if (is_host) {
            NETID netid = msg->source;
            bool has_client = false;
            for (auto client : m_clients) {
                if (client->netidPlayer == netid) {
                    has_client = true;
                    break;
                }
            }
            if (has_client) {
                fprintf(stderr, "Got handshake when already has a client! from 0x%" PRIX64 "\n", msg->source);
                xassert(0);
            } else {
                ProcessClientHandshake(connection, msg);
                return; //msg passed
            }
        } else {
            fprintf(stderr, "Got handshake being a client! from 0x%" PRIX64 "\n", msg->source);
            xassert(0);
        }
    } else if (head == NC_INFO_REPLY_ID) {
        //On direct mode the local client has NONE as we don't know yet, in relay we get NETID already by this point
        if (m_localNETID != NETID_NONE && msg->destination != m_localNETID) {
            fprintf(stderr, "Received client handshake reply for someone else! from 0x%" PRIX64 " for 0x%" PRIX64 "\n",
                    msg->source, msg->destination);
            xassert(0);
        } else if (is_host) {
            fprintf(stderr, "Got handshake reply being a host! from 0x%" PRIX64 "\n", msg->source);
            xassert(0);
        } else {
            ProcessClientHandshakeResponse(connection, msg);
            return; //msg passed
        }
    } else {
        fprintf(stderr, "Received msg can't be identified! from 0x%" PRIX64 " head 0x%" PRIX32 "\n", msg->source, head);
        xassert(0);
    }

    //Cleanup
    delete msg;
}

bool PNetCenter::SendClientHandshake(NetConnection* connection) const {
    if (connection) {        
        //
        NetConnectionInfo connectInfo;
        connectInfo.set(currentShortVersion, gamePassword.c_str(), terGameContentSelect, m_PlayerName.c_str());
        XBuffer buffer(256, true);
        connectInfo.write(buffer);
        int ret = connection->send(&buffer, m_localNETID, m_hostNETID, CONNECTION_HANDSHAKE_TIMEOUT);
        if (0 < ret) {
            //We have to wait for response which will be handled later
            return true;
        } else if (ret == 0) {
            fprintf(stderr, "Client connection handshake reply timeout\n");
        } else {
            fprintf(stderr, "Client connection handshake reply error\n");
        }
    } else {
        fprintf(stderr, "Client connection not available\n");
    }

    //Failed
    gameShell->callBack_JoinGameReturnCode(GameShell::JG_RC_CONNECTION_ERR, "");
	return false;
}

void PNetCenter::ProcessClientHandshakeResponse(NetConnection* connection, NetConnectionMessage* msg) {
    std::string extraInfo;
    GameShell::e_JoinGameReturnCode rc = GameShell::JG_RC_CONNECTION_ERR;
    msg->set(0);
    NetConnectionInfoResponse response;
    response.read(*msg);
    printf("ProcessClientHandshakeResponse NETID 0x%" PRIX64 "\n", msg->source);
    if(response.checkOwnCorrect()){
        if (response.connectResult == e_ConnectResult::CR_OK) {
            if (msg->source != response.hostID) {
                response.connectResult = e_ConnectResult::CR_NONE;
                fprintf(stderr, "NetConnectionInfoResponse hostID mismatch 0x%" PRIX64 " with packet source 0x%" PRIX64 "\n", response.hostID, msg->source);
            }
            if (msg->destination != response.clientID) {
                response.connectResult = e_ConnectResult::CR_NONE;
                fprintf(stderr, "NetConnectionInfoResponse clientID mismatch 0x%" PRIX64 " with packet destination 0x%" PRIX64 "\n", response.clientID, msg->destination);
            }
            xassert(response.connectResult == e_ConnectResult::CR_OK);
        }
        switch (response.connectResult) {
            case e_ConnectResult::CR_NONE:
                break;
            case e_ConnectResult::CR_OK:
                m_hostNETID = response.hostID;
                m_localNETID = response.clientID;
                m_GameName = response.gameName;
                connectionHandler.setHasClient(m_hostNETID, true);
                if (!connection->isRelay()) {
                    //Reassign if needed
                    connectionHandler.reassignConnectionNETID(connection, m_hostNETID);
                }
                rc = GameShell::JG_RC_OK;
                break;
            case e_ConnectResult::CR_ERR_INCORRECT_SIGNATURE:
                rc = GameShell::JG_RC_SIGNATURE_ERR;
                break;
            case e_ConnectResult::CR_ERR_INCORRECT_ARCH:
                rc = GameShell::JG_RC_GAME_NOT_EQUAL_ARCH_ERR;
                break;
            case e_ConnectResult::CR_ERR_INCORRECT_VERSION:
                rc = GameShell::JG_RC_GAME_NOT_EQUAL_VERSION_ERR;
                break;
            case e_ConnectResult::CR_ERR_INCORRECT_CONTENT:
                rc = GameShell::JG_RC_GAME_NOT_EQUAL_CONTENT_ERR;
                break;
            case e_ConnectResult::CR_ERR_INCORRECT_CONTENT_FILES:
                rc = GameShell::JG_RC_GAME_NOT_EQUAL_CONTENT_ERR;
                if (!response.gameContentList.empty()) {
                    extraInfo += "RMM=resource/models/main\n";
                    std::string differ;
                    std::string missing;
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
            case e_ConnectResult::CR_ERR_GAME_STARTED:
                rc = GameShell::JG_RC_GAME_IS_RUN_ERR;
                break;
            case e_ConnectResult::CR_ERR_GAME_FULL:
                rc = GameShell::JG_RC_GAME_IS_FULL_ERR;
                break;
            case e_ConnectResult::CR_ERR_INCORRECT_PASWORD:
                rc = GameShell::JG_RC_PASSWORD_ERR;
                break;
        }
    }

    delete msg;

    flag_connected = rc == GameShell::JG_RC_OK;
    if (isConnected()) {
        serverList->stopFind();
    } else {
        //Change state instead of using PNC_COMMAND__RESET
        //or PNC_STATE__CLIENT_TUNING_GAME will show host disconnected dialog instead of our result
        m_state = PNC_STATE__RESETTING;
    }

    gameShell->callBack_JoinGameReturnCode(rc, extraInfo);
};

bool PNetCenter::isConnected() const {
	return flag_connected;
}

//Out
size_t PNetCenter::SendNetBuffer(InOutNetComBuffer* netbuffer, NETID destination) {
    size_t sent = 0;
    if (destination == m_localNETID || (
            destination != m_hostNETID && 
            destination != NETID_ALL &&
            !isHost()
    )) {
        fprintf(stderr, "Discarding sending from 0x%" PRIX64 " to 0x%" PRIX64 "\n", m_localNETID, destination);
        xassert(0);
    } else {
        //Pinky promise that buf won't modify the buffer ptr
        XBuffer buf(reinterpret_cast<uint8_t*>(netbuffer->buf), netbuffer->filled_size);
        buf.set(netbuffer->filled_size);
        sent = connectionHandler.sendToNETID(&buf, m_localNETID, destination);
    }
    netbuffer->init();
    netbuffer->reset();
    netbuffer->byte_sending += sent;
    return sent;
}
