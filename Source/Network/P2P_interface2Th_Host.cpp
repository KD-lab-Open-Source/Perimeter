#include "NetIncludes.h"

#include "Runtime.h"
#include "P2P_interface.h"
#include "NetConnectionAux.h"

#include "Universe.h"
#include "GameContent.h"

#include <algorithm>
#include <set>
#include <SDL.h>
#include "files/files.h"
#include "NetRelay.h"
#include "codepages/codepages.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////

PClientData::PClientData(const char* name, NETID netid)
{
    strncpy(playerName, name, PLAYER_MAX_NAME_LEN - 1);
    netidPlayer = netid;
	m_flag_Ready=false;
	lagQuant=0;
	lastExecuteQuant=0;
	curLastQuant=0;
	lastTimeBackPacket=clocki();
    last_time_latency_response = clock_us();
	confirmQuant=0;
    
    desync_amount=0;
    desync_last_time = 0;
    desync_state=PNC_DESYNC_NONE;
    desync_missionDescription=nullptr;
    desync_netlog.alloc(0);

	requestPause=false;
	clientPause=false;
	timeRequestPause=0;

}

PClientData::~PClientData() = default;

//////////////////////////////////////////////////////////////////////////////////////////////////////

void PNetCenter::ProcessClientHandshake(NetConnection* connection, NetConnectionMessage* msg) {
    //Is connection active?
    if (!connection->hasTransport()) {
        return;
    }
    
    //Clients connecting directly don't know own NETID yet as we assign them so we can't use msg source
    NETID netid = connection->isRelay() ? msg->source : connection->getNETID();
    printf("ProcessClientHandshake NETID 0x%" PRIX64 "\n", netid);
    
    e_ConnectResult connectResult = CR_NONE;
    msg->set(0);
    NetConnectionInfo clientInfo;
    clientInfo.read_header(*msg);

    //Check header first in case format changed in future
    if (!clientInfo.isIDCorrect()) {
        fprintf(stderr, "Connection info header ID failed\n");
        connectResult = e_ConnectResult::CR_ERR_INCORRECT_SIGNATURE;
    } else if (!clientInfo.isVersionCorrect(currentShortVersion)) { //Несоответствующая версия игры
        connectResult = e_ConnectResult::CR_ERR_INCORRECT_VERSION;
    } else {
        //Read all the info if OK
        clientInfo.read_content(*msg);

        //Check rest
        if (!clientInfo.checkCRCCorrect()) {
            fprintf(stderr, "Connection info CRC failed\n");
            connectResult = e_ConnectResult::CR_ERR_INCORRECT_SIGNATURE;
        } else if (!clientInfo.isArchCompatible(~server_arch_mask)) {
            connectResult = e_ConnectResult::CR_ERR_INCORRECT_ARCH;
            fprintf(
                    stderr, "Arch mismatch, Server 0x%" PRIX64 " Client 0x%" PRIX64 " Mask 0x%" PRIX64 "\n",
                    computeArchFlags(), clientInfo.getArchFlags(), server_arch_mask
            );
        } else if (!clientInfo.isGameContentCompatible(terGameContentSelect)) {
            connectResult = e_ConnectResult::CR_ERR_INCORRECT_CONTENT;
        } else if (!clientInfo.isGameContentCRCCorrect(server_content_crc)) {
            connectResult = e_ConnectResult::CR_ERR_INCORRECT_CONTENT_FILES;
        } else if (m_bStarted) { // Игра запущена
            connectResult = e_ConnectResult::CR_ERR_GAME_STARTED;
        } else if ((!gamePassword.empty()) && (!clientInfo.isPasswordCorrect(gamePassword.c_str()))) {
            connectResult = e_ConnectResult::CR_ERR_INCORRECT_PASWORD;
        } else {
            //Print warning if arch is diff but was masked
            if (!clientInfo.isArchCompatible(0xFFFFFFFFFFFFFFFF)) {
                fprintf(
                        stderr, "Arch mismatch, desync may happen! Server 0x%" PRIX64 " Client 0x%" PRIX64 " Mask 0x%" PRIX64 "\n",
                        computeArchFlags(), clientInfo.getArchFlags(), server_arch_mask
                );
            }

            //All OK, load player data
            PlayerData pd;
            pd.set(clientInfo.getPlayerName(), netid);
            int resultIdx = AddClient(pd);
            if (resultIdx < 0) {
                connectResult = e_ConnectResult::CR_ERR_GAME_FULL;
            } else {
                connectResult = e_ConnectResult::CR_OK;
            }
            
            //Set as having client
            connectionHandler.setHasClient(netid, true);
        }
    }
    delete msg;

    xassert(connectResult != CR_NONE);
    SendClientHandshakeResponse(connection, netid, connectResult);
}

