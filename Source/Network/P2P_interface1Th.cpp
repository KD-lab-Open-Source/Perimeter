#include "NetIncludes.h"

#include "P2P_interface.h"
#include "ServerList.h"

#include <SDL.h>

#include "GameShell.h"
#include "Universe.h"
#include "files/files.h"
#include "NetConnectionAux.h"
#include "Runtime.h"
#include "../HT/mt_config.h"

#include <algorithm>
#include "qd_textdb.h"
#include "Localization.h"

extern const char* currentShortVersion;

const int NORMAL_QUANT_INTERVAL=100;
const int PNETCENTER_BUFFER_SIZE = PERIMETER_MESSAGE_MAX_SIZE * 2;

std::atomic_uint64_t net_thread_id = -1;

const char* PNetCenter::getStrState() const
{
	switch(m_state){
	case		PNC_STATE__NONE:
		return("PNC_STATE__NONE");
    case		PNC_STATE__END:
        return("PNC_STATE__END");
    case		PNC_STATE__NET_CENTER_CRITICAL_ERROR:
        return("PNC_STATE__NET_CENTER_CRITICAL_ERROR");
    case		PNC_STATE__CLOSED:
        return("PNC_STATE__CLOSED");
    case		PNC_STATE__RESETTING:
        return("PNC_STATE__RESETTING");
            
	case		PNC_STATE__CLIENT_FIND_HOST:
		return("PNC_STATE__CLIENT_FIND_HOST");
	case		PNC_STATE__CLIENT_TUNING_GAME:
		return("PNC_STATE__CLIENT_TUNING_GAME");
	case		PNC_STATE__CLIENT_LOADING_GAME:
		return("PNC_STATE__CLIENT_LOADING_GAME");
	case		PNC_STATE__CLIENT_GAME:
		return("PNC_STATE__CLIENT_GAME");
    case		PNC_STATE__CLIENT_DESYNC:
        return("PNC_STATE__CLIENT_DESYNC");

	case		PNC_STATE__CLIENT_RESTORE_GAME_AFTE_CHANGE_HOST_PHASE_0:
		return("PNC_STATE__CLIENT_RESTORE_GAME_AFTE_CHANGE_HOST_PHASE_0");
	case		PNC_STATE__CLIENT_RESTORE_GAME_AFTE_CHANGE_HOST_PHASE_AB:
		return("PNC_STATE__CLIENT_RESTORE_GAME_AFTE_CHANGE_HOST_PHASE_AB");
        
	case		PNC_STATE__HOST_TUNING_GAME:
		return("PNC_STATE__HOST_TUNING_GAME");
    case		PNC_STATE__HOST_SENDING_GAME:
        return("PNC_STATE__HOST_SENDING_GAME");
	case		PNC_STATE__HOST_LOADING_GAME:
		return("PNC_STATE__HOST_LOADING_GAME");
	case		PNC_STATE__HOST_GAME:
		return("PNC_STATE__HOST_GAME");
    case		PNC_STATE__HOST_DESYNC:
        return("PNC_STATE__HOST_DESYNC");

	case		PNC_STATE__NEWHOST_PHASE_0:
		return("PNC_STATE__NEWHOST_PHASE_0");
	case		PNC_STATE__NEWHOST_PHASE_A:
		return("PNC_STATE__NEWHOST_PHASE_A");
	case		PNC_STATE__NEWHOST_PHASE_B:
		return("PNC_STATE__NEWHOST_PHASE_B");
	default:
		return("PNC_???");
	}
}

