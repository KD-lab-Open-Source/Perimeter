#include "NetIncludes.h"

#include "P2P_interface.h"

#include <SDL.h>

#include "GameShell.h"
#include "Universe.h"

#include <algorithm>

extern const char* currentShortVersion;

const int NORMAL_QUANT_INTERVAL=100;

SDL_threadID net_thread_id=-1;

const char* PNetCenter::getStrState() const
{
	switch(m_state){
	case		PNC_STATE__NONE:
		return("PNC_STATE__NONE");
	case		PNC_STATE__CLIENT_FIND_HOST:
		return("PNC_STATE__CLIENT_FIND_HOST");
	case		PNC_STATE__CONNECTION:
		return("PNC_STATE__CONNECTION");
	case		PNC_STATE__CLIENT_TUNING_GAME:
		return("PNC_STATE__CLIENT_TUNING_GAME");
	case		PNC_STATE__CLIENT_LOADING_GAME:
		return("PNC_STATE__CLIENT_LOADING_GAME");
	case		PNC_STATE__CLIENT_GAME:
		return("PNC_STATE__CLIENT_GAME");

	case		PNC_STATE__CLIENT_RESTORE_GAME_AFTE_CHANGE_HOST_PHASE_0:
		return("PNC_STATE__CLIENT_RESTORE_GAME_AFTE_CHANGE_HOST_PHASE_0");
	case		PNC_STATE__CLIENT_RESTORE_GAME_AFTE_CHANGE_HOST_PHASE_AB:
		return("PNC_STATE__CLIENT_RESTORE_GAME_AFTE_CHANGE_HOST_PHASE_AB");

	case		PNC_STATE__END:
		return("PNC_STATE__END");
	case		PNC_STATE__NET_CENTER_CRITICAL_ERROR:
		return("PNC_STATE__NET_CENTER_CRITICAL_ERROR");
	case		PNC_STATE__HOST_TUNING_GAME:
		return("PNC_STATE__HOST_TUNING_GAME");
	case		PNC_STATE__HOST_LOADING_GAME:
		return("PNC_STATE__HOST_LOADING_GAME");
	case		PNC_STATE__HOST_GAME:
		return("PNC_STATE__HOST_GAME");

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
in_ClientBuf(1000000, true), out_ClientBuf(1000000, true),
in_HostBuf(1000000, true), out_HostBuf(1000000, true),
connectionHandler(this)
{
    publicServerHost=false;
    hostConnection=NetAddress();

    m_hostNETID = m_localNETID = NETID_NONE;
	
    flag_connected=false;
	flag_NetworkSimulation=false;
	if(IniManager("Network.ini").getInt("General", "NetworkSimulator")) flag_NetworkSimulation=true;
	flag_HostMigrate=false;
	if(IniManager("Network.ini").getInt("General", "HostMigrate")) flag_HostMigrate=true;
	flag_NoUseDPNSVR=false;
	if(IniManager("Network.ini").getInt("General", "NoUseDPNSVR")) flag_NoUseDPNSVR=true;

	m_DPSigningLevel=0;
	m_DPSigningLevel=IniManager("Network.ini").getInt("General", "DPSigningLevel");


	m_clients.reserve(NETWORK_PLAYERS_MAX);

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


	flag_LockIputPacket=0;
	flag_SkipProcessingGameCommand=0;
	hostPause=0;
	clientPause=0;
	clientInPacketPause=0;

	m_quantInterval=NORMAL_QUANT_INTERVAL;

	
	TIMEOUT_CLIENT_OR_SERVER_RECEIVE_INFORMATION=5000;//5s
	TIMEOUT_CLIENT_OR_SERVER_RECEIVE_INFORMATION=IniManager("Network.ini").getInt("General","TimeOutClientOrServerReceive");

	TIMEOUT_DISCONNECT=600000;
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
    SDL_Thread* thread = SDL_CreateThread(InternalServerThread, "perimeter_server_thread", this);
    if (thread == nullptr) {
        SDL_FATAL_ERROR("SDL_CreateThread perimeter_server_thread failed");
    }
    SDL_DetachThread(thread);

	if(WaitForSingleObject(hSecondThreadInitComplete, INFINITE) != WAIT_OBJECT_0) {
		xassert(0&&"NetCenter:Error second thread init");
		ErrH.Abort("Network: General error 1!");
	}
    xassert(net_thread_id == SDL_GetThreadID(thread));

	lastTimeServerPacket=clocki();

    LogMsg("Created PNetCenter\n");
}


PNetCenter::~PNetCenter()
{
	ExecuteInternalCommand(PNC_COMMAND__END, true);
	const unsigned int TIMEOUT=5000;// ms
	if( WaitForSingleObject(hSecondThread, TIMEOUT) != WAIT_OBJECT_0) {
        LogMsg("Net Thread terminated!!!\n");
		xassert(0);
        SetEvent(hSecondThread); //TODO not sure if this even necessary
	}

	ClearDeletePlayerGameCommand();

	ClearClients();

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ClearInputPacketList();



	SDL_DestroyMutex(m_GeneralLock);
	///CloseHandle(hServerReady);
	///CloseHandle(hStartServer);

    DestroyEvent(hSecondThreadInitComplete);
    DestroyEvent(hCommandExecuted);

    LogMsg("Destroyed PNetCenter\n");
}

//////////////////////////////////////////////
/// Current game info getters

const char* PNetCenter::getMissionName() {
	if (!isHost()) return ""; 
    return hostMissionDescription.missionName().c_str();
}

const char* PNetCenter::getGameName() {
	if (!isHost()) return ""; 
    return m_GameName.c_str();
}

const char* PNetCenter::getGameVer() {
	return currentShortVersion;
}

int PNetCenter::getNumPlayers() {
	if(!isHost()) return 0; 
    return hostMissionDescription.playersAmount();
}

int PNetCenter::getMaxPlayers() {
	if(!isHost()) return 0; 
    return hostMissionDescription.playersMaxEasily();//playersAmountScenarioMax();
}

int PNetCenter::getHostPort() {
	if(!isHost()) return 0; 
    return hostConnection.addr.port;
}

//////////////////////////////////////////////
/// New network game creation or joining

void PNetCenter::CreateGame(const NetAddress& connection, const std::string& gameName, const std::string& missionName, const std::string& playerName, const std::string& password)
{
    LogMsg("Create Game\n");
	clientPause=false;
	clientInPacketPause=false;
    hostConnection = connection;
    gamePassword = password;

	m_quantInterval=NORMAL_QUANT_INTERVAL; //round((float)NORMAL_QUANT_INTERVAL/gameSpeed);

    //Argument PNC_COMMAND__START_HOST_AND_CREATE_GAME_AND_STOP_FIND_HOST
	hostMissionDescription = MissionDescription(missionName.c_str(), GT_createMPGame);
	m_GameName = gameName;
	m_PlayerName = playerName;
	ExecuteInternalCommand(PNC_COMMAND__START_HOST_AND_CREATE_GAME_AND_STOP_FIND_HOST, true);

    gameShell->callBack_CreateGameReturnCode(isConnected() ? GameShell::CG_RC_OK : GameShell::CG_RC_CREATE_HOST_ERR);
}

void PNetCenter::JoinGame(const NetAddress& connection, const std::string& playerName, const std::string& password)
{
	LogMsg("Join Game\n");
	clientPause = false;
	clientInPacketPause=false;
    hostConnection = connection;
    gamePassword = password;

	//Argument  PNC_COMMAND__CONNECT_2_HOST_AND_STOP_FIND_HOST
	m_PlayerName = playerName;
	//internalIP установлен ранее
	ExecuteInternalCommand(PNC_COMMAND__CONNECT_2_HOST_AND_STOP_FIND_HOST, false);
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
		case NETCOM_4C_ID_JOIN_RESPONSE:
			{
				//netCommand4C_JoinResponse ncjr(in_ClientBuf);
				//m_playerNETID=ncjr.playerNETID_;
			}
			break;
		case NETCOM_4C_ID_START_LOAD_GAME:
			{
				netCommand4C_StartLoadGame nc4c_sl(in_ClientBuf);
				clientMissionDescription=nc4c_sl.missionDescription_;

				m_bStarted = true;

				///if(!isHost()) ExecuteInternalCommand(PNC_COMMAND__CLIENT_STARTING_LOAD_GAME, false);
				//game_shell.GameStart(ncbc.missionDescription_);
                gameShell->MultiplayerGameStart(clientMissionDescription);
			}
			break;
        case NETCOM_4G_ID_EXIT: {
            netCommand4G_Exit nc4g_exit(in_ClientBuf);
            ExitClient(nc4g_exit.netid);
            break;
        }
		case NETCOM_4C_ID_PAUSE:
			{
				netCommand4C_Pause ncp(in_ClientBuf);
				if(ncp.pause){
					std::string s;
					for(int i=0; i<NETWORK_PLAYERS_MAX; i++){
						if(ncp.playersIDArr[i]!=netCommand4C_Pause::NOT_PLAYER_ID) {
							clientMissionDescription.getPlayerName(ncp.playersIDArr[i], s);
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
		case NETCOM_4C_ID_ALIFE_PACKET:
			{
				netCommand4C_AlifePacket nc(in_ClientBuf);
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
				gameShell->addStringToChatWindow(nc_ChatMessage.text, nc_ChatMessage.locale);
			}
			break;
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
			curInterfaceCommand=*interfaceCommandList.begin();
			interfaceCommandList.pop_front();
		}

		serverList.refreshHostInfoList();

		HandlerInputNetCommand();
	}

//	void playerDisconnected(string& playerName, bool disconnectOrExit);

	switch(curInterfaceCommand.icID){
	case PNC_INTERFACE_COMMAND_NONE:
		break;
	case PNC_INTERFACE_COMMAND_INFO_PLAYER_DISCONNECTED:
		gameShell->playerDisconnected(curInterfaceCommand.textInfo, true);
		break;
	case PNC_INTERFACE_COMMAND_INFO_PLAYER_EXIT:
		gameShell->playerDisconnected(curInterfaceCommand.textInfo, false);
		break;
	case PNC_INTERFACE_COMMAND_CONNECTION_FAILED:
		//gameShell->abnormalNetCenterTermination();
		gameShell->generalErrorOccured(GameShell::GENERAL_CONNECTION_FAILED);
		ExecuteInternalCommand(PNC_COMMAND__END_GAME, true);
		//ExecuteInternalCommand(PNC_COMMAND__END, true);
		break;
	case PNC_INTERFACE_COMMAND_CONNECTION_DROPPED:
		gameShell->generalErrorOccured(GameShell::CLIENT_DROPPED);
		ExecuteInternalCommand(PNC_COMMAND__END_GAME, true);
		break;
	case PNC_INTERFACE_COMMAND_HOST_TERMINATED_GAME:
		gameShell->generalErrorOccured(GameShell::HOST_TERMINATED);
		ExecuteInternalCommand(PNC_COMMAND__END_GAME, true);
		break;
    case PNC_INTERFACE_COMMAND_DESYNC:
        gameShell->generalErrorOccured(GameShell::DESYNC);
        ExecuteInternalCommand(PNC_COMMAND__END_GAME, true);
        break;
	case PNC_INTERFACE_COMMAND_CRITICAL_ERROR_GAME_TERMINATED:
		xassert(0&& "Host stoping, game ending");
		gameShell->generalErrorOccured(GameShell::GENERAL_CONNECTION_FAILED);
		ExecuteInternalCommand(PNC_COMMAND__END_GAME, true);
        break;
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
	if( (!clientPause) && m_state!=PNC_STATE__NONE){
		if( gameShell->GameActive /*&& (!isHost())*/ ){
			if(clientInPacketPause){
				if(clocki() <= lastTimeServerPacket+TIMEOUT_CLIENT_OR_SERVER_RECEIVE_INFORMATION){
					gameShell->hideConnectFailedInGame();
					clientInPacketPause=false;
				}
			}
			else if(clocki() > lastTimeServerPacket+TIMEOUT_CLIENT_OR_SERVER_RECEIVE_INFORMATION){
				std::string s;
				clientMissionDescription.getAllOtherPlayerName(s);
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

void PNetCenter::chatMessage(bool clanOnly, const std::string& text, const std::string& locale)
{
	netCommand4G_ChatMessage nc_ChatMessage(clanOnly, text, locale);
	SendEvent(&nc_ChatMessage);
}

void PNetCenter::changeMap(const char* missionName)
{
	MissionDescription md(missionName, GT_createMPGame);
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
			netCommand4H_RequestPause nc_rp(clientMissionDescription.activePlayerID, true);
			SendEvent(&nc_rp);
		}
		else {
			netCommand4H_RequestPause nc_rp(clientMissionDescription.activePlayerID, false);
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
	netCommandC_PlayerReady event2(vMap.getWorldCRC());
	SendEventSync(&event2);
}

std::vector<GameHostInfo>& PNetCenter::getGameHostList()
{
	return serverList.gameHostInfoList;
}

/////////////////////////////////////////////////////////////////
void PNetCenter::FinishGame()
{
	ExecuteInternalCommand(PNC_COMMAND__END_GAME, true);
}

void PNetCenter::StartFindHost()
{
	ExecuteInternalCommand(PNC_COMMAND__START_FIND_HOST, true);
}

void PNetCenter::DisconnectAndStartFindHost()
{
	ExecuteInternalCommand(PNC_COMMAND__DISCONNECT_AND_ABORT_GAME_AND_END_START_FIND_HOST, true);
}

void PNetCenter::StopServerAndStartFindHost()
{
	ExecuteInternalCommand(PNC_COMMAND__STOP_HOST_AND_ABORT_GAME_AND_START_FIND_HOST, true);
}
