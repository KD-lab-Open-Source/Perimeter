#include "NetIncludes.h"

#include "Runtime.h"
#include "P2P_interface.h"
#include "ServerList.h"
#include "NetConnectionAux.h"

#include "Universe.h"

#include <algorithm>
#include <set>
#include <SDL.h>
#include "files/files.h"
#include "../HT/ht.h"

#ifdef _WIN32
#include <combaseapi.h>
#endif

const int PNC_MIN_SLEEP_TIME = 10; //millis
const unsigned int MAX_TIME_WAIT_RESTORE_GAME_AFTER_MIGRATE_HOST=10000;//10sec
const int PNC_DESYNC_RESTORE_ATTEMPTS = 3;
const int PNC_DESYNC_RESTORE_MODE_PARTIAL = 0; //2; TODO set back once partial load is finished 
const int PNC_DESYNC_RESTORE_ATTEMPTS_TIME = 4 * 60 * 1000; //4 mins
const int PNC_DESYNC_RESTORE_MODE_FULL = PNC_DESYNC_RESTORE_MODE_PARTIAL + 1;
const size_t PNC_LATENCY_UPDATE_INTERVAL = 50 * 1000; //us

void PNetCenter::clearInOutClientHostBuffers()
{
	in_ClientBuf.reset();
	out_ClientBuf.reset();
    in_ClientBuf.reset_stats();
    out_ClientBuf.reset_stats();

	in_HostBuf.reset();
	out_HostBuf.reset();
    in_HostBuf.reset_stats();
    out_HostBuf.reset_stats();
}

//TestThread
/*int flaaaag=0;
uint32_t WINAPI TestServerThread(void* lpParameter)
{
	PNetCenter* pPNetCenter=(PNetCenter*)lpParameter;
	while(1){
		if(flaaaag){
			if(pPNetCenter->m_state&PNC_State_GameRun){
				netCommand4G_ForcedDefeat nc(4);
				pPNetCenter->SendEvent(&nc);
				if(flaaaag==1)
					Sleep(30);
				else
					Sleep(100);
			}
		}
		else Sleep(1000);
	}
}
*/

extern std::atomic_uint64_t net_thread_id;

//Second thread
PNetCenter* netCenter = nullptr;
int InternalServerThread(void* lpParameter)
{
/*	// TEST thread!!!!
	DWORD ThreadId;
	HANDLE  hTestThread=CreateThread( NULL, 0, TestServerThread, lpParameter, 0, &ThreadId);
*/

    xassert(netCenter == nullptr);
    net_thread_id = SDL_ThreadID();

	PNetCenter* pPNetCenter=(PNetCenter*)lpParameter;
    pPNetCenter->SecondThreadInit();

    if (HTManager::instance()->IsUseHT()) {
        while (pPNetCenter->SecondThreadLive()) {
            pPNetCenter->SecondThreadQuant();
        }
        pPNetCenter->SecondThreadDeinit();
    } else {
        netCenter = pPNetCenter;
    }

	return 0;
}

extern void PNetCenterNetQuant() {
    if (netCenter) {
        if (netCenter->SecondThreadLive()) {
            netCenter->SecondThreadQuant();
        } else {
            netCenter = nullptr;
            netCenter->SecondThreadDeinit();
        }
    }
}

XBuffer BUFFER_LOG(10000, true);

void PNetCenter::SecondThreadInit() {
    xassert(net_thread_id == SDL_ThreadID());

#ifdef _WIN32
    //TODO is this necessary?
    CoInitializeEx(0, COINIT_MULTITHREADED);
#endif

    Init();

    m_state=PNC_STATE__CLIENT_FIND_HOST;
    serverList->startFind();

    //Инициализация завершена - XDPConnection создан
    SetEvent(hSecondThreadInitComplete);

    flag_end = false;
}

bool PNetCenter::SecondThreadLive() {
    return !flag_end;
}