PNetCenter::PNetCenter() :
in_ClientBuf(PNETCENTER_BUFFER_SIZE, true), out_ClientBuf(PNETCENTER_BUFFER_SIZE, true),
in_HostBuf(PNETCENTER_BUFFER_SIZE, true), out_HostBuf(PNETCENTER_BUFFER_SIZE, true),
connectionHandler(this)
{
    hostConnection=NetAddress();

    m_hostNETID = m_localNETID = NETID_NONE;

	m_clients.reserve(NETWORK_PLAYERS_MAX);

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
    serverList = new ServerList();


	flag_LockIputPacket=0;
	flag_SkipProcessingGameCommand=0;
	hostPause=0;
	clientPause=0;
	clientInPacketPause=0;

	m_quantInterval=NORMAL_QUANT_INTERVAL;

	
	TIMEOUT_CLIENT_OR_SERVER_RECEIVE_INFORMATION=10000;//5s
	TIMEOUT_CLIENT_OR_SERVER_RECEIVE_INFORMATION=IniManager("Network.ini").getInt("General","TimeOutClientOrServerReceive");

	TIMEOUT_DISCONNECT=60000;
	TIMEOUT_DISCONNECT=IniManager("Network.ini").getInt("General","TimeOutDisconnect");

	MAX_TIME_PAUSE_GAME=40000;
	IniManager("Network.ini").getInt("General","MaxTimePauseGame", (int&)MAX_TIME_PAUSE_GAME);

	//m_state=PGMS_CREATING_GAME;
	m_state=PNC_STATE__NONE;//PNCS_SIMLE_CLIENT;



	//m_quantPeriod=100;//ms
	m_nextQuantTime=0;

	hostGeneralCommandCounter=0;

	//m_nWorldID = -1;
	m_bStarted = false;
	m_nQuantCommandCounter = 0;

	//m_nServerPause = 0;

	///m_hPlayerListReady = CreateEvent(0, TRUE, FALSE, 0);
	//m_flag_PlayerListReady=0;

	m_GeneralLock = SDL_CreateMutex();

	///hServerReady=CreateEvent(0, true, false, 0);
	///hStartServer=CreateEvent(0, true, false, 0);

	hSecondThreadInitComplete=CreateEvent(0, true, false, 0);
	hCommandExecuted=CreateEvent(0, true, false, 0);

    hSecondThread = CreateEvent(0, true, false, 0);
    if (MTConfig::multithreading()) {
        SDL_Thread *thread = SDL_CreateThread(InternalServerThreadInit, "perimeter_server_thread", this);
        if (thread == nullptr) {
            SDL_FATAL_ERROR("SDL_CreateThread perimeter_server_thread failed");
        }
        SDL_DetachThread(thread);

        if(WaitForSingleObject(hSecondThreadInitComplete, INFINITE) != WAIT_OBJECT_0) {
            xassert(0&&"NetCenter:Error second thread init");
            ErrH.Abort("Network: General error 1!");
        }
        xassert(net_thread_id == SDL_GetThreadID(thread));
    } else {
        InternalServerThreadInit(this);
    }


	lastTimeServerPacket=clocki();

    LogMsg("Created PNetCenter\n");
}

extern PNetCenter* netCenter;
PNetCenter::~PNetCenter()
{
    netCenter = nullptr;
	ExecuteInternalCommand(PNC_COMMAND__END, true);
	if(MTConfig::multithreading() && WaitForSingleObject(hSecondThread, 5000) != WAIT_OBJECT_0) {
        LogMsg("PNetCenter::~PNetCenter(): InternalServerThread deadlock!!!\n");
		xassert(0);
	}

    serverList->stopFind();
    SetConnectionTimeout(1); // For quick termination
    connectionHandler.reset();

	ClearClients();

    ClearInputPacketList();



	SDL_DestroyMutex(m_GeneralLock);
	///CloseHandle(hServerReady);
	///CloseHandle(hStartServer);

    DestroyEvent(hSecondThreadInitComplete);
    DestroyEvent(hCommandExecuted);

    if (hostMissionDescription) {
        delete hostMissionDescription;
        hostMissionDescription = nullptr;
    }

    if (clientMissionDescription) {
        delete clientMissionDescription;
        clientMissionDescription = nullptr;
    }

    for (auto cmd : interfaceCommandList) {
        delete cmd;
    }
    
    if (serverList) {
        delete serverList;
        serverList = nullptr;
    }

    LogMsg("Destroyed PNetCenter\n");
}

//////////////////////////////////////////////
/// New network game creation or joining

void PNetCenter::CreateGame(
        bool isPublicGame, const NetAddress& connection,
        const std::string& gameName, MissionDescription* mission,
        const std::string& playerName, const std::string& password
) {
    LogMsg("Create Game\n");
	clientPause=false;
	clientInPacketPause=false;
    hostConnection = connection;
    clientRoom = 0;
    gamePassword = password;

	m_quantInterval=NORMAL_QUANT_INTERVAL; //xm::round((float)NORMAL_QUANT_INTERVAL/gameSpeed);
    
    //Setup mission
    xassert(mission);
    hostMissionDescription = mission;

    //Arguments for:
    // PNC_COMMAND__START_HOST_AND_CREATE_GAME_AND_STOP_FIND_HOST
    // PNC_COMMAND__CONNECT_RELAY_AND_CREATE_GAME_AND_STOP_FIND_HOST
    
	m_GameName = gameName;
	m_PlayerName = playerName;
	ExecuteInternalCommand(
            isPublicGame ? PNC_COMMAND__CONNECT_RELAY_AND_CREATE_GAME_AND_STOP_FIND_HOST
            : PNC_COMMAND__START_HOST_AND_CREATE_GAME_AND_STOP_FIND_HOST,
            true
    );

    gameShell->callBack_CreateGameReturnCode(isConnected() ? GameShell::CG_RC_OK : GameShell::CG_RC_CREATE_HOST_ERR);
}