void PNetCenter::SendClientHandshakeResponse(NetConnection* connection, NETID netid, e_ConnectResult result) {
    printf("SendClientHandshakeResponse NETID 0x%" PRIX64 " response: %d\n", netid, result);
    NetConnectionInfoResponse response;
    if (result == e_ConnectResult::CR_OK) {
        //Connection is active now
        response.accept(result, netid, m_localNETID, m_GameName);
        response.send_to_connection(connectionHandler, m_localNETID, netid);
    } else {
        if (result != e_ConnectResult::CR_NONE) {
            response.reject(result);
            response.send_to_connection(connectionHandler, m_localNETID, netid);
        }

        //Close connection if result was not OK
        fprintf(stderr, "Incoming connection NETID 0x%" PRIX64 " closed, host response: %d\n", netid, result);
        connectionHandler.terminateNETID(netid);
    }
}

int PNetCenter::AddClient(PlayerData& pd)
{
    CAutoLock _lock(m_GeneralLock); //В этой функции в некоторых вызовах будет вложенный

    MissionDescription& mission = *hostMissionDescription;
    int idxPlayerData=-1;
    if (mission.gameType_ == GT_MULTI_PLAYER_CREATE) {
        idxPlayerData=mission.connectNewPlayer2PlayersData(pd);
    } else if(mission.gameType_ == GT_MULTI_PLAYER_LOAD) {
        idxPlayerData=mission.connectLoadPlayer2PlayersData(pd);
    }

    mission.setChanged();

    if (0 <= idxPlayerData && pd.netid != NETID_NONE) {
        //missionDescription.playersData[idxPlayerData].netid=netid;
        //missionDescription.playersData[idxPlayerData].flag_playerStartReady=1;

        PClientData* pCD=new PClientData(pd.name(), pd.netid);
        pCD->backGameInf2List.reserve(20000);//резерв под 20000 квантов
        m_clients.push_back(pCD);

        LogMsg("New client idx: %d netid: 0x%" PRIX64 " name: %s for game %s\n", idxPlayerData, pd.netid, pd.name(), m_GameName.c_str());

        return idxPlayerData;
    } else {
        LogMsg("Client netid: 0x%" PRIX64 " name: %s for game %s id denied\n", pd.netid, pd.name(), m_GameName.c_str());
        return -1;
    }
}


void PNetCenter::ExitClient(NETID netid) {
    DeleteClient(netid, true);
}