void PNetCenter::SecondThreadQuant()
{
    xassert(net_thread_id == SDL_ThreadID());
    CAutoLock* _pLock=new CAutoLock(m_GeneralLock);
    //decoding command
    if(!internalCommandList.empty()){
        e_PNCInternalCommand curInternalCommand;
        //{ //Сейчас сделано так, что нельзя поместить команду если другая выполняется
        curInternalCommand=*internalCommandList.begin();
        internalCommandList.pop_front();
        //}
        switch(curInternalCommand){
        case PNC_COMMAND__STOP_GAME_AND_ASSIGN_HOST_2_MY:
            {
                m_state=PNC_STATE__NEWHOST_PHASE_0;
            }
            SetEvent(hCommandExecuted);
            break;
        case PNC_COMMAND__STOP_GAME_AND_WAIT_ASSIGN_OTHER_HOST:
            {
                m_state=PNC_STATE__CLIENT_RESTORE_GAME_AFTE_CHANGE_HOST_PHASE_0;
            }
            SetEvent(hCommandExecuted);
            break;

			case PNC_COMMAND__CONNECT_2_HOST_AND_STOP_FIND_HOST:
            case PNC_COMMAND__CONNECT_2_RELAY_ROOM_AND_STOP_FIND_HOST:
				{
					flag_LockIputPacket=0;
					flag_SkipProcessingGameCommand=0;
					ClearInputPacketList();
					clearInOutClientHostBuffers();
					m_bStarted = false;
                    m_hostNETID = m_localNETID = NETID_NONE;
                    
                    m_state = PNC_STATE__CLIENT_TUNING_GAME;
                    
                    NetConnection* connection = nullptr;
                    if (curInternalCommand == PNC_COMMAND__CONNECT_2_RELAY_ROOM_AND_STOP_FIND_HOST) {
                        LogMsg("SendClientHandshake Room to: %s room 0x%" PRIX64 "\n", hostConnection.getString().c_str(), clientRoom);
                        connection = connectionHandler.startRelayRoomConnection(hostConnection, clientRoom);
                    } else {
                        LogMsg("SendClientHandshake Direct to: %s\n", hostConnection.getString().c_str());
                        connection = connectionHandler.startDirectConnection(hostConnection);
                        m_hostNETID = NETID_HOST;
                        m_localNETID = NETID_NONE;
                    }
                    flag_connected = SendClientHandshake(connection);
                    
                    if (!isConnected()) {
                        m_state=PNC_STATE__RESETTING;
                        ExecuteInternalCommand(PNC_COMMAND__RESET, false);
                    }
                    SetEvent(hCommandExecuted);
                    break;
				}
				break;
			case PNC_COMMAND__START_HOST_AND_CREATE_GAME_AND_STOP_FIND_HOST:
            case PNC_COMMAND__CONNECT_RELAY_AND_CREATE_GAME_AND_STOP_FIND_HOST:
				{
					flag_LockIputPacket=0;
					flag_SkipProcessingGameCommand=0;
					ClearInputPacketList();
                    clearInOutClientHostBuffers();
                    m_bStarted = false;
                    m_hostNETID = m_localNETID = NETID_NONE;
                    
                    //Set here so the isHost() gives true
                    m_state=PNC_STATE__HOST_TUNING_GAME;

                if (isConnected()) {
                    connectionHandler.reset();
                }

                LogMsg("New game <%s> for start...\n", m_GameName.c_str());

                LogMsg("Starting server...\n");
                bool isPublic = curInternalCommand == PNC_COMMAND__CONNECT_RELAY_AND_CREATE_GAME_AND_STOP_FIND_HOST;
                flag_connected = connectionHandler.startHost(hostConnection.port(), isPublic);

                if (!isConnected()) {
                    fprintf(stderr, "...error!\n");
                    m_state=PNC_STATE__RESETTING;
                    ExecuteInternalCommand(PNC_COMMAND__RESET, false);
                } else {
                    if (!isPublic) {
                        //If not public we assign NETIDs ourselves
                        m_hostNETID = m_localNETID = NETID_HOST;
                    }
                    serverList->stopFind();
                    LogMsg("...started OK\n");

                    ClearClients();
                    PlayerData pd;
                    pd.set(m_PlayerName, m_localNETID);
                    if (AddClient(pd) == -1) {
                        if (hostMissionDescription->gameType_ == GT_MULTI_PLAYER_LOAD) {
                            //Try forcing first player as open to allocate host
                            if (0 < hostMissionDescription->playersAmountScenarioMax()) {
                                hostMissionDescription->playersData[0].realPlayerType = REAL_PLAYER_TYPE_OPEN;
                            }
                            if (AddClient(pd) == -1) {
                                ErrH.Abort("Network: Couldnt add host player to saved scenario");
                            }
                        } else {
                            ErrH.Abort("Network: Couldnt add host player to scenario");
                        }
                    }

                    hostMissionDescription->clearAllPlayerGameReady();
                    hostMissionDescription->setChanged();
                }
            }
            SetEvent(hCommandExecuted);
            break;
        case PNC_COMMAND__DESYNC:
            {
                flag_SkipProcessingGameCommand = true;
                if (isHost()) {
                    m_state=PNC_STATE__HOST_DESYNC;
                } else {
                    m_state=PNC_STATE__CLIENT_DESYNC;
                }
            }
            SetEvent(hCommandExecuted);
            break;
        case PNC_COMMAND__RESET:
            {
                LogMsg("PNC_COMMAND__RESET\n");
                m_state = PNC_STATE__RESETTING;
                CAutoLock _lock(m_GeneralLock); //! Lock
                bool is_host = isHost();
                if (isConnected()
                    && m_localNETID != NETID_NONE && m_hostNETID != NETID_NONE
                    && (is_host || connectionHandler.hasClient(m_hostNETID, true))) {

                    //Guaranteed sending of the last quant
                    if (m_bStarted && is_host) {
                        netCommandNextQuant com(m_numberGameQuant, 0, hostGeneralCommandCounter, 0);
                        SendEvent(com, NETID_ALL);
                    }

                    //Send exit event to everyone connected (host or clients)
                    netCommand4G_Exit ex(m_localNETID);
                    SendEvent(ex, NETID_ALL);
                    LogMsg("Sent Exit packets to NETID_ALL\n");
                }

                //Start shutdown
                bool was_started = m_bStarted;
                connectionHandler.reset();
                flag_connected = false;
                m_bStarted = false;
                ClearClients();

                //Stay closed if game was active as we don't want to find host yet
                if (was_started) {
                    LogMsg("State: PNC_STATE__CLOSED\n");
                    m_state = PNC_STATE__CLOSED;
                } else {
                    LogMsg("State: PNC_STATE__CLIENT_FIND_HOST\n");
                    m_state = PNC_STATE__CLIENT_FIND_HOST;
                    serverList->startFind();
                }
            }
            SetEvent(hCommandExecuted);
            break;
        case PNC_COMMAND__END:
            {
                flag_end=true;
                //m_state=PNC_STATE__END;
            }
            SetEvent(hCommandExecuted);
            break;
        case PNC_COMMAND__ABORT_PROGRAM:
            {
                m_state=PNC_STATE__NET_CENTER_CRITICAL_ERROR;
                ///ExecuteInterfaceCommand(PNC_INTERFACE_COMMAND_CRITICAL_ERROR_GAME_TERMINATED); //????????
            }
            SetEvent(hCommandExecuted);
            break;
        case PNC_COMMAND__CLIENT_STARTING_LOAD_GAME: {
            m_state=PNC_STATE__CLIENT_LOADING_GAME;
            SetEvent(hCommandExecuted);
            break;
        }
        case PNC_COMMAND__CLIENT_GAME_IS_READY: {
            m_state=PNC_STATE__CLIENT_GAME;

            //Set this off to return to usual client business after a desync
            flag_SkipProcessingGameCommand = false;

            SetEvent(hCommandExecuted);
            break;
        }
        default:
            SetEvent(hCommandExecuted);
            break;
        }
    }

    if (flag_connected) {
        //Check connection states
        connectionHandler.pollConnections();

        //Host/Client quant
        if (isHost()) {
            connectionHandler.acceptConnection();
            HostReceiveQuant();
        } else {
            ClientPredReceiveQuant();
        }
    }

    delete _pLock;

    if(flag_end) return; //для быстрого выхода

    //Logic quant

    int curTime=clocki();
    int minWakingTime=curTime+m_quantInterval;//NORMAL_QUANT_INTERVAL;

    if(m_nextQuantTime==0) m_nextQuantTime=curTime; //tracking start game
    if(m_nextQuantTime<=curTime) {
        m_nextQuantTime+=m_quantInterval;//m_quantPeriod;
        if(minWakingTime > m_nextQuantTime) minWakingTime=m_nextQuantTime;
        LLogicQuant();
    }


    curTime=clocki();
    uint32_t sleepTime = minWakingTime > curTime ? minWakingTime - curTime : 0;
    if (0 < sleepTime) {
        Sleep(min(sleepTime, PNC_MIN_SLEEP_TIME));
    }
    //end logic quant
}