void PNetCenter::JoinDirectGame(const NetAddress& connection, const std::string& playerName, const std::string& password)
{
    LogMsg("Join Direct Game\n");
    clientPause = false;
    clientInPacketPause=false;
    hostConnection = connection;
    clientRoom = 0;
    gamePassword = password;

    //Argument  PNC_COMMAND__CONNECT_2_HOST_AND_STOP_FIND_HOST
    m_PlayerName = playerName;
    //internalIP установлен ранее
    ExecuteInternalCommand(PNC_COMMAND__CONNECT_2_HOST_AND_STOP_FIND_HOST, false);
}

void PNetCenter::JoinPublicRoomGame(const NetAddress& connection, NetRoomID room_id, const std::string& playerName, const std::string& password) {
    LogMsg("Join Public Room Game\n");
    clientPause = false;
    clientInPacketPause=false;
    hostConnection = connection;
    clientRoom = room_id;
    gamePassword = password;

    //Argument  PNC_COMMAND__CONNECT_2_RELAY_ROOM_AND_STOP_FIND_HOST
    m_PlayerName = playerName;
    ExecuteInternalCommand(PNC_COMMAND__CONNECT_2_RELAY_ROOM_AND_STOP_FIND_HOST, false);
}

//////////////////////////////////////////////

// first thread function !
void PNetCenter::SendEvent(const netCommandGeneral* event)
{
	SendEventSync(event);
}
void PNetCenter::SendEventSync(const netCommandGeneral* event)
{
	CAutoLock lock(m_GeneralLock);

	if(isHost()){
		in_HostBuf.putNetCommand(event);
	}
	else { //Client
		out_ClientBuf.putNetCommand(event);
	}
}


//First thread !