void PNetCenter::DeleteClient(NETID netid, bool normalExit) {
    LogMsg("DeleteClient NID 0x%" PRIX64 " normal %d\n", netid, normalExit);
    CAutoLock _lock(m_GeneralLock);
    if (netid == NETID_ALL || netid == NETID_NONE) {
        xassert(0);
        return;
    }
    if (!connectionHandler.hasClient(netid, false)) {
        connectionHandler.terminateNETID(netid);
        return;
    }
    connectionHandler.setHasClient(netid, false);
    connectionHandler.terminateNETID(netid);
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
                LogMsg("Client NID 0x%" PRIX64 " disconnecting\n", (*p)->netidPlayer);
                delete *p;
                m_clients.erase(p);
                break;
            }
        }

        hostMissionDescription->disconnectPlayer2PlayerDataByNETID(netid);
    }
    else {
        if (netid == m_hostNETID) {
            ExecuteInternalCommand(PNC_COMMAND__RESET, false);
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

void PNetCenter::UpdateBattleData() {
    MissionDescription* mission = hostMissionDescription;

    if (mission->gameType_ == GT_MULTI_PLAYER_CREATE) {
        //Random xchg
        ////random_shuffle(missionDescription.playersData, missionDescription.playersData+missionDescription.playersAmount);
        mission->shufflePlayers();
    } else if (mission->gameType_ == GT_MULTI_PLAYER_LOAD) {
        //Set the current allocated client player names
        for(int i=0; i<mission->playerAmountScenarioMax; i++) {
            PlayerData& pd = mission->playersData[i];
            if (pd.realPlayerType == REAL_PLAYER_TYPE_PLAYER) {
                for (auto client : m_clients) {
                    if (client->netidPlayer == pd.netid) {
                        pd.setName(client->playerName);
                        pd.setNameInitial(client->playerName);
                        break;
                    }
                }
            }
        }
    }
    mission->packPlayerIDs();

    LogMsg("Battle info ready\n");
}

void PNetCenter::SendBattleData() {
    MissionDescription* mission = hostMissionDescription;

    //Load unit attrs before sending them
    loadUnitAttributes(false, nullptr);
    //initUnitAttributes();

    //Load current attributes into host mission
    XPrmOArchive oaScripts;
    oaScripts.binary_friendly = true;
    oaScripts << WRAP_NAME(rigidBodyPrmLibrary(), "rigidBodyPrmLibrary");
    oaScripts << WRAP_NAME(attributeLibrary(), "attributeLibrary");
    oaScripts << WRAP_NAME(globalAttr(), "globalAttr");
    std::swap(mission->scriptsData, oaScripts.buffer());

    //Close open positions
    for (auto& pd : mission->playersData) {
        if (pd.realPlayerType == REAL_PLAYER_TYPE_OPEN) {
            pd.realPlayerType = REAL_PLAYER_TYPE_CLOSE;
        }
    }

    //Send data
    //Don't change player data here or prev players will have outdated info about next players 
    for (int i=0; i<mission->playerAmountScenarioMax; i++) {
        const PlayerData& pd = mission->playersData[i];
        if (pd.realPlayerType == REAL_PLAYER_TYPE_PLAYER) {
            mission->activePlayerID = pd.playerID;
            netCommand4C_StartLoadGame nccsl(mission);
            printf("Sending mission to idx %d id %d netid 0x%" PRIX64 "\n", i, pd.playerID, pd.netid);
            SendEvent(nccsl, pd.netid);
        }
    }
    mission->activePlayerID = mission->findPlayer(m_localNETID);

    LogMsg("Sent battle info\n");
}

void PNetCenter::UpdateCurrentMissionOnRelayRoom() {
    if (connectionHandler.hasRelayConnection()) {
        const NetRelayMessage_PeerSetupRoom& msg = GenerateRelaySetupRoom();
        for (auto& pair : connectionHandler.getConnections()) {
            NetConnection* connection = pair.second;
            if (connection && connection->isRelay() && !connection->isClosed()) {
                sendNetRelayMessage(connection, &msg, m_localNETID);
            }
        }
    }
}

void PNetCenter::UpdateCurrentMissionDescription4C()
{
    //Update room in relay if any
    UpdateCurrentMissionOnRelayRoom();

    //Send mission description to every real player
    const MissionDescription& md = *hostMissionDescription;
    for(int i=0; i<md.playerAmountScenarioMax; i++){
        if(md.playersData[i].realPlayerType==REAL_PLAYER_TYPE_PLAYER){
            netCommand4C_CurrentMissionDescriptionInfo nccmd(md, md.playersData[i].playerID);
            SendEvent(nccmd, md.playersData[i].netid);
        }
    }
}

void PNetCenter::CheckClients()
{
    ClientMapType::iterator i = m_clients.begin();
    while(i != m_clients.end())
    {
        if(!((*i)->m_flag_Ready))
        {
            LogMsg("Client NID 0x%" PRIX64 " is not ready. removing.\n", (*i)->netidPlayer);

            RemovePlayer((*i)->netidPlayer);

            delete *i;
            m_clients.erase(i++);
        }
        else
            i++;
    }
}

void PNetCenter::ResetAllClients()
{
    unsigned int curTime=clocki();
    size_t timestamp_us = clock_us();
    ClientMapType::iterator k;
    for(k=m_clients.begin(); k!=m_clients.end(); k++){
        (*k)->m_flag_Ready = false;
        (*k)->backGameInf2List.clear();
        //Reset timestamps to avoid kicking due to timeout
        (*k)->last_time_latency_response = timestamp_us;
        (*k)->lastTimeBackPacket = curTime;
    }
}

void PNetCenter::DumpClients()
{
    LogMsg("List of clients -------------------------\n");
    ClientMapType::iterator i;
    FOR_EACH(m_clients, i)
    {
        LogMsg("Client NID 0x%" PRIX64 "\n", (*i)->netidPlayer);
    }
    LogMsg("-----------------------------------------\n");
}

bool PNetCenter::AddClientToMigratedHost(const NETID _netid, unsigned int _curLastQuant, unsigned int _confirmQuant)
{
    //поиск есть-ли такой клиент
    ClientMapType::iterator p;
    for(p=m_clients.begin(); p!=m_clients.end(); p++){
        if( (*p)->netidPlayer == _netid) return 0;
    }

    int idxPlayerData=hostMissionDescription->findPlayer(_netid);
    if(idxPlayerData!=-1){
        PlayerData& pd = hostMissionDescription->playersData[idxPlayerData];
        PClientData* pPCD=new PClientData(pd.name(), _netid);
        m_clients.push_back(pPCD);
        //m_pConnection->AddPlayerToGroup(m_netidGroupGame, netid);
        pPCD->curLastQuant=_curLastQuant;//m_clientNumberGameQuant;
        pPCD->confirmQuant=_confirmQuant;
        pd.flag_playerStartReady = true;

        hostMissionDescription->setChanged();

        LogMsg("ReJoin client NID 0x%" PRIX64 " PID %d for game %s\n", _netid, idxPlayerData, m_GameName.c_str());

        return 1;
    }
    else {
        LogMsg("Client NID 0x%" PRIX64 " for game %s id denied\n", _netid, m_GameName.c_str());
        return 0;
    }
}

bool PNetCenter::PutInputPacket2NetBuffer(InOutNetComBuffer& netBuf, NETID& returnNETID)
{
    if(flag_LockIputPacket) return 0;

    int cnt=0;
    std::list<NetConnectionMessage*>::iterator p=m_InputPacketList.begin();
    if(p != m_InputPacketList.end()){
        returnNETID=(*p)->source;
        while(p != m_InputPacketList.end()){
            NetConnectionMessage* packet = *p;
            if(returnNETID==packet->source){
                if(netBuf.putBufferPacket(packet->address(), packet->tell())) {
                    delete packet;
                    p=m_InputPacketList.erase(p);
                    cnt++;
                }
                else break;
            }
            else p++;
        }
    }
    return (cnt!=0);
}

void PNetCenter::HostReceiveQuant()
{
    if(flag_LockIputPacket) return; //return 0;

    NETID netid=m_localNETID;
    do { //Первый проход для внутреннего клиента


        //NETCOM_4H_ID_CHANGE_REAL_PLAYER_TYPE
        //netCommand4H_ChangeRealPlayerType

        ///ClientMapType::iterator i;
        ///FOR_EACH(m_clients, i)
        ///	i->second->NetHandlerProc(netid);


        bool belongs_to_player = false;
        for (PClientData* pcd : m_clients) {
            //if(in_buffer.receive(*m_pConnection, netid))
            /////////////in_HostBuf.receive(*m_pConnection, netid);
            if(netid==pcd->netidPlayer) {
                hostProcessPlayerClientPackets(pcd);
                belongs_to_player = true;
                break; //for-a
            }
        }
        if (!belongs_to_player) { //Ни один существующий клиент не соответствует NETID
            while(int currentCMD = in_HostBuf.currentNetCommandID()) {
                /*
                if(currentCMD==NETCOM_4H_ID_REJOIN_REQUEST && m_state==PNC_STATE__NEWHOST_PHASE_A){
                    netCommand4H_ReJoinRequest nc(in_HostBuf);
                    AddClientToMigratedHost(netid, nc.currentLastQuant, nc.confirmedQuant);
                }
                else if(currentCMD==NETCOM_4H_ID_RESPONCE_LAST_QUANTS_COMMANDS){
                    netCommand4H_ResponceLastQuantsCommands nci(in_HostBuf);
                } else
                */
                {
                    fprintf(stderr, "Invalid netCommand %d to host from non-client NETID 0x%" PRIX64 "\n", currentCMD, netid);
                    in_HostBuf.ignoreNetCommand();
                }

                in_HostBuf.nextNetCommand();//Завершение обработки комманды
            }
        }

    } while (PutInputPacket2NetBuffer(in_HostBuf, netid)!=0);
}

void PNetCenter::hostProcessPlayerClientPackets(PClientData* client) {
    NETID netid = client->netidPlayer;
    while (int currentCMD = in_HostBuf.currentNetCommandID()) {
        netLog <= currentCMD < "\n";
        switch (currentCMD) {
            case NETCOM_4H_ID_REQUEST_PAUSE:
            {
                netCommand4H_RequestPause nc_rp(in_HostBuf);
                client->requestPause=nc_rp.pause;
                client->timeRequestPause=clocki();

/*							if( (!client->requestPause) && nc_rp.pause){
								int playersIDArr[NETWORK_PLAYERS_MAX];
								for(int i=0; i<NETWORK_PLAYERS_MAX; i++) playersIDArr[i]=netCommand4C_Pause::NOT_PLAYER_ID;
								playersIDArr[0]=nc_rp.playerID;
								netCommand4C_Pause ncp(playersIDArr, true);
								SendEvent(ncp, NETID_ALL_PLAYERS_GROUP);
								client->requestPause=true;
							}
							else if(client->requestPause && (!nc_rp.pause) ){
								int playersIDArr[NETWORK_PLAYERS_MAX];
								for(int i=0; i<NETWORK_PLAYERS_MAX; i++) playersIDArr[i]=netCommand4C_Pause::NOT_PLAYER_ID;
								netCommand4C_Pause ncp(playersIDArr, false);
								SendEvent(ncp, NETID_ALL_PLAYERS_GROUP);
								client->requestPause=false;
							}*/

            }
                break;
            case NETCOM_4H_ID_CHANGE_REAL_PLAYER_TYPE:
            {
                netCommand4H_ChangeRealPlayerType ncChRT(in_HostBuf);
                if (m_state!=PNC_STATE__HOST_TUNING_GAME) break;

                if(netid==m_hostNETID){
                    bool isSave = isSaveGame();
                    MissionDescription& mission = *hostMissionDescription;
                    mission.setChanged();
                    xassert(ncChRT.idxPlayerData_ < mission.playersData.size());
                    //Проверка на то, что меняется не у Host-а / check that we are not changing host
                    if (ncChRT.idxPlayerData_!=mission.findPlayer(m_hostNETID)) {
                        PlayerData& pd = mission.playersData[ncChRT.idxPlayerData_];
                        //Eject player
                        if (pd.realPlayerType==REAL_PLAYER_TYPE_PLAYER) {
                            //Отбрасывание игрока
                            NETID delPlayerNETID=pd.netid;
                            mission.disconnect2PlayerData(ncChRT.idxPlayerData_);
                            if (isSave) {
                                if (ncChRT.newRealPlayerType_ == REAL_PLAYER_TYPE_OPEN || ncChRT.newRealPlayerType_ == REAL_PLAYER_TYPE_PLAYER_AI) {
                                    pd.realPlayerType = ncChRT.newRealPlayerType_;
                                }
                            } else {
                                if (ncChRT.newRealPlayerType_ == REAL_PLAYER_TYPE_OPEN || ncChRT.newRealPlayerType_ == REAL_PLAYER_TYPE_CLOSE) {
                                    pd.realPlayerType = ncChRT.newRealPlayerType_;
                                }
                            }
                            RemovePlayer(delPlayerNETID);
                        } else if (pd.realPlayerType==REAL_PLAYER_TYPE_PLAYER_AI) {
                            if (ncChRT.newRealPlayerType_ == REAL_PLAYER_TYPE_OPEN) {
                                mission.disconnect2PlayerData(ncChRT.idxPlayerData_);
                                pd.realPlayerType=ncChRT.newRealPlayerType_;
                            }
                        }

                        if (isSave) {
                            if (pd.realPlayerType==REAL_PLAYER_TYPE_OPEN && ncChRT.newRealPlayerType_==REAL_PLAYER_TYPE_PLAYER_AI) {
                                pd.realPlayerType = ncChRT.newRealPlayerType_;
                            }
                        } else {
                            if(ncChRT.newRealPlayerType_==REAL_PLAYER_TYPE_PLAYER) {
                                ncChRT.newRealPlayerType_ = REAL_PLAYER_TYPE_OPEN;
                            }
                            if (pd.realPlayerType == REAL_PLAYER_TYPE_AI) { //Если был AI
                                if (ncChRT.newRealPlayerType_ == REAL_PLAYER_TYPE_OPEN
                                    || ncChRT.newRealPlayerType_ == REAL_PLAYER_TYPE_CLOSE) {
                                    //Закрывать AI
                                    mission.disconnect2PlayerData(ncChRT.idxPlayerData_);
                                    pd.realPlayerType=ncChRT.newRealPlayerType_;
                                }
                            } else { //Если был Close Или Open
                                if (ncChRT.newRealPlayerType_ == REAL_PLAYER_TYPE_AI) {
                                    mission.connectAI2PlayersData(ncChRT.idxPlayerData_);
                                } else if (ncChRT.newRealPlayerType_ == REAL_PLAYER_TYPE_OPEN ||
                                           ncChRT.newRealPlayerType_ == REAL_PLAYER_TYPE_CLOSE) {
                                    pd.realPlayerType = ncChRT.newRealPlayerType_;
                                }
                            }
                        }
                    }
                }
            }
                break;
            case NETCOM_4H_ID_CHANGE_PLAYER_BELLIGERENT:
            {
                netCommand4H_ChangePlayerBelligerent  ncChB(in_HostBuf);
                if (m_state!=PNC_STATE__HOST_TUNING_GAME || isSaveGame()) break;

                hostMissionDescription->setChanged();
                xassert(ncChB.idxPlayerData_ < hostMissionDescription->playersData.size());
                //Host может менять у любого
                int playerIndex = netid == m_hostNETID ? ncChB.idxPlayerData_ : hostMissionDescription->findPlayer(netid);
                hostMissionDescription->changePlayerBelligerent(playerIndex, ncChB.newBelligerent_);
            }
                break;
            case NETCOM_4H_ID_CHANGE_PLAYER_COLOR:
            {
                netCommand4H_ChangePlayerColor  ncChC(in_HostBuf);
                if (m_state!=PNC_STATE__HOST_TUNING_GAME || isSaveGame()) break;

                hostMissionDescription->setChanged();
                xassert(ncChC.idxPlayerData_ < hostMissionDescription->playersData.size());
                //Host может менять цвет любого
                int playerIndex = netid == m_hostNETID ? ncChC.idxPlayerData_ : hostMissionDescription->findPlayer(netid);
                hostMissionDescription->changePlayerColor(playerIndex, ncChC.newColor_, ncChC.direction);
            }
                break;
            case NETCOM_4H_ID_CHANGE_PLAYER_DIFFICULTY:
            {
                netCommand4H_ChangePlayerDifficulty ncChD(in_HostBuf);
                if (m_state!=PNC_STATE__HOST_TUNING_GAME || isSaveGame()) break;

                hostMissionDescription->setChanged();
                xassert(ncChD.idxPlayerData_ < hostMissionDescription->playersData.size());
                //Host может менять у любого
                int playerIndex = netid == m_hostNETID ? ncChD.idxPlayerData_ : hostMissionDescription->findPlayer(netid);
                hostMissionDescription->changePlayerDifficulty(playerIndex, ncChD.difficulty_);
            }
                break;
            case NETCOM_4H_ID_CHANGE_PLAYER_CLAN:
            {
                netCommand4H_ChangePlayerClan ncChC(in_HostBuf);
                if (m_state!=PNC_STATE__HOST_TUNING_GAME || isSaveGame()) break;

                hostMissionDescription->setChanged();
                xassert(ncChC.idxPlayerData_ < hostMissionDescription->playersData.size());
                //Host может менять у любого
                int playerIndex = netid == m_hostNETID ? ncChC.idxPlayerData_ : hostMissionDescription->findPlayer(netid);
                hostMissionDescription->changePlayerClan(playerIndex, ncChC.clan_);
            }
                break;
            case NETCOM_4H_ID_CHANGE_PLAYER_HANDICAP:
            {
                netCommand4H_ChangePlayerHandicap ncChH(in_HostBuf);
                if (m_state!=PNC_STATE__HOST_TUNING_GAME || isSaveGame()) break;

                hostMissionDescription->setChanged();
                xassert(ncChH.idxPlayerData_ < hostMissionDescription->playersData.size());
                //Host может менять у любого
                int playerIndex = netid == m_hostNETID ? ncChH.idxPlayerData_ : hostMissionDescription->findPlayer(netid);
                hostMissionDescription->changePlayerHandicap(playerIndex, ncChH.handicap_);
            }
                break;
            case NETCOM_4H_ID_CHANGE_PLAYER_SEAT:
            {
                netCommand4H_ChangePlayerSeat ncChS(in_HostBuf);
                if (m_state!=PNC_STATE__HOST_TUNING_GAME || !isSaveGame()) break;

                MissionDescription& mission = *hostMissionDescription;
                xassert(ncChS.idxPlayerData_ < mission.playersData.size());
                //Check if player who wants to change seat is the sender
                int originIndex = mission.findPlayer(netid);
                if (originIndex != -1 && ncChS.idxPlayerData_ < mission.playersData.size()) {
                    PlayerData& origin = mission.playersData[originIndex];
                    PlayerData& destination = mission.playersData[ncChS.idxPlayerData_];
                    bool allowed = false;
                    switch (destination.realPlayerType) {
                        case REAL_PLAYER_TYPE_OPEN:
                        case REAL_PLAYER_TYPE_PLAYER:
                        case REAL_PLAYER_TYPE_PLAYER_AI:
                            allowed = true;
                            break;
                        default:
                            break;
                    }
                    if (!origin.flag_playerStartReady && !destination.flag_playerStartReady && allowed) {
                        //Swap the players
                        std::string originName = origin.name();
                        if (destination.realPlayerType == REAL_PLAYER_TYPE_PLAYER) {
                            origin.setName(destination.name());
                        } else {
                            origin.setName(origin.nameInitial());
                        }
                        destination.setName(originName);
                        std::swap(origin.netid, destination.netid);
                        std::swap(origin.realPlayerType, destination.realPlayerType);
                        mission.setChanged();
                    }
                }
            }
                break;
            case NETCOM_4H_ID_CHANGE_MAP:
            {
                netCommand4H_ChangeMap nc_changeMap(in_HostBuf);
                if (m_state!=PNC_STATE__HOST_TUNING_GAME || isSaveGame()) break;

                hostMissionDescription->setChanged();
                if(netid==m_hostNETID){//только Host может менять карту
                    int i;
                    for(i=0; i< nc_changeMap.missionDescription_.playerAmountScenarioMax; i++){
                        if (i < hostMissionDescription->playerAmountScenarioMax) {
                            nc_changeMap.missionDescription_.playersData[i] = hostMissionDescription->playersData[i];
                        } else {
                            //Don't copy from previous description since this slot wasnt used and may contain unknown data
                            nc_changeMap.missionDescription_.playersData[i].realPlayerType = REAL_PLAYER_TYPE_OPEN;
                        }
                    }
                    //Remove players that exceed scenario max
                    for(i; i< hostMissionDescription->playersData.size(); i++){
                        if(hostMissionDescription->playersData[i].realPlayerType==REAL_PLAYER_TYPE_PLAYER){
                            NETID delPlayerNETID=hostMissionDescription->playersData[i].netid;
                            hostMissionDescription->disconnect2PlayerData(i);
                            //hostMissionDescription->playersData[i].realPlayerType=REAL_PLAYER_TYPE_CLOSE;
                            RemovePlayer(delPlayerNETID); //Полное удаление по DPN_MSGID_DESTROY_PLAYER
                        }
                    }
                    *hostMissionDescription = nc_changeMap.missionDescription_;
                }
            }
                break;
            case NETCOM_4H_ID_START_LOAD_GAME:
            {
                netCommand4H_StartLoadGame ncslg(in_HostBuf);
                if(m_state!=PNC_STATE__HOST_TUNING_GAME) break;

                hostMissionDescription->setPlayerStartReady(netid, ncslg.ready != 0);
            }
                break;
            case NETCOMC_ID_PLAYER_READY:
            {
                hostMissionDescription->setChanged();

                netCommandC_PlayerReady event(in_HostBuf);
                client->m_flag_Ready=true;
                client->clientGameCRC=event.gameCRC_;

                LogMsg("Player 0x%" PRIX64 " (GCRC=0x%" PRIX32 ") reported ready\n", netid, client->clientGameCRC);

                //Flag the client as restore finished if was desynced
                for (auto& client : m_clients) {
                    if (client->netidPlayer == netid
                        && client->desync_state != PNC_DESYNC_NONE
                        && client->desync_state != PNC_DESYNC_RESTORE_FINISHED) {
                        xassert(client->desync_state == PNC_DESYNC_SENT_RESTORE);
                        if (client->netidPlayer == m_hostNETID) {
                            //Host always restores OK
                            client->desync_state = PNC_DESYNC_NONE;
                        } else {
                            client->desync_state = PNC_DESYNC_RESTORE_FINISHED;
                        }
                        break;
                    }
                }
            }
                break;
            case NETCOM_4H_ID_DESYNC_ACKNOWLEDGE:
            {
                hostMissionDescription->setChanged();

                netCommand4H_DesyncAcknowledge event(in_HostBuf);

                LogMsg("Desync Ack 0x%" PRIX64 "\n", netid);

                //Flag the client as acknowledged
                for (auto& cl : m_clients) {
                    if (cl->netidPlayer == netid) {
                        xassert(cl->desync_state == PNC_DESYNC_NOTIFIED);
                        cl->desync_state = PNC_DESYNC_ACKNOLEDGED;
                        cl->desync_missionDescription = std::move(event.missionDescription);
                        std::swap(cl->desync_netlog, event.netlog);
                        break;
                    }
                }

            }
                break;

            case NETCOM_4G_ID_UNIT_COMMAND:
            {
                netCommand4G_UnitCommand* pCommand = new netCommand4G_UnitCommand(in_HostBuf);
                PutGameCommand2Queue_andAutoDelete(netid, pCommand);
            }
                break;
            case NETCOM_4G_ID_REGION:
            {
                netCommand4G_Region* pCommand=new netCommand4G_Region(in_HostBuf);
                PutGameCommand2Queue_andAutoDelete(netid, pCommand);
            }
                break;
            case NETCOM_4H_ID_BACK_GAME_INFORMATION:
            {
                netCommand4H_BackGameInformation* pEvent= new netCommand4H_BackGameInformation(in_HostBuf);
                //p->second->backGameInfList.push_back(pEvent);
                delete pEvent;
            }
                break;
            case NETCOM_4H_ID_BACK_GAME_INFORMATION_2:
            {
                netCommand4H_BackGameInformation2 nc(in_HostBuf);
                client->backGameInf2List.push_back(nc);
                client->lagQuant=nc.lagQuant_;
                client->lastExecuteQuant=nc.quant_;
                client->lastTimeBackPacket=clocki();
            }
                break;
            case NETCOM_4H_ID_LATENCY_RESPONSE:
            {
                netCommand4H_LatencyResponse nc(in_HostBuf);
                size_t timestamp = clock_us();
                if (timestamp > nc.timestamp) {
                    client->last_time_latency_response = timestamp;
                    client->latency = timestamp - nc.timestamp;
                }
            }
                break;
            case NETCOM_4H_ID_RESPONCE_LAST_QUANTS_COMMANDS:
            {
                xassert(m_state==PNC_STATE__NEWHOST_PHASE_B);
                netCommand4H_ResponceLastQuantsCommands nci(in_HostBuf);
                if(m_state!=PNC_STATE__NEWHOST_PHASE_B) break;

                std::vector<netCommandGame*> tmpListGameCommands;

                InOutNetComBuffer in_buffer(nci.sizeCommandBuf, 1); //проверить необходимость автоувелечения!
                in_buffer.putBufferPacket(nci.pData, nci.sizeCommandBuf);

                while(in_buffer.currentNetCommandID()!=NETCOM_ID_NONE) {
                    terEventID event = (terEventID)in_buffer.currentNetCommandID();
                    switch(event){
                        case NETCOM_4G_ID_UNIT_COMMAND: {
                            netCommand4G_UnitCommand*  pnc= new netCommand4G_UnitCommand(in_buffer);
                            tmpListGameCommands.push_back(pnc);
                            break;
                        }
                        case NETCOM_4G_ID_REGION: {
                            netCommand4G_Region*  pnc= new netCommand4G_Region(in_buffer);
                            tmpListGameCommands.push_back(pnc);
                            break;
                        }
                        case NETCOM_4G_ID_FORCED_DEFEAT: {
                            netCommand4G_ForcedDefeat* pnc=new netCommand4G_ForcedDefeat(in_buffer);
                            tmpListGameCommands.push_back(pnc);
                            break;
                        }
                        default:
                            xassert(0&&"Incorrect commanf in playReel file!");
                            break;
                    }
                    in_buffer.nextNetCommand();
                }
                //Запуск продолжения
                netCommand4C_ContinueGameAfterHostMigrate ncContinueGame;
                SendEvent(ncContinueGame, NETID_ALL);
                //Выполнение команд, которые не у всех были выполнены
                for(m_numberGameQuant=nci.beginQuantCommandTransmit; m_numberGameQuant<=nci.endQuantCommandTransmit; m_numberGameQuant++){
                    m_nQuantCommandCounter=0;
                    std::vector<netCommandGame*>::iterator p;
                    for(p=tmpListGameCommands.begin(); p!=tmpListGameCommands.end(); p++){
                        if((*p)->curCommandQuant_==m_numberGameQuant) {
                            SendEvent(**p, NETID_ALL);
                            m_nQuantCommandCounter++;
                        }
                    }
                    //netCommandNextQuant netCommandNextQuant(m_numberGameQuant, m_nQuantCommandCounter, netCommandNextQuant::NOT_QUANT_CONFIRMATION);
                    //SendEvent(netCommandNextQuant, NETID_ALL_PLAYERS_GROUP/*m_netidGroupGame*/);

                }
                hostGeneralCommandCounter=nci.finGeneraCommandCounter;
                netCommandNextQuant netCommandNextQuant(nci.endQuantCommandTransmit, m_nQuantCommandCounter, hostGeneralCommandCounter, netCommandNextQuant::NOT_QUANT_CONFIRMATION);
                SendEvent(netCommandNextQuant, NETID_ALL);

                terHyperSpace::clearListGameCommands(tmpListGameCommands);

                //Init game counter afte MigrateHost
                //hostGeneralCommandCounter; //уже уставится выше
                quantConfirmation=netCommandNextQuant::NOT_QUANT_CONFIRMATION;
                m_nQuantCommandCounter=0;
                m_numberGameQuant=nci.endQuantCommandTransmit+1;//!выше
                ///ClearDeletePlayerGameCommand();
                ClearCommandList();
                //отмена паузы если была
                hostPause=0;
                int playersIDArr[NETWORK_PLAYERS_MAX];
                for(int m=0; m<NETWORK_PLAYERS_MAX; m++) playersIDArr[m]=netCommand4C_Pause::NOT_PLAYER_ID;
                netCommand4C_Pause ncp(playersIDArr, false);
                SendEvent(ncp, NETID_ALL);

                m_state=PNC_STATE__HOST_GAME;
            }
                break;
            case NETCOM_4G_ID_CHAT_MESSAGE:
            {
                netCommand4G_ChatMessage nc_ChatMessage(in_HostBuf);

                //Get player data that send this
                int playerID = hostMissionDescription->findPlayer(netid);
                if (playerID != -1) {
                    nc_ChatMessage.playerID = playerID;
                    const auto& playerData = hostMissionDescription->playersData[playerID];
                    const auto& pc = playerColors[playerData.colorIndex].unitColor;

                    //Add color and name of player to text
                    std::string text = "&"
                                       + toColorCode(sColor4f(pc[0], pc[1], pc[2], 1.0f))
                                       + playerData.name()
                                       + "&FFFFFF: "
                                       + nc_ChatMessage.text;
                    nc_ChatMessage.text = text;

                    //Find if we need to send to sender clan or everyone (sender included)
                    if (nc_ChatMessage.clanOnly) {
                        for (const auto& missionPlayer : hostMissionDescription->playersData) {
                            if (missionPlayer.clan == playerData.clan && missionPlayer.netid) {
                                SendEvent(nc_ChatMessage, missionPlayer.netid);
                            }
                        }
                    } else {
                        SendEvent(nc_ChatMessage, NETID_ALL);
                    }
                }
            }
                break;
            case EVENT_ID_SERVER_TIME_CONTROL:
            {
                terEventControlServerTime event(in_HostBuf);

                ///					m_pGame->SetGameSpeedScale(event.scale, netidPlayer);
            }
                break;
            case NETCOM_4G_ID_EXIT:
            {
                netCommand4G_Exit event(in_HostBuf);
                xassert(event.netid == netid);
                if (event.netid == netid) {
                    SendEvent(event, NETID_ALL);
                    ExitClient(netid);
                }
            }
                break;
            case NETCOM_4H_ID_REJOIN_REQUEST:
            {
                //Опустошение лишней команды(сейчас клиент продолжает посылать ее до NEXT_QUANT)
                netCommand4H_ReJoinRequest nc(in_HostBuf);
            }
                break;
            default:
            {
                xassert("Invalid netCommand to host.");
                in_HostBuf.ignoreNetCommand();
            }
                break;
        }
        //Запрещается вызывать currentNetCommandID(т.к. используется ignoreNetCommand)
        in_HostBuf.nextNetCommand();
    }
}

const NetRelayMessage_PeerSetupRoom& PNetCenter::GenerateRelaySetupRoom() const {
    const MissionDescription* mission = hostMissionDescription;
    const std::string& locale = getLocale();
    static NetRelayMessage_PeerSetupRoom msg;
    msg.roomName = convertToUnicode(m_GameName, locale);
    msg.hasPasword = !gamePassword.empty();
    msg.playersCount = mission->playersAmount();
    msg.playersMax = mission->playerSlotsAvailable();
    msg.isClosed = false; //We handle the connections ourselves, this way we can tell why we are rejecting them
    msg.gameStarted = m_bStarted;
    msg.scenarioName = mission->worldName();
    msg.gameContent = terGameContentSelect;
    msg.locale = locale;
    return msg;
}