void PNetCenter::SecondThreadDeinit() {
    if (net_thread_id != -1) {
        serverList->stopFind();

        SetConnectionTimeout(1);//Для быстрого завершения
        //if(m_pConnection->Connected()) m_pConnection->Close();
        connectionHandler.reset();
        flag_connected = false;
        m_bStarted = false;

#ifdef _WIN32
        CoUninitialize();
#endif

        net_thread_id = -1;

        //We are creating it with SDL thread, so we need to manually signal it
        SetEvent(hSecondThread);
    }
}

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

void PNetCenter::SendEvent(netCommandGeneral& event, NETID destination)
{
    if (destination != m_localNETID ) {
        out_HostBuf.putNetCommand(&event);
        SendNetBuffer(&out_HostBuf, destination);
    }

    if( (destination == m_localNETID) || (destination == NETID_ALL) ){
        in_ClientBuf.putNetCommand(&event);
    }
}

void PNetCenter::ClearCommandList()
{
	std::list<netCommandGeneral*>::iterator m;
	for(m=m_CommandList.begin(); m!=m_CommandList.end(); m++)
		delete *m;
	m_CommandList.clear();
}

void PNetCenter::LLogicQuant()
{
	//LogMsg("q=%d\n", m_nQuantDelay + nDbgServerLag + int(nDbgServerLagNoise*float(rand())/RAND_MAX));

	//Не залоченный поток !
	//CAutoLock* _pLock=new CAutoLock(&m_GeneralLock);
	//delete _pLock;

    in_ClientBuf.reset_stats();
    out_ClientBuf.reset_stats();

	switch(m_state) {

	case PNC_STATE__HOST_TUNING_GAME:
		{
			CAutoLock _lock(m_GeneralLock); //! Lock

			m_numberGameQuant= 1;//!

			if(hostMissionDescription->isChanged()){
				UpdateCurrentMissionDescription4C();
				hostMissionDescription->clearChanged();
			}
			///if(m_flag_PlayerListReady)//end waite
			if(hostMissionDescription->isAllRealPlayerStartReady()){

				LogMsg("Game Started\n");

				//LockAllPlayers
				UpdateBattleData();
				//ReleaseAllPlayers
                ClearQueuedGameCommands();

                //Notify clients that we will send mission now
                netCommand4C_StartLoadGame start_signal(nullptr);
                SendEvent(start_signal, NETID_ALL);

				LogMsg("Wait for all clients to receive battle info. \n");

				m_state=PNC_STATE__HOST_SENDING_GAME;
			}
		}
		break;
    case PNC_STATE__HOST_SENDING_GAME:
        {
            CAutoLock _lock(m_GeneralLock); //! Lock

            LogMsg("Sending battle info\n");

            SendBattleData();
            ClearQueuedGameCommands();

            LogMsg("Wait for all clients ready. \n");

            m_state=PNC_STATE__HOST_LOADING_GAME;
        }
        break;
	case PNC_STATE__HOST_LOADING_GAME: 
		{
			CAutoLock _lock(m_GeneralLock); //! Lock

			bool flag_ready = true;
			ClientMapType::iterator i;
			for (i=m_clients.begin(); i!=m_clients.end(); i++) {
                PClientData* client = *i;
				flag_ready &= client->m_flag_Ready;
				if (flag_ready) {
                    if ((*m_clients.begin())->clientGameCRC != (*i)->clientGameCRC) {
                        XBuffer buf;
                        buf < "Game of the player " <= (*i)->netidPlayer < "does not match game of the player " <=
                        (*m_clients.begin())->netidPlayer;
                        xxassert(0, buf.buf);
                    }
                }
			}
			if (flag_ready) {
				CheckClients();
				DumpClients();
                UpdateCurrentMissionOnRelayRoom();

				///terEventBeginCommand ev_begin(m_nQuantDelay/100, 0);
				///SendEvent(ev_begin, m_netidGroupGame);

				///netCommandStartGame event;
				///SendEvent(event, NETID_ALL_PLAYERS_GROUP/*m_netidGroupGame*/);
				LogMsg("Go! go! go!\n");

				ResetAllClients();

				m_state=PNC_STATE__HOST_GAME;

				//Init game counter
				ClearCommandList();

				hostGeneralCommandCounter=0;
				quantConfirmation=netCommandNextQuant::NOT_QUANT_CONFIRMATION;
				m_nQuantCommandCounter=0;
				m_numberGameQuant= 1;//!
				hostPause=0;

			}
		}
		break;
	case PNC_STATE__HOST_GAME: 
		{
			CAutoLock _lock(m_GeneralLock); //! Lock
			
            //Process latency/pings
            if (last_latency_status + PNC_LATENCY_UPDATE_INTERVAL < clock_us()) {
                size_t timestamp = clock_us();
                //Send current latency info
                last_latency_status = timestamp;
                size_t max_quant = 0;
                netCommand4C_LatencyStatus ev;
                ev.info.timestamp = timestamp;
                for (auto client : m_clients) {
                    int idx=hostMissionDescription->findPlayer(client->netidPlayer);
                    if (idx < 0) continue;
                    //Client game might be in background, update latency to avoid kicking
                    if (client->clientPause) {
                        client->last_time_latency_response = timestamp;
                    }
                    ev.info.player_ids.push_back(idx);
                    ev.info.player_pings.push_back(client->latency);
                    ev.info.player_last_seen.push_back(client->last_time_latency_response);
					size_t quant = 0;
					auto& blist = client->backGameInf2List;
					if (!blist.empty()) {
						quant = blist.front().quant_;
					}
                    ev.info.player_quants.push_back(quant);
                    if (max_quant < quant) {
                        max_quant = quant;
                    }
					if (client->netidPlayer == m_localNETID) {
						ev.info.quant = quant;
					}
                }
                //Sometimes host might be absent in backgame2list, so use the highest quant from clients
                if (ev.info.quant == 0) {
                    ev.info.quant = max_quant;
                }
                //Sometimes some clients might not have backGameInf2List too, fix them
                for (auto& quant : ev.info.player_quants) {
                    if (quant == 0) {
                        quant = max_quant;
                    }
                }
                SendEvent(ev, NETID_ALL);
            }

            //Process BackGameInfo2 packets to check sync status and if any client is behind
			bool client_desync = false;
			std::vector<netCommand4H_BackGameInformation2> & firstList=(*m_clients.begin())->backGameInf2List;
			while(!firstList.empty()) { //проверка что первый список не пустой
				unsigned int countCompare=0;
				for (auto client : m_clients) {
					std::vector<netCommand4H_BackGameInformation2> &  secondList=client->backGameInf2List;
					if(!secondList.empty()) {
                        
#if defined(PERIMETER_DEBUG) && 0
                        //This forces desync every 20 quants, don't enable unless debugging desyncs
                        if (quantConfirmation > 20) {
                            client_desync = true;
                            if (client->netidPlayer != m_localNETID) {
                                (*secondList.begin()).signature_ = 0;
                            }
                        }
#endif
						if( *firstList.begin() == *secondList.begin() ) {
                            countCompare++;//if( (**firstList.begin()).equalVData(**secondList.begin()) )
                        } else {
                            if( (*firstList.begin()).quant_ != (*secondList.begin()).quant_ ) {
								//xassert(0 && "Unmatched number quants");
								XStream f(convert_path_content("outnet.log", true), XS_OUT);
                                f < currentVersion < "\r\n";
								f.write(BUFFER_LOG.address(), BUFFER_LOG.tell());
								f < "Unmatched number quants !" < "\n";
								std::vector<netCommand4H_BackGameInformation2>::iterator q;
								for(q=firstList.begin(); q!=firstList.end(); q++){
									f < "HostQuant=" <= (*q).quant_ < " " <= (*q).replay_ < " " <= (*q).state_< "\n";
								}
								for(q=secondList.begin(); q!=secondList.end(); q++){
									f < "ClientQuant=" <= (*q).quant_ < " " <= (*q).replay_ < " " <= (*q).state_ < "\n";
								}
								f.close();

                                XBuffer to(1024,true);
								to < "Unmatched number quants ! N1=" <= (*firstList.begin()).quant_ < " N2=" <=(*secondList.begin()).quant_;
                                fprintf(stderr, "Error network synchronization with 0x%" PRIX64 ": %s\n", client->netidPlayer, to.address());
								ExecuteInternalCommand(PNC_COMMAND__ABORT_PROGRAM, false);
                                return;
							} else if ((*firstList.begin()).signature_ != (*secondList.begin()).signature_) {
                                switch (client->desync_state) {
                                    case PNC_DESYNC_NONE:
                                    case PNC_DESYNC_RESTORE_FINISHED:
                                        client->desync_amount++;
                                        if (client->desync_amount > PNC_DESYNC_RESTORE_ATTEMPTS) {
                                            client->desync_state = PNC_DESYNC_RESTORE_FAILED;
                                            std::string gameID = std::to_string(time(nullptr)) + "_failed";
                                            netCommand4C_DesyncNotify ev_notify = netCommand4C_DesyncNotify(gameID);
                                            ev_notify.desync_amount = client->desync_amount;
                                            SendEvent(ev_notify, client->netidPlayer);
                                            fprintf(stderr, "Failed to recover network synchronization with 0x%" PRIX64 " after %d times\n", client->netidPlayer, client->desync_amount);
                                        } else {
                                            client_desync |= true;
                                            if (client->desync_last_time && clocki() - client->desync_last_time >
                                                PNC_DESYNC_RESTORE_ATTEMPTS_TIME) {
                                                client->desync_amount = 1;
                                            }
                                            client->desync_state = PNC_DESYNC_DETECTED;
                                            fprintf(stderr, "Error network synchronization with 0x%" PRIX64 ": "
                                                            "Unmatched game quants signatures ! Quant=%u\n",
                                                    client->netidPlayer, (*firstList.begin()).quant_);
                                        }
                                        client->desync_last_time = clocki();
                                        break;
                                    case PNC_DESYNC_RESTORE_FAILED:
                                        //Take it as valid for now
                                        countCompare++;
                                        break;
                                    case PNC_DESYNC_DETECTED:
                                        break;
                                    default: 
                                        xassert(0 && "Unexpected desync state");
                                        countCompare++;
                                        break;
                                }
							} else {
                                xassert(0 && "Unknown desync reason");
                            }
						}
					} else {
                        goto end_while_01; //break; //завершение while если один из спмсков кончится раньше первого
                    }
				}
                
                if (client_desync) {
                    goto end_while_01;
                }
                
				if(countCompare+1>=m_clients.size()) {
                    quantConfirmation=(*(*m_clients.begin())->backGameInf2List.begin()).quant_;
					//erase begin elements
					for(auto client : m_clients) {
						std::vector<netCommand4H_BackGameInformation2>& list=client->backGameInf2List;
						BUFFER_LOG <= (*list.begin()).quant_ < " " <= (*list.begin()).replay_
						           < " " <= (*list.begin()).signature_ < " " <= (*list.begin()).state_< "\n";
						///delete *secondList.begin();
						list.erase(list.begin());
                        if (client->desync_state == PNC_DESYNC_RESTORE_FINISHED) {
                            //If we are here then is resynced
                            fprintf(stdout, "Client network synchronization restored with 0x%" PRIX64 "\n", client->netidPlayer);
                            client->desync_state = PNC_DESYNC_NONE;
                        }
					}
				}
			}

end_while_01:;            
            if (client_desync) {
                ExecuteInternalCommand(PNC_COMMAND__DESYNC, false);
                return;
            }

			//std::string notResponceClientList;
			unsigned int maxInternalLagQuant=0;
			unsigned short minClientExecutionQuat=m_numberGameQuant;
			for (auto& client : m_clients) {
				if(client->lagQuant > maxInternalLagQuant) {//Для подгонки всех клиентов
					maxInternalLagQuant=client->lagQuant; 
				}
				if(client->lastExecuteQuant < minClientExecutionQuat) {//Для подгонки всех клиентов
					minClientExecutionQuat=client->lastExecuteQuant;
				}
                /*
				if(clocki() > (client->lastTimeBackPacket + TIMEOUT_CLIENT_OR_SERVER_RECEIVE_INFORMATION)){
					NETID d=client->netidPlayer;
					int n=hostMissionDescription->findPlayer(d);
					if(n!=-1) { notResponceClientList+=hostMissionDescription->playersData[n].name(); notResponceClientList+=' '; }
				}
                */
			}
			/// Подгонка под всех клиентов !
			//const unsigned int MAX_LAG_QUANT_WAIT=4;
			//if(maxInternalLagQuant > MAX_LAG_QUANT_WAIT) m_nextQuantTime+=m_quantInterval*(maxInternalLagQuant/2);
			const unsigned int MAX_EXTERNAL_LAG_QUANT_WAIT=8*3;
			const unsigned int maxExternalLagQuat=m_numberGameQuant-minClientExecutionQuat;
			unsigned int timePlus=0;
			if(maxExternalLagQuat>MAX_EXTERNAL_LAG_QUANT_WAIT) timePlus+=m_quantInterval*(maxExternalLagQuat/2);
			if(timePlus>1000) timePlus=500;// 0.5 secunda
			m_nextQuantTime+=timePlus;


			//Pause handler
			int playersIDArr[NETWORK_PLAYERS_MAX];
			for(int m=0; m<NETWORK_PLAYERS_MAX; m++) playersIDArr[m]=netCommand4C_Pause::NOT_PLAYER_ID;
			bool flag_requestPause=0;
			bool flag_changePause=0;
			int curPlayer=0;
            
			for (auto k=m_clients.begin(); k!=m_clients.end(); k++){
				flag_requestPause|=(*k)->requestPause;
				flag_changePause|=((*k)->requestPause!=(*k)->clientPause);
				(*k)->clientPause=(*k)->requestPause;
				if((*k)->requestPause){
					xassert(curPlayer<NETWORK_PLAYERS_MAX);
					if(curPlayer<NETWORK_PLAYERS_MAX){
						int idx=hostMissionDescription->findPlayer((*k)->netidPlayer);
						xassert(idx!=-1);
						if(idx!=-1) playersIDArr[curPlayer++]=hostMissionDescription->playersData[idx].playerID;
					}
				}
			}
            
            //Check if any client should be removed, only one per quant!
			for (auto& client : m_clients) {
				if ((client->requestPause && ((clocki()-client->timeRequestPause) > MAX_TIME_PAUSE_GAME))
                || (!hostPause && client->netidPlayer != m_hostNETID
                    && (client->last_time_latency_response + (TIMEOUT_DISCONNECT * 1000) < clock_us())
                   )
                ) {
                    fprintf(stdout, "Removing non responding player: 0x%" PRIX64 " %s\n", client->netidPlayer, client->playerName);

					RemovePlayer(client->netidPlayer);
					break;
				}
			}

			if(hostPause && (!flag_requestPause) ){
				//Отмена паузы
				netCommand4C_Pause ncp(playersIDArr, false);
				SendEvent(ncp, NETID_ALL);
				hostPause=false;
			}
			else if(flag_changePause && flag_requestPause){
				netCommand4C_Pause ncp(playersIDArr, true);
				SendEvent(ncp, NETID_ALL);
				hostPause=true;
			}
			if(hostPause)
				break;

			//перенесение всех команд удаления в список комманд на выполнение
			for(auto p=m_QueuedGameCommands.begin(); p != m_QueuedGameCommands.end(); p++){
				PutGameCommand2Queue_andAutoDelete(m_hostNETID, *p);
			}
            m_QueuedGameCommands.clear();

			//Установка последней команде, признака, последняя
			if(!m_CommandList.empty()){
				std::list<netCommandGeneral*>::iterator p=m_CommandList.end();
				p--; //последняя команда
				if((*p)->EventID==NETCOM_4G_ID_UNIT_COMMAND || (*p)->EventID==NETCOM_4G_ID_REGION || (*p)->EventID==NETCOM_4G_ID_FORCED_DEFEAT){
					(static_cast<netCommandGame*>(*p))->setFlagLastCommandInQuant();
				}
				else xassert(0 && "Error in commands list on server");
			}

			///hostGeneralCommandCounter+=m_CommandList.size(); //сейчас добавляется при PutGameCommand2Queue_andAutoDelete

            /* TODO is this even necessary? it was commented on original source release
			if(0){
				static bool flag_timeOut=0; //Надо перенести в тело класса!!!
				if(!flag_timeOut) {
					if(!notResponceClientList.empty()){
						netCommandNextQuant* pcom=new netCommandNextQuant(m_numberGameQuant, m_nQuantCommandCounter, hostGeneralCommandCounter, quantConfirmation, true);
						m_CommandList.push_back(pcom);

						netCommand4C_ClientIsNotResponce* pnr=new netCommand4C_ClientIsNotResponce(notResponceClientList);
						m_CommandList.push_back(pnr);
						flag_timeOut=1;
					}
					else {
						netCommandNextQuant* pcom=new netCommandNextQuant(m_numberGameQuant, m_nQuantCommandCounter, hostGeneralCommandCounter, quantConfirmation);
						m_CommandList.push_back(pcom);
					}
				}
				else {
					if(!notResponceClientList.empty()){
						netCommand4C_ClientIsNotResponce* pnr=new netCommand4C_ClientIsNotResponce(notResponceClientList);
						m_CommandList.push_back(pnr);
					}
					else {
						netCommandNextQuant* pcom=new netCommandNextQuant(m_numberGameQuant, m_nQuantCommandCounter, hostGeneralCommandCounter, quantConfirmation);
						m_CommandList.push_back(pcom);
					}
				}
			}
			else {
            */
				//Или сверху или это
				netCommandNextQuant* pcom=new netCommandNextQuant(m_numberGameQuant, m_nQuantCommandCounter, hostGeneralCommandCounter, quantConfirmation);
				m_CommandList.push_back(pcom);
			//}


	//		if(!DbgPause())
	//		{
				std::list<netCommandGeneral*>::iterator i;
				FOR_EACH(m_CommandList, i)
				{
                    SendEvent(**i, NETID_ALL);
					delete *i;
				}

				m_CommandList.clear();
	//		}


			m_numberGameQuant++;
		}
		break;
    case PNC_STATE__HOST_DESYNC: {
        CAutoLock _lock(m_GeneralLock); //! Lock
        
        int highest_amount = 0;
        bool waiting_ack = false;
        bool waiting_restore = false;
        std::set<PClientData*> to_notify;
        std::set<PClientData*> to_restore;
        for (auto& client : m_clients) {
            switch (client->desync_state) {
                case PNC_DESYNC_DETECTED:
                    highest_amount = std::max(highest_amount, client->desync_amount);
                    to_notify.emplace(client);
                    break;
                case PNC_DESYNC_NOTIFIED:
                    highest_amount = std::max(highest_amount, client->desync_amount);
                    waiting_ack = true;
                    break;
                case PNC_DESYNC_ACKNOLEDGED:
                    highest_amount = std::max(highest_amount, client->desync_amount);
                    to_restore.emplace(client);
                    break;
                case PNC_DESYNC_SENT_RESTORE:
                    waiting_restore = true;
                    break;
                default:
                    break;
            }
        }
        
        if (!to_notify.empty()) {
            for (auto& client : m_clients) {
                //We need to notify host to obtain its save
                //Or when notifying everyone due to full restore
                if (client->netidPlayer == m_hostNETID || highest_amount >= PNC_DESYNC_RESTORE_MODE_FULL) {
                    to_notify.emplace(client);
                }
            }
            
            //Send notify first
            std::string gameID = std::to_string(time(nullptr));
            netCommand4C_DesyncNotify ev_notify = netCommand4C_DesyncNotify(gameID);
            for (auto& client : to_notify) {
                if (client->netidPlayer == m_hostNETID) {
                    ev_notify.desync_amount = highest_amount;
                } else {
                    ev_notify.desync_amount = client->desync_amount;
                }
                SendEvent(ev_notify, client->netidPlayer);
                
                client->desync_state = PNC_DESYNC_NOTIFIED;
            }
        } else if (!waiting_ack && !to_restore.empty()) {
            //Find host client data
            PClientData* clientHost = nullptr;
            for (auto& client: m_clients) {
                if (client->netidPlayer == m_hostNETID) {
                    clientHost = client;
                }
            }
            
            //Check if we should include everyone
            if (highest_amount >= PNC_DESYNC_RESTORE_MODE_FULL) {
                for (auto& client: m_clients) {
                    switch (client->desync_state) {
                        case PNC_DESYNC_NONE:
                        case PNC_DESYNC_DETECTED:
                        case PNC_DESYNC_ACKNOLEDGED:
                            to_restore.emplace(client);
                            break;
                        default:
                            break;
                    }
                }
            }
            
            //We create a copy in event and save host state into it
            if (!clientHost || !clientHost->desync_missionDescription) {
                ErrH.Abort("Host client unavailable or doesn't contain mission description");
            }

            //Save the data for debugging
            std::string crash_dir = CRASH_DIR;
            terminate_with_char(crash_dir, PATH_SEP);
            crash_dir += "desync_" + std::to_string(time(nullptr)) + "_" + std::to_string(m_localNETID) + "_restore" + PATH_SEP;
            fprintf(stderr, "Dumped desync restore data at: %s\n", crash_dir.c_str());
            create_directories(crash_dir);
            for (auto& client : to_restore) {
                std::string path = crash_dir + std::to_string(client->netidPlayer);
                
                if (client->desync_netlog.tell()) {
                    XStream ffnl(setExtension(path, "log").c_str(), XS_OUT, 0);
                    ffnl.write(client->desync_netlog.address(), client->desync_netlog.tell());
                    ffnl.close();
                }
                
                if (client->desync_missionDescription == nullptr) {
                    continue;
                }
                MissionDescription mission(*client->desync_missionDescription);
                
                client->desync_missionDescription->setSaveName(path.c_str());

                //Save client mission data if any
                if (mission.saveData.length()) {
                    XStream ffp(setExtension(path, "prm").c_str(), XS_OUT, 0);
                    ffp.write(mission.saveData, mission.saveData.length());
                    ffp.close();
                }

                if (mission.binaryData.length()) {
                    XStream ffb(setExtension(path, "bin").c_str(), XS_OUT, 0);
                    ffb.write(mission.binaryData, mission.binaryData.length());
                    ffb.close();
                }

                //Load save data into IA and deserialize
                SavePrm savePrm;
                if (mission.saveData.length()) {
                    XPrmIArchive ia;
                    std::swap(ia.buffer(), mission.saveData);
                    ia.reset();
                    ia >> WRAP_NAME(savePrm, "SavePrm");
                }
                client->desync_missionDescription->saveMission(savePrm, true);
            }

            //Clear host buffers
            ClearInputPacketList();
            clearInOutClientHostBuffers();
            flag_SkipProcessingGameCommand=false;
            
            //Create restore event
            clientHost->desync_missionDescription->setSaveName("");
            netCommand4C_DesyncRestore ev_restore = netCommand4C_DesyncRestore(std::move(clientHost->desync_missionDescription));
            if (highest_amount < PNC_DESYNC_RESTORE_MODE_FULL) {
                ev_restore.missionDescription->gameType_ = GT_MULTI_PLAYER_RESTORE_PARTIAL;
            } else {
                ev_restore.missionDescription->gameType_ = GT_MULTI_PLAYER_RESTORE_FULL;
            }
            
            //Send the restore event to each desynced client
            for (auto& client : to_restore) {
                client->desync_missionDescription = nullptr;
                client->desync_state = PNC_DESYNC_SENT_RESTORE;
                client->desync_netlog.alloc(0);
                
                ev_restore.desync_amount = highest_amount;
                
                int playerID = ev_restore.missionDescription->findPlayer(client->netidPlayer);
                ev_restore.missionDescription->activePlayerID = playerID;
                SendEvent(ev_restore, client->netidPlayer);
            }

            if (highest_amount < PNC_DESYNC_RESTORE_MODE_FULL) {
                //Clear old backGame2Infos
                unsigned int curTime=clocki();
                for (auto& client : to_restore) {
                    client->m_flag_Ready=false;
                    client->lastTimeBackPacket=curTime;
                    client->backGameInf2List.clear();
                }
                ClearCommandList();
                flag_SkipProcessingGameCommand = false;
            } else {
                //Proceed like loading a game
                m_state = PNC_STATE__HOST_LOADING_GAME;
            }
        } else if (!waiting_ack && !waiting_restore) {            
            //No clients pending or restoring, continue hosting game
            m_state=PNC_STATE__HOST_GAME;
        }
        break;
    }
    case PNC_STATE__CLIENT_LOADING_GAME:
        ClearQueuedGameCommands(); /// TODO is this need? copied from original code
        break;
    case PNC_STATE__CLIENT_TUNING_GAME: {
        if (connectionHandler.isConnectionClosed(m_hostNETID)) {
            fprintf(stderr, "PNC_STATE__CLIENT_TUNING_GAME host closed\n");
            ExecuteInternalCommand(PNC_COMMAND__RESET, false);
            ExecuteInterfaceCommand(PNC_INTERFACE_COMMAND_HOST_TERMINATED_GAME);
        }
        break;
    }
    case PNC_STATE__CLIENT_FIND_HOST:
        serverList->fetchRelayHostInfoList();
        break;
    case PNC_STATE__CLIENT_GAME:
    case PNC_STATE__CLIENT_DESYNC:
		break;
	case PNC_STATE__NEWHOST_PHASE_0:
		{
			//Ожидание обработки всех комманд игрой
			{
				CAutoLock _lock(m_GeneralLock); //! Lock
				if(universe()){
					if(in_ClientBuf.currentNetCommandID()!=NETCOM_ID_NONE) break;
					if(universe()->allowedRealizingQuant > universe()->lastRealizedQuant) break;
					//по идее безопасно вызывать т.к. верхнее условие выполнится только при прошедшем кванте, но лучше переделать
					universe()->stopGame_HostMigrate();//Очистка всех команд текущего кванта
				}
				flag_SkipProcessingGameCommand=1;
				//По идее список пуст т.к. только выполнился ClientPredReceiveQuant
                ClearInputPacketList();
			}

			///m_netidGroupGame=m_groupNETID;
			//Разблокирование входных пакетов, заблокированных при поступлении сообщения о смене Host-а
			//Необходимо для того, чтобы не пропустить сообщение о reJoin-е
			UnLockInputPacket();

			//Уничтожение всех не выполнившихся команд ?
			///////CAutoLock lock(m_ClientInOutBuffLock);

			ClearClientData();
			//Т.к. миграция разрешается только после START_LOAD_GAME clientMissionDescription корректен
            delete hostMissionDescription;            
			hostMissionDescription = new MissionDescription(*clientMissionDescription);
			hostMissionDescription->clearAllPlayerStartReady();
			//missionDescription.clearAllPlayerGameReady();
			//clientMissionDescription

			m_beginWaitTime=clocki();

			//очистка InHostBuffer в него пришли уже не те подтверждения!
			{
				CAutoLock _lock(m_GeneralLock); //! Lock
				clearInOutClientHostBuffers();
			}
			if(AddClientToMigratedHost(m_localNETID, universe()->getCurrentGameQuant(), universe()->getConfirmQuant()) ){
				m_state=PNC_STATE__NEWHOST_PHASE_A;
			}
			else {
				///m_state=PNC_STATE__NET_CENTER_CRITICAL_ERROR;
				ExecuteInternalCommand(PNC_COMMAND__ABORT_PROGRAM, false);
			}
		}
		break;
	case PNC_STATE__NEWHOST_PHASE_A:
		{
			CAutoLock _lock(m_GeneralLock); //! Lock

			const unsigned int MAX_TIME_4_HOST_MIGRATE=10000;//10s

			bool result=1;
			///m_pConnection->refreshGroupMember();
			///list<NETID>::iterator p;
			///for(p=m_pConnection->gameGroupMemberList.begin(); p!=m_pConnection->gameGroupMemberList.end(); p++){
			///	result&=(missionDescription.isPlayerStartReady(*p));
			///}
			if(hostMissionDescription->isAllRealPlayerStartReady()){
				result=1;
			}
			else result=0;
			
			if( result==1 ) { //если все существующие в группе игроки
				unsigned int maxConfirmedQuant=0;
				///unsigned int maxQuant=0;
				///unsigned int minQuant=UINT_MAX;
				///NETID maxQuantClientNETID=0;
				ClientMapType::iterator p;
				for(p=m_clients.begin(); p!=m_clients.end(); p++){
					if((*p)->confirmQuant > maxConfirmedQuant){
						maxConfirmedQuant=(*p)->confirmQuant;
					}
					/*if((*p)->curLastQuant >= maxQuant){
						maxQuant=(*p)->curLastQuant;
						maxQuantClientNETID=(*p)->netidPlayer;
					}
					if((*p)->curLastQuant <= minQuant){
						minQuant=(*p)->curLastQuant;
					}*/
				}
				netCommand4C_sendLog2Host ncslh(maxConfirmedQuant+1);
				SendEvent(ncslh, NETID_ALL);

				///minQuant+=1; //Запрос следующего кванта за мин квантом
				///netCommand4C_RequestLastQuantsCommands nc(minQuant);
				///SendEvent(nc, maxQuantClientNETID);
				netidClientWhichWeWait=-1;//maxQuantClientNETID;


				m_beginWaitTime=clocki();
				m_state=PNC_STATE__NEWHOST_PHASE_B;
			}
		}
		break;
	case PNC_STATE__NEWHOST_PHASE_B:
		{
			//if( ((clocki()-m_beginWaitTime) > MAX_TIME_WAIT_RESTORE_GAME_AFTER_MIGRATE_HOST) ) {
			//}
			if( (netidClientWhichWeWait==-1) || (hostMissionDescription->findPlayer(netidClientWhichWeWait)==-1) ){ //игрок выбыл до прихода нужных хосту игровых комманд
				unsigned int maxQuant=0;
				unsigned int minQuant=UINT_MAX;
				NETID maxQuantClientNETID=NETID_NONE;
				ClientMapType::iterator p;
				for(p=m_clients.begin(); p!=m_clients.end(); p++){
					if((*p)->curLastQuant >= maxQuant){
						maxQuant=(*p)->curLastQuant;
						maxQuantClientNETID=(*p)->netidPlayer;
					}
					if((*p)->curLastQuant <= minQuant){
						minQuant=(*p)->curLastQuant;
					}
				}
				minQuant+=1; //Запрос следующего кванта за мин квантом
				netCommand4C_RequestLastQuantsCommands nc(minQuant);
				SendEvent(nc, maxQuantClientNETID);
				netidClientWhichWeWait=maxQuantClientNETID;
			}
		}
		break;
	case PNC_STATE__CLIENT_RESTORE_GAME_AFTE_CHANGE_HOST_PHASE_0:
		{
			//Ожидание обработки всех комманд игрой
			{
				CAutoLock _lock(m_GeneralLock); //! Lock
				if(universe()){
					if(in_ClientBuf.currentNetCommandID()!=NETCOM_ID_NONE) break;
					if(universe()->allowedRealizingQuant > universe()->lastRealizedQuant) break;
					//по идее безопасно вызывать т.к. верхнее условие выполнится только при прошедшем кванте, но лучше переделать
					universe()->stopGame_HostMigrate();//Очистка всех команд текущего кванта
				}
				flag_SkipProcessingGameCommand=1;
				//По идее список пуст т.к. только выполнился ClientPredReceiveQuant
                ClearInputPacketList();
			}

			//Разблокирование входных пакетов, заблокированных при поступлении сообщения о смене Host-а
			UnLockInputPacket();

			//очистка out_ClientBuf в него пришли уже не те подтверждения!
			{
				CAutoLock _lock(m_GeneralLock); //! Lock
				clearInOutClientHostBuffers();
			}
			m_state=PNC_STATE__CLIENT_RESTORE_GAME_AFTE_CHANGE_HOST_PHASE_AB;
			ClearClientData();
			m_beginWaitTime=clocki();
		}
		break;
	case PNC_STATE__CLIENT_RESTORE_GAME_AFTE_CHANGE_HOST_PHASE_AB:
		{
			CAutoLock _lock(m_GeneralLock); //! Lock для senda

			static unsigned char bandPassFilter=0;
			if((bandPassFilter&0x7)==0){//каждый 8й квант
				netCommand4H_ReJoinRequest ncrjr(universe()->getCurrentGameQuant(), universe()->getConfirmQuant() );
				SendEvent(ncrjr, m_hostNETID);
				///m_state=PNC_STATE__CLIENT_GAME;
			}
			bandPassFilter++;

			if((clocki()-m_beginWaitTime) < MAX_TIME_WAIT_RESTORE_GAME_AFTER_MIGRATE_HOST){
			}
			else { //все время вышло
			}
		}
		break;
    case PNC_STATE__NONE:
    case PNC_STATE__END:
	case PNC_STATE__NET_CENTER_CRITICAL_ERROR:
		{
            LogMsg("Final state, closing: %s\n", getStrState());
			//Close(false);
			ExecuteInternalCommand(PNC_COMMAND__END, false);
		}
		break;
    case PNC_STATE__CLOSED:
        break;
    case PNC_STATE__RESETTING:
        ExecuteInternalCommand(PNC_COMMAND__RESET, false);
		break;
	default:
		break;
	}	
}