void PNetCenter::HandlerInputNetCommand()
{
	while(in_ClientBuf.currentNetCommandID()!=NETCOM_ID_NONE) {
		terEventID event = (terEventID)in_ClientBuf.currentNetCommandID();

		lastTimeServerPacket=clocki();
		switch(event){
		case NETCOM_4C_ID_REQUEST_LAST_QUANTS_COMMANDS:
			{
				netCommand4C_RequestLastQuantsCommands nc(in_ClientBuf);

				if(universe()){
					//По идее вызов корректный т.к. reJoin не пошлется пока игра не остановлена(stopGame_HostMigrate)
					universe()->sendListGameCommand2Host(nc.beginQunat_);
				}
			}
			break;
		case NETCOM_4C_ID_START_LOAD_GAME:
			{
				netCommand4C_StartLoadGame nc4c_sl(in_ClientBuf);
                if (nc4c_sl.missionDescription_ == nullptr) {
                    //Not yet the real start but about to start
                    gameShell->MultiplayerGameStarting();
                } else {
                    //Transfer mission from packet
                    clientMissionDescription = nc4c_sl.missionDescription_;
                    nc4c_sl.missionDescription_ = nullptr;
                    
                    //Set active player as ourselves
                    int playerIndex = clientMissionDescription->findPlayer(m_localNETID);
                    if (0 <= playerIndex) {
                        clientMissionDescription->activePlayerID = clientMissionDescription->playersData[playerIndex].playerID;
                    }
                    
                    m_bStarted = true;

                    if (!isHost()) {
                        ExecuteInternalCommand(PNC_COMMAND__CLIENT_STARTING_LOAD_GAME, false);
                    }
                    //game_shell.GameStart(ncbc.missionDescription_);
                    gameShell->MultiplayerGameStart(*clientMissionDescription);
                }
			}
			break;
        case NETCOM_4G_ID_EXIT: {
            netCommand4G_Exit nc4g_exit(in_ClientBuf);
            //Host already does this before relaying to itself
            if (!isHost()) {
                if (nc4g_exit.netid == m_localNETID) {
                    ExecuteInternalCommand(PNC_COMMAND__RESET, false);
                    ExecuteInterfaceCommand(PNC_INTERFACE_COMMAND_KICKED);
                } else {
                    ExitClient(nc4g_exit.netid);
                }
            }
            break;
        }
		case NETCOM_4C_ID_PAUSE:
			{
				netCommand4C_Pause ncp(in_ClientBuf);
				if(ncp.pause){
					std::string s;
					for(int i=0; i<NETWORK_PLAYERS_MAX; i++){
						if(ncp.playersIDArr[i]!=netCommand4C_Pause::NOT_PLAYER_ID) {
							clientMissionDescription->getPlayerName(ncp.playersIDArr[i], s);
						}
					}
					gameShell->showConnectFailedInGame(s);
					clientPause=true;
					clientInPacketPause=false;
				}
				else {
					gameShell->hideConnectFailedInGame();
					clientPause=false;
				}

			}
			break;
		case NETCOM_4C_ID_CUR_MISSION_DESCRIPTION_INFO: 
			{
				netCommand4C_CurrentMissionDescriptionInfo ncCMD(in_ClientBuf);
                lobbyMissionDescription=ncCMD.missionDescription_;
			}
			break;
		case NETCOM_4C_ID_DISPLAY_DISTRINC_AREAS:
			{
				netCommand4C_DisplayDistrincAreas event(in_ClientBuf);
				vMap.displayChAreas(event.pDAData_, event.DASize_);
			}
			break;
		case NETCOM_4C_ID_CONTINUE_GAME_AFTER_HOST_MIGRATE:
			{
				netCommand4C_ContinueGameAfterHostMigrate nc(in_ClientBuf);
				flag_SkipProcessingGameCommand=0; //Возобновление после миграции Hosta
			}
			break;
		case NETCOM_4C_ID_SEND_LOG_2_HOST:
			{
				// !!! передается HiperSpace
				xassert(flag_SkipProcessingGameCommand);
				if(universe()) 
					universe()->ReceiveEvent(event, in_ClientBuf);
			}
			break;
		case NETCOM_4C_ID_LATENCY_STATUS:
			{
				netCommand4C_LatencyStatus nc(in_ClientBuf);
                
                //Send timestamp back to server
                netCommand4H_LatencyResponse rsp;
                rsp.timestamp = nc.info.timestamp;
                SendEventSync(&rsp);
                
                gameShell->updateLatencyInfo(nc.info, clientMissionDescription);
			}
			break;
		case NETCOM_4C_ID_CLIENT_IS_NOT_RESPONCE:
			{
				netCommand4C_ClientIsNotResponce nc(in_ClientBuf);
			}
			break;
		case NETCOM_4G_ID_CHAT_MESSAGE:
			{
				netCommand4G_ChatMessage nc_ChatMessage(in_ClientBuf);
				gameShell->addStringToChatWindow(nc_ChatMessage.clanOnly, nc_ChatMessage.text, nc_ChatMessage.locale);
			}
			break;
        case NETCOM_4C_ID_DESYNC_NOTIFY: {
            netCommand4C_DesyncNotify nc(in_ClientBuf);
            fprintf(stderr, "NETCOM_4C_ID_DESYNC_NOTIFY\n");
            
            
            if (nc.data.desync_amount > PNC_DESYNC_RESTORE_ATTEMPTS) {
                ExecuteInterfaceCommand(PNC_INTERFACE_COMMAND_DESYNC);
            } else {                
                if (!isHost()) {
                    ExecuteInternalCommand(PNC_COMMAND__DESYNC, false);
                }
                
                gameShell->MultiplayerGameDesyncNotify(nc.data);
            }
            break;
        }
        case NETCOM_4C_ID_DESYNC_RESTORE: {
            //Load the mission state sent by host
            netCommand4C_DesyncRestore nc(in_ClientBuf);
            clientMissionDescription=nc.missionDescription.release();

            if (!isHost()) {
                CAutoLock _lock(m_GeneralLock); //! Lock   

                ClearInputPacketList();
                clearInOutClientHostBuffers();
            }
            
            {
                CAutoLock _lock(m_GeneralLock); //! Lock
                gameShell->MultiplayerGameRestore(*clientMissionDescription);
            }

            clientMissionDescription->gameType_ = GT_MULTI_PLAYER_LOAD;
        
            LogMsg("Desync Restore attempt %d\n", nc.desync_amount);

            break;
        }
		default: 
			{
				/* !!! передается HiperSpace
					SetConnectionTimeout(30000);
					int c=GetConnectionTimeout();
				*/
				if(flag_SkipProcessingGameCommand) in_ClientBuf.ignoreNetCommand(); //Нужно при миграции Host-а
				else {
					if(gameShell->GameActive){
//						if(in_ClientBuf.getQuantAmount()>=1){ //Отсылать на выполнение только ПОЛНОСТЬЮ законченный квант!!!
							if(universe()->ReceiveEvent(event, in_ClientBuf)==false) {
								in_ClientBuf.backNetCommand();
								goto loc_end_quant;
							}
//						}
					}
					else {
						in_ClientBuf.ignoreNetCommand();
					}
				}
			}
			break;
		}
		in_ClientBuf.nextNetCommand();
	}
loc_end_quant:
	;
}

