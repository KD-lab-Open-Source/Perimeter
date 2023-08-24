#include "NetIncludes.h"

#include "Runtime.h"
#include "P2P_interface.h"
#include "NetConnectionAux.h"

#include "Universe.h"

#include <algorithm>
#include <set>
#include <SDL.h>
#include "files/files.h"

#ifdef _WIN32
#include <combaseapi.h>
#endif

const unsigned int MAX_TIME_WAIT_RESTORE_GAME_AFTER_MIGRATE_HOST=10000;//10sec
const int PNC_DESYNC_RESTORE_ATTEMPTS = 8;
const int PNC_DESYNC_RESTORE_MODE_PARTIAL = 0; //2; TODO set back once partial load is finished 
const int PNC_DESYNC_RESTORE_ATTEMPTS_TIME = 5 * 60 * 1000; //5 mins
const int PNC_DESYNC_RESTORE_MODE_FULL = PNC_DESYNC_RESTORE_MODE_PARTIAL + 1;
const size_t PNC_LATENCY_UPDATE_INTERVAL = 50 * 1000; //us

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
PClientData::PClientData(const char* name, NETID netid)
{
    strncpy(playerName, name, PLAYER_MAX_NAME_LEN);
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
//
///////////////////////////////////////////////////////////

void PNetCenter::clearInOutClientHostBuffers()
{
	in_ClientBuf.reset();
	out_ClientBuf.reset();

	in_HostBuf.reset();
	out_HostBuf.reset();
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

extern SDL_threadID net_thread_id;

//Second thread
int InternalServerThread(void* lpParameter)
{
/*	// TEST thread!!!!
	DWORD ThreadId;
	HANDLE  hTestThread=CreateThread( NULL, 0, TestServerThread, lpParameter, 0, &ThreadId);
*/

    net_thread_id = SDL_ThreadID();

	PNetCenter* pPNetCenter=(PNetCenter*)lpParameter;
	pPNetCenter->SecondThread();

    net_thread_id = -1;
    
	//We are creating it with SDL thread, so we need to manually signal it
    SetEvent(pPNetCenter->hSecondThread);
    
	return 0;
}
XBuffer BUFFER_LOG(10000,1);

bool PNetCenter::SecondThread(void)
{
    xassert(net_thread_id == SDL_ThreadID());

#ifdef _WIN32
    //TODO is this necessary?
    CoInitializeEx(0, COINIT_MULTITHREADED);
#endif

	Init();

	m_state=PNC_STATE__CLIENT_FIND_HOST;

	//Инициализация завершена - XDPConnection создан
	SetEvent(hSecondThreadInitComplete);

	bool flag_end = false;

	while(!flag_end){
		CAutoLock* _pLock=new CAutoLock(m_GeneralLock);
		//decoding command 
		if(internalCommandList.size() > 0){
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
				{
					flag_LockIputPacket=0;
					flag_SkipProcessingGameCommand=0;
					ClearInputPacketList();
					clearInOutClientHostBuffers();
					m_bStarted = false;
					if(m_state==PNC_STATE__CLIENT_FIND_HOST){
                        m_state=PNC_STATE__CONNECTION;
					}
					else xassert(0&&"Connecting: command order error(not find host state)");
				}
				break;
			case PNC_COMMAND__DISCONNECT_AND_ABORT_GAME_AND_END:
				{
					m_bStarted = false;

					///if(m_state==PNC_STATE__CLIENT_TUNING_GAME || m_state==PNC_STATE__CLIENT_LOADING_GAME || m_state==PNC_STATE__CLIENT_GAME){
					flag_end= true;
					///}
					///else xassert(0&&"Disconnect: command order error(not game)");
				}
				SetEvent(hCommandExecuted);
				break;
			case PNC_COMMAND__START_HOST_AND_CREATE_GAME_AND_STOP_FIND_HOST:
				{
					flag_LockIputPacket=0;
					flag_SkipProcessingGameCommand=0;
					ClearInputPacketList();
					clearInOutClientHostBuffers();
					m_bStarted = false;

					////if(WaitForSingleObject(hStartServer, INFINITE) != WAIT_OBJECT_0) xassert(0&&"Network server: run error.");
					m_state=PNC_STATE__HOST_TUNING_GAME; //Необходимо для DPN_MSGID_ENUM_HOSTS_QUERY чтоб сразу выдавал правильную инфу
					serverList.stopHostFind();

					LogMsg("starting server...\n");
					if(!isConnected()) {
						//m_pConnection->Init();
						if (ServerStart()) {
                            LogMsg("...started OK\n");
                        }
					}

				//	pNewGame->AddClient(nccg.createPlayerData_, 0/*netid*/, nccg.computerName_);
				//	pNewGame->StartGame();

					///hostGUIDInstance=getHostGUIDInstance();
					///m_netidGroupGame = m_pConnection->CreateGroup();

					////SetEvent(hServerReady);

					ClearClients();
                    PlayerData pd;
                    pd.set(m_PlayerName, m_localNETID);
					if(AddClient(pd)==-1){
						ErrH.Abort("Network: Couldnt add host player to mission");
					}

					LogMsg("New game <%s> for start...\n", m_GameName.c_str());

					hostMissionDescription->clearAllPlayerGameReady();
					hostMissionDescription->setChanged();
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
			case PNC_COMMAND__DISCONNECT_AND_ABORT_GAME_AND_END_START_FIND_HOST:
				{
					///if(m_state==PNC_STATE__CONNECTION || m_state==PNC_STATE__CLIENT_TUNING_GAME || m_state==PNC_STATE__CLIENT_LOADING_GAME || m_state==PNC_STATE__CLIENT_GAME){
					///}
					///else xassert(0&&"Disconnect: command order error(not game)");
				}
				//break; !!!Специально!!!
			case PNC_COMMAND__STOP_HOST_AND_ABORT_GAME_AND_START_FIND_HOST:
				{
					m_bStarted = false;
					if(isConnected()) {
						Close();
					}
                    if (m_state == PNC_STATE__CONNECTION) {
                        //Avoid calling connect twice until PNC_COMMAND__START_FIND_HOST is processed
                        m_state = PNC_STATE__CLIENT_FIND_HOST;
                    }
                    ExecuteInternalCommand(PNC_COMMAND__START_FIND_HOST, false);
				}
				SetEvent(hCommandExecuted);
				break;
			case PNC_COMMAND__END_GAME:
				{
					m_state=PNC_STATE__ENDING_GAME;
					m_bStarted = false;
					if(isConnected()) {
						if(isHost()){
							//Гарантированная отсылка последнего кванта
							netCommandNextQuant com(m_numberGameQuant, 0, hostGeneralCommandCounter, 0);
							SendEvent(com, NETID_ALL);
						}
                        
                        //Send exit event to everyone connected (host or clients)
                        netCommand4G_Exit ex(m_localNETID);
                        SendEvent(ex, NETID_ALL);
					}
				}
				break;
			case PNC_COMMAND__START_FIND_HOST:
				{
                    serverList.startHostFind();
					m_state=PNC_STATE__CLIENT_FIND_HOST;
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
			case PNC_COMMAND__CLIENT_STARTING_LOAD_GAME:
				{
					m_state=PNC_STATE__CLIENT_LOADING_GAME;
				}
			case PNC_COMMAND__CLIENT_STARTING_GAME:
				{
					m_state=PNC_STATE__CLIENT_GAME;
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
                HostReceiveQuant();
            } else {
                ClientPredReceiveQuant();
            }
        }

		delete _pLock;

		if(flag_end) break; //для быстрого выхода
        
        //Accept any new incoming connections
        if (flag_connected && isHost()) {
            connectionHandler.acceptConnection();
        }

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
		if(minWakingTime > curTime){
			Sleep(minWakingTime-curTime);
		}
		//end logic quant
	}

	serverList.stopHostFind();

	SetConnectionTimeout(1);//Для быстрого завершения
	//if(m_pConnection->Connected()) m_pConnection->Close();
	Close();

#ifdef _WIN32
    CoUninitialize();
#endif

	return false;
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
            printf("Sending mission to idx %d id %d netid %lu\n", i, pd.playerID, pd.netid);
			SendEvent(nccsl, pd.netid);
		}
	}
    mission->activePlayerID = mission->findPlayer(m_localNETID);

	LogMsg("Sent battle info\n");
}

void PNetCenter::UpdateCurrentMissionDescription4C()
{
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
			LogMsg("Client NID %lu is not ready. removing.\n", (*i)->netidPlayer);

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
        LogMsg("Client NID %lu\n", (*i)->netidPlayer);
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

		LogMsg("ReJoin client NID %lu PID %d for game %s\n", _netid, idxPlayerData, m_GameName.c_str());

		return 1;
	}
	else {
		LogMsg("Client NID %lu for game %s id denied\n", _netid, m_GameName.c_str());
		return 0;
	}
}


void PNetCenter::SendEvent(netCommandGeneral& event, NETID destination)
{
    if (destination != m_localNETID ) {
        out_HostBuf.putNetCommand(&event);
        out_HostBuf.send(*this, destination);
    }

    if( (destination == m_localNETID) || (destination == NETID_ALL) ){
        in_ClientBuf.putNetCommand(&event);
    }
}

//void PNetCenter::StartGame()
//{
	//m_flag_PlayerListReady=1;
//}

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


	switch(m_state) {

	case PNC_STATE__CONNECTION:
        if(!Connect()) {
            ExecuteInternalCommand(PNC_COMMAND__DISCONNECT_AND_ABORT_GAME_AND_END_START_FIND_HOST, false);
            //ErrH.Abort("Unable to find multiplayer server");
        } else {
            serverList.stopHostFind();
            m_state = PNC_STATE__CLIENT_TUNING_GAME;
            SetEvent(hCommandExecuted);
        }
		break;
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
                        buf < "Game of the player " <= (*i)->netidPlayer < "does not meet to game of the player " <=
                        (*m_clients.begin())->netidPlayer;
                        xxassert(0, buf.buf);
                    }
                }
			}
			if (flag_ready) {
				CheckClients();
				DumpClients();


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
					if (client->netidPlayer == NETID_HOST) {
						ev.info.quant = quant;
					}
                }
                //Sometimes host might be absent in backgame2list, so use the highest quant from clients
                if (ev.info.quant == 0) {
                    ev.info.quant = max_quant;
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
                            if (client->netidPlayer != m_hostNETID) {
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
                                fprintf(stderr, "Error network synchronization with %llX: %s\n", client->netidPlayer, to.address());
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
                                            fprintf(stderr, "Failed to recover network synchronization with %llX after %d times\n", client->netidPlayer, client->desync_amount);
                                        } else {
                                            client_desync |= true;
                                            if (client->desync_last_time && clocki() - client->desync_last_time >
                                                PNC_DESYNC_RESTORE_ATTEMPTS_TIME) {
                                                client->desync_amount = 1;
                                            }
                                            client->desync_state = PNC_DESYNC_DETECTED;
                                            fprintf(stderr, "Error network synchronization with %llX: "
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
                            fprintf(stdout, "Client network synchronization restored with %llX\n", client->netidPlayer);
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
                || (!hostPause && client->netidPlayer != NETID_HOST && (client->last_time_latency_response + (TIMEOUT_DISCONNECT * 1000) < clock_us()))) {
                    fprintf(stdout, "Removing non responding player: %lu %s\n", client->netidPlayer, client->playerName);
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
				PutGameCommand2Queue_andAutoDelete(NETID_HOST, *p);
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
    case PNC_STATE__CLIENT_FIND_HOST:
	case PNC_STATE__CLIENT_TUNING_GAME: 
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
	case PNC_STATE__ENDING_GAME:
		{
			serverList.stopHostFind();
			Close(false);
			CAutoLock _lock(m_GeneralLock); //! Lock
			ClearClients();
			m_state=PNC_STATE__NONE;
		}
		SetEvent(hCommandExecuted);
		break;
	default:
		break;
	}	
}

void PNetCenter::ClientPredReceiveQuant()
{
	if(!out_ClientBuf.isEmpty()) out_ClientBuf.send(*this, m_hostNETID);

	if(flag_LockIputPacket) return; //return 0;
	int cnt=0;
	std::list<InputPacket*>::iterator p=m_InputPacketList.begin();
	while(p != m_InputPacketList.end()){
        InputPacket* packet = *p;
		if(packet->netid==m_hostNETID){

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

			//комманды клиенту
			if (in_ClientBuf.putBufferPacket(packet->address(), packet->tell())) {
                delete packet;
				p=m_InputPacketList.erase(p);
				cnt++;
			} else {
                break;
            }
		} else {
			fprintf(stderr, "Received packet from non-host! %llu\n", packet->netid);
            delete packet;
			p=m_InputPacketList.erase(p);
		}
	}
	///return (cnt!=0);

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


		ClientMapType::iterator p;
		for(p=m_clients.begin(); p!=m_clients.end(); p++){
			//if(in_buffer.receive(*m_pConnection, netid))
			/////////////in_HostBuf.receive(*m_pConnection, netid);
			if(netid==(*p)->netidPlayer) {
				while(in_HostBuf.currentNetCommandID()) {
					netLog <= in_HostBuf.currentNetCommandID() < "\n";
					switch(in_HostBuf.currentNetCommandID()) {

					case NETCOM_4H_ID_REQUEST_PAUSE:
						{
							netCommand4H_RequestPause nc_rp(in_HostBuf);
							(*p)->requestPause=nc_rp.pause;
							(*p)->timeRequestPause=clocki();

/*							if( (!(*p)->requestPause) && nc_rp.pause){
								int playersIDArr[NETWORK_PLAYERS_MAX];
								for(int i=0; i<NETWORK_PLAYERS_MAX; i++) playersIDArr[i]=netCommand4C_Pause::NOT_PLAYER_ID;
								playersIDArr[0]=nc_rp.playerID;
								netCommand4C_Pause ncp(playersIDArr, true);
								SendEvent(ncp, NETID_ALL_PLAYERS_GROUP);
								(*p)->requestPause=true;
							}
							else if((*p)->requestPause && (!nc_rp.pause) ){
								int playersIDArr[NETWORK_PLAYERS_MAX];
								for(int i=0; i<NETWORK_PLAYERS_MAX; i++) playersIDArr[i]=netCommand4C_Pause::NOT_PLAYER_ID;
								netCommand4C_Pause ncp(playersIDArr, false);
								SendEvent(ncp, NETID_ALL_PLAYERS_GROUP);
								(*p)->requestPause=false;
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
                                if (!origin.flag_playerStartReady && !destination.flag_playerStartReady) {
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
							(*p)->m_flag_Ready=true;
							(*p)->clientGameCRC=event.gameCRC_;

                            LogMsg("Player 0x%X (GCRC=0x%X) reported ready\n", netid, (*p)->clientGameCRC);
                            
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
                            
                            LogMsg("Desync Ack %llu\n", netid);

                            //Flag the client as acknowledged
                            for (auto& client : m_clients) {
                                if (client->netidPlayer == netid) {
                                    xassert(client->desync_state == PNC_DESYNC_NOTIFIED);
                                    client->desync_state = PNC_DESYNC_ACKNOLEDGED;
                                    client->desync_missionDescription = std::move(event.missionDescription);
                                    std::swap(client->desync_netlog, event.netlog);
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
							(*p)->backGameInf2List.push_back(nc);
							(*p)->lagQuant=nc.lagQuant_;
							(*p)->lastExecuteQuant=nc.quant_;
							(*p)->lastTimeBackPacket=clocki();
						}
						break;
					case NETCOM_4H_ID_LATENCY_RESPONSE:
						{
							netCommand4H_LatencyResponse nc(in_HostBuf);
                            size_t timestamp = clock_us();
                            if (timestamp > nc.timestamp) {
                                (*p)->last_time_latency_response = timestamp;
                                (*p)->latency = timestamp - nc.timestamp;
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
								case NETCOM_4G_ID_UNIT_COMMAND: 
									{
										netCommand4G_UnitCommand*  pnc= new netCommand4G_UnitCommand(in_buffer);
										tmpListGameCommands.push_back(pnc);
									}
									break;
								case NETCOM_4G_ID_REGION:
									{
										netCommand4G_Region*  pnc= new netCommand4G_Region(in_buffer);
										tmpListGameCommands.push_back(pnc);
									}
									break;
								case NETCOM_4G_ID_FORCED_DEFEAT:
									{
										netCommand4G_ForcedDefeat* pnc=new netCommand4G_ForcedDefeat(in_buffer);
										tmpListGameCommands.push_back(pnc);
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
				break; //for-a
			}
		}
		if(p==m_clients.end()){ //Ни один существующий клиент не соответствует NETID
			while(in_HostBuf.currentNetCommandID()) {
				if(in_HostBuf.currentNetCommandID()==NETCOMC_ID_JOIN_REQUEST){
					//HandleNewPlayer(netid);
					//netCommandC_JoinRequest nc(in_HostBuf);
				}
				else if(in_HostBuf.currentNetCommandID()==NETCOM_4H_ID_REJOIN_REQUEST && m_state==PNC_STATE__NEWHOST_PHASE_A){
					netCommand4H_ReJoinRequest nc(in_HostBuf);
					AddClientToMigratedHost(netid, nc.currentLastQuant, nc.confirmedQuant);
				}
				else if(in_HostBuf.currentNetCommandID()==NETCOM_4H_ID_RESPONCE_LAST_QUANTS_COMMANDS){
					netCommand4H_ResponceLastQuantsCommands nci(in_HostBuf);
				}
				else {
					xassert("Invalid netCommand to host (unknown source)");
					in_HostBuf.ignoreNetCommand();
				}

				in_HostBuf.nextNetCommand();//Завершение обработки комманды
			}
		}
		
	}while(PutInputPacket2NetBuffer(in_HostBuf, netid)!=0);
}


void PNetCenter::UnLockInputPacket(void)
{
	if(flag_LockIputPacket) flag_LockIputPacket--;
}

void PNetCenter::ClearInputPacketList() {
    for (auto packet : m_InputPacketList) {
        delete packet;
    }
    m_InputPacketList.clear();
}

bool PNetCenter::PutInputPacket2NetBuffer(InOutNetComBuffer& netBuf, NETID& returnNETID)
{
	if(flag_LockIputPacket) return 0;

	int cnt=0;
	std::list<InputPacket*>::iterator p=m_InputPacketList.begin();
	if(p != m_InputPacketList.end()){
		returnNETID=(*p)->netid;
		while(p != m_InputPacketList.end()){
            InputPacket* packet = *p;
			if(returnNETID==packet->netid){
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