void PNetCenter::LockInputPacket() {
    flag_LockIputPacket++;
}

void PNetCenter::UnLockInputPacket() {
    if (0 < flag_LockIputPacket) flag_LockIputPacket--;
}

void PNetCenter::ClientPredReceiveQuant()
{
    if(!out_ClientBuf.isEmpty()) SendNetBuffer(&out_ClientBuf, m_hostNETID);

    if(flag_LockIputPacket) return; //return 0;
    //int cnt=0;
    std::list<NetConnectionMessage*>::iterator p=m_InputPacketList.begin();
    while(p != m_InputPacketList.end()){
        NetConnectionMessage* packet = *p;
#if 0
        //отфильтровывание команды
        InOutNetComBuffer tmp(packet->address(),packet->tell());
        int cmd = tmp.currentNetCommandID();
        if (cmd == NETCOM_4C_ID_START_LOAD_GAME) {
            ExecuteInternalCommand(PNC_COMMAND__CLIENT_STARTING_LOAD_GAME, false);
        } else if (cmd == NETCOM_ID_NEXT_QUANT) {
            if (m_state == PNC_STATE__CLIENT_RESTORE_GAME_AFTE_CHANGE_HOST_PHASE_AB) {
                m_state=PNC_STATE__CLIENT_GAME;
            }
        }
#endif

        //комманды клиенту
        if (in_ClientBuf.putBufferPacket(packet->address(), packet->tell())) {
            delete packet;
            p=m_InputPacketList.erase(p);
            //cnt++;
        } else {
            break;
        }
    }
    ///return (cnt!=0);

}