void PNetCenter::P2PIQuant()
{
	sPNCInterfaceCommand curInterfaceCommand(PNC_INTERFACE_COMMAND_NONE);
	{
		CAutoLock p_Lock(m_GeneralLock);
		if(!interfaceCommandList.empty()){
			////Сейчас сделано так, что нельзя поместить команду если другая выполняется
            sPNCInterfaceCommand* cmd = *interfaceCommandList.begin();
			interfaceCommandList.pop_front();
            curInterfaceCommand.icID=cmd->icID;
            curInterfaceCommand.text=std::move(cmd->text);
            delete cmd;
        }

        if (!isConnected()) {
            serverList->refreshHostInfoList();
        }

		HandlerInputNetCommand();
	}

//	void playerDisconnected(string& playerName, bool disconnectOrExit);

    bool end_game = false;
	switch(curInterfaceCommand.icID){
	case PNC_INTERFACE_COMMAND_NONE:
		break;
	case PNC_INTERFACE_COMMAND_INFO_PLAYER_DISCONNECTED:
    case PNC_INTERFACE_COMMAND_INFO_PLAYER_EXIT: {
        bool disconnected = curInterfaceCommand.icID == PNC_INTERFACE_COMMAND_INFO_PLAYER_DISCONNECTED;
        const char* res = qdTextDB::instance().getText(disconnected ? "Interface.Menu.Messages.PlayersDisconnected"
                                                                    : "Interface.Menu.Messages.PlayersExited");
        const int bufferSize = 500;
        static char tempBuffer[bufferSize];
        std::string name = curInterfaceCommand.text->text + "&FFFFFF";
        snprintf(tempBuffer, bufferSize, res, name.c_str());
        curInterfaceCommand.text->text = tempBuffer;
        gameShell->serverMessage(curInterfaceCommand.text.get());
        break;
    }
    case PNC_INTERFACE_COMMAND_INFO_MESSAGE:
        gameShell->serverMessage(curInterfaceCommand.text.get());
        break;
	case PNC_INTERFACE_COMMAND_CONNECTION_FAILED:
		//gameShell->abnormalNetCenterTermination();
		gameShell->generalErrorOccured(GameShell::GENERAL_CONNECTION_FAILED);
        end_game = true;
		break;
	case PNC_INTERFACE_COMMAND_KICKED:
		gameShell->generalErrorOccured(GameShell::KICKED);
        end_game = true;
		break;
	case PNC_INTERFACE_COMMAND_HOST_TERMINATED_GAME:
		gameShell->generalErrorOccured(GameShell::HOST_TERMINATED);
        end_game = true;
		break;
    case PNC_INTERFACE_COMMAND_DESYNC:
        gameShell->generalErrorOccured(GameShell::DESYNC);
        gameShell->CurrentMission.PrintInfo();
        break;
	case PNC_INTERFACE_COMMAND_CRITICAL_ERROR_GAME_TERMINATED:
		xassert(0&& "Host stoping, game ending");
		gameShell->generalErrorOccured(GameShell::GENERAL_CONNECTION_FAILED);
        gameShell->CurrentMission.PrintInfo();
        end_game = true;
        break;
	}

    if (end_game && m_bStarted) {
        ExecuteInternalCommand(PNC_COMMAND__RESET, true);
    }

//	if(m_state==PNC_STATE__NET_CENTER_CRITICAL_ERROR) {
//		const unsigned int TIMEOUT=5000;// ms
//		if( WaitForSingleObject(hSecondThread, TIMEOUT) != WAIT_OBJECT_0) {
//			xassert(0&&"Net Thread terminated!!!");
//			TerminateThread(hSecondThread, 0);
//		}
//		ErrH.Exit();
//	}

	/// Pause
	if( (!clientPause) && m_bStarted){
		if( gameShell->GameActive /*&& (!isHost())*/ ){
			if(clientInPacketPause){
				if(clocki() <= lastTimeServerPacket+TIMEOUT_CLIENT_OR_SERVER_RECEIVE_INFORMATION){
					gameShell->hideConnectFailedInGame();
					clientInPacketPause=false;
				}
			}
			else if(clocki() > lastTimeServerPacket+TIMEOUT_CLIENT_OR_SERVER_RECEIVE_INFORMATION){
				std::string s;
				clientMissionDescription->getAllOtherPlayerName(s);
				gameShell->showConnectFailedInGame(s);
				clientInPacketPause=true;
			}
		}
	}

}


//first thread !
void PNetCenter::ServerTimeControl(float scale)
{
	terEventControlServerTime event(scale);
	SendEvent(&event);
}

void PNetCenter::changePlayerBelligerent(int idxPlayerData, terBelligerent newBelligerent)
{
	netCommand4H_ChangePlayerBelligerent nc_ChPB(idxPlayerData, newBelligerent);
	SendEvent(&nc_ChPB);
}
void PNetCenter::changePlayerColor(int idxPlayerData, int newColorIndex, bool direction)
{
	netCommand4H_ChangePlayerColor nc_ChPC(idxPlayerData, newColorIndex, direction);
	SendEvent(&nc_ChPC);
}

void PNetCenter::changeRealPlayerType(int idxPlayerData, RealPlayerType newRealPlayerType)
{
	netCommand4H_ChangeRealPlayerType nc_ChRPT(idxPlayerData, newRealPlayerType);
	SendEvent(&nc_ChRPT);
}

void PNetCenter::changePlayerDifficulty(int idxPlayerData, Difficulty difficulty)
{
	netCommand4H_ChangePlayerDifficulty nc_ChD(idxPlayerData, difficulty);
	SendEvent(&nc_ChD);
}
void PNetCenter::changePlayerClan(int idxPlayerData, int clan)
{
	netCommand4H_ChangePlayerClan nc_ChC(idxPlayerData, clan);
	SendEvent(&nc_ChC);
}
void PNetCenter::changePlayerHandicap(int idxPlayerData, int handicap)
{
	netCommand4H_ChangePlayerHandicap nc_ChH(idxPlayerData, handicap);
	SendEvent(&nc_ChH);
}
void PNetCenter::changePlayerSeat(int idxPlayerData)
{
    netCommand4H_ChangePlayerSeat nc_ChS(idxPlayerData);
    SendEvent(&nc_ChS);
}

void PNetCenter::chatMessage(bool clanOnly, const std::string& text, const std::string& locale)
{
	netCommand4G_ChatMessage nc_ChatMessage(clanOnly, text, locale);
	SendEvent(&nc_ChatMessage);
}

void PNetCenter::changeMap(const char* missionName)
{
	MissionDescription md(missionName, GT_MULTI_PLAYER_CREATE);
	netCommand4H_ChangeMap nc_changeMap(md);
	SendEvent(&nc_changeMap);
}

//void PNetCenter::pauseQuant(bool flag_gameIsGo)
//{
//
//}

bool PNetCenter::setPause(bool pause)
{
	if(m_bStarted){
		CAutoLock p_Lock(m_GeneralLock);
		if(pause){
			netCommand4H_RequestPause nc_rp(clientMissionDescription->activePlayerID, true);
			SendEvent(&nc_rp);
		}
		else {
			netCommand4H_RequestPause nc_rp(clientMissionDescription->activePlayerID, false);
			SendEvent(&nc_rp);
		}
		return 1;
	}
	return 0;
}



void PNetCenter::StartLoadTheGame(bool state)
{
	netCommand4H_StartLoadGame ncslg(state);
	if(isConnected()) {
		SendEventSync(&ncslg);
	}
}

void PNetCenter::GameIsReady()
{
    //Clear MD data in case we restored it
    clientMissionDescription->clearData();
	netCommandC_PlayerReady event2(vMap.getWorldCRC());
	SendEventSync(&event2);
    if (!isHost()) {
        ExecuteInternalCommand(PNC_COMMAND__CLIENT_GAME_IS_READY, false);
    }
}

const std::vector<GameInfo>& PNetCenter::getGameList() const {
	return serverList->getList();
}

/////////////////////////////////////////////////////////////////
void PNetCenter::Reset()
{
    ExecuteInternalCommand(PNC_COMMAND__RESET, true);
}
