#include "NetIncludes.h"

#include "P2P_interface.h"

#include <SDL.h>

#include "GameShell.h"
#include "Universe.h"

#include "../Terra/terra.h"

#include <algorithm>

const int NORMAL_QUANT_INTERVAL=100;

char* terCurrentServerName; //Из ini файла

const char* PNetCenter::getStrState()
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

bool PNetCenter::resolveHostAddress(GameHostConnection& address, const std::string& host) {
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
    }
    int ret = SDLNet_ResolveHost(&address.host_ip, ip.c_str(), port) == 0;
    if (ret < 0 || address.host_ip.host == INADDR_NONE) {
        fprintf(stderr, "Error resolving host %s: %s\n", host.c_str(), SDLNet_GetError());
        return false;
    }
    return true;
}

PNetCenter::PNetCenter()
	: in_ClientBuf(1000000, true), out_ClientBuf(1000000, true), in_HostBuf(1000000, true), out_HostBuf(1000000, true)
{
    publicServerHost=false;
    hostConnection=GameHostConnection();
	
    m_hostNETID=0;
    m_localNETID=0;
	
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

	//hSecondThread=INVALID_HANDLE_VALUE;

    SDL_Thread* thread = SDL_CreateThread(InternalServerThread, "perimeter_server_thread", this);
    if (thread == nullptr) {
        SDL_FATAL_ERROR("SDL_CreateThread perimeter_server_thread failed");
    }
    SDL_DetachThread(thread);
    hSecondThread = CreateEvent(0, true, false, 0);

	if(WaitForSingleObject(hSecondThreadInitComplete, INFINITE) != WAIT_OBJECT_0) {
		xassert(0&&"NetCenter:Error second thread init");
		ErrH.Abort("Network: General error 1!");
	}

	lastTimeServerPacket=clocki();

    LogMsg("Created PNetCenter\n");
}


PNetCenter::~PNetCenter()
{
	ExecuteInternalCommand(PNC_COMMAND__END, true);
	const unsigned int TIMEOUT=5000;// ms
	if( WaitForSingleObject(hSecondThread, TIMEOUT) != WAIT_OBJECT_0) {
		xassert(0&&"Net Thread terminated!!!");
        SetEvent(hSecondThread); //TODO not sure if this even necessary
	}

	ClearDeletePlayerGameCommand();

	ClearClients();

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	m_DPPacketList.clear();



	SDL_DestroyMutex(m_GeneralLock);
	///CloseHandle(hServerReady);
	///CloseHandle(hStartServer);

    DestroyEvent(hSecondThreadInitComplete);
    DestroyEvent(hCommandExecuted);
}

//////////////////////////////////////////////
/// Current game info getters

const char* PNetCenter::getMissionName() {
	if (!isHost()) return ""; 
    return hostMissionDescription.missionName();
}

const char* PNetCenter::getGameName() {
	if (!isHost()) return ""; 
    return m_GameName.c_str();
}

const char* PNetCenter::getGameVer() {
	return SIMPLE_GAME_CURRENT_VERSION;
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
    return hostConnection.host_ip.port;
}

//////////////////////////////////////////////
/// New network game creation or joining

void PNetCenter::CreateGame(const GameHostConnection& connection, const std::string& gameName, const std::string& missionName, const std::string& playerName, const std::string& password)
{
    LogMsg("Create Game\n");
	clientPause=false;
	clientInPacketPause=false;
    hostConnection = connection;
    gamePassword = password;

	m_quantInterval=NORMAL_QUANT_INTERVAL; //round((float)NORMAL_QUANT_INTERVAL/gameSpeed);

	MissionDescription md(missionName.c_str(), GT_createMPGame);

	hostMissionDescription=md; //Argument PNC_COMMAND__START_HOST_AND_CREATE_GAME_AND_STOP_FIND_HOST
	m_GameName = gameName;
	internalPlayerData.set(PlayerData::PLAYER_ID_NONE, playerName);
	ExecuteInternalCommand(PNC_COMMAND__START_HOST_AND_CREATE_GAME_AND_STOP_FIND_HOST, true);

    gameShell->callBack_CreateGameReturnCode(isConnected() ? GameShell::CG_RC_OK : GameShell::CG_RC_CREATE_HOST_ERR);
}

void PNetCenter::JoinGame(const GameHostConnection& connection, const std::string& playerName, const std::string& password)
{
	LogMsg("Join Game\n");
	clientPause = false;
	clientInPacketPause=false;
    hostConnection = connection;
    gamePassword = password;

	//Argument  PNC_COMMAND__CONNECT_2_HOST_AND_STOP_FIND_HOST
	internalPlayerData.set(PlayerData::PLAYER_ID_NONE, playerName);
	//internalIP установлен ранее
	ExecuteInternalCommand(PNC_COMMAND__CONNECT_2_HOST_AND_STOP_FIND_HOST, false /*true*/);
//	if(isConnected()) {
//		LogMsg("connect Ok\n");
//		gameShell->callBack_JoinGameReturnCode(GameShell::JG_RC_OK);
//		return;// JGRC_OK;
//	}
//	else {
//		LogMsg("connect Error\n");
//		gameShell->callBack_JoinGameReturnCode(GameShell::JG_RC_CONNECTION_ERR);
//		return;// JGRC_ERR_CONNECTION;
//	}
}

//////////////////////////////////////////////

// first thread function !
void PNetCenter::SendEvent(const netCommandGeneral* event)
{
	SendEventSync(event);
	///out_buffer.send(m_connection);
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
				netCommand4C_JoinResponse ncjr(in_ClientBuf);
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
				curMD=ncCMD.missionDescription_;
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
				gameShell->addStringToChatWindow(nc_ChatMessage.userID, nc_ChatMessage.getText());
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
	//in_buffer.receive(m_connection, XDP_NETID_PLAYER_GENERAL);

	sPNCInterfaceCommand curInterfaceCommand(PNC_INTERFACE_COMMAND_NONE);
	{
		CAutoLock p_Lock(m_GeneralLock);
		if(!interfaceCommandList.empty()){
			////Сейчас сделано так, что нельзя поместить команду если другая выполняется
			curInterfaceCommand=*interfaceCommandList.begin();
			interfaceCommandList.pop_front();
		}

		refreshLanGameHostList();

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
		//gameShell->abortLobby(GameShell::CLIENT_DROPPED);
		gameShell->generalErrorOccured(GameShell::HOST_TERMINATED);
		ExecuteInternalCommand(PNC_COMMAND__END_GAME, true);
		break;
	case PNC_INTERFACE_COMMAND_HOST_TERMINATED_GAME:
		gameShell->generalErrorOccured(GameShell::HOST_TERMINATED);
		ExecuteInternalCommand(PNC_COMMAND__END_GAME, true);
		break;

	case PNC_INTERFACE_COMMAND_CONNECT_OK:
		gameShell->callBack_JoinGameReturnCode(GameShell::JG_RC_OK);
		break;
	case PNC_INTERFACE_COMMAND_CONNECT_ERR_INCORRECT_VERSION:
		gameShell->callBack_JoinGameReturnCode(GameShell::JG_RC_GAME_NOT_EQUAL_VERSION_ERR);
		break;
	case PNC_INTERFACE_COMMAND_CONNECT_ERR_GAME_STARTED:
		gameShell->callBack_JoinGameReturnCode(GameShell::JG_RC_GAME_IS_RUN_ERR);
		break;
	case PNC_INTERFACE_COMMAND_CONNECT_ERR_GAME_FULL:
		gameShell->callBack_JoinGameReturnCode(GameShell::JG_RC_GAME_IS_FULL_ERR);
		break;
	case PNC_INTERFACE_COMMAND_CONNECT_ERR_PASSWORD:
		gameShell->callBack_JoinGameReturnCode(GameShell::JG_RC_PASSWORD_ERR);
		break;
	case PNC_INTERFACE_COMMAND_CONNECT_ERR:
		gameShell->callBack_JoinGameReturnCode(GameShell::JG_RC_CONNECTION_ERR);
		break;

	case PNC_INTERFACE_COMMAND_CRITICAL_ERROR_GAME_TERMINATED:
		xassert(0&& "Host stoping, game ending");
		gameShell->generalErrorOccured(GameShell::GENERAL_CONNECTION_FAILED);
		ExecuteInternalCommand(PNC_COMMAND__END_GAME, true);
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
void PNetCenter::changePlayerColor(int idxPlayerData, int newColorIndex)
{
	netCommand4H_ChangePlayerColor nc_ChPC(idxPlayerData, newColorIndex);
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

void PNetCenter::chatMessage(int userID, const char* str)
{
	netCommand4G_ChatMessage nc_ChatMessage(userID, str);
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



bool PNetCenter::StartLoadTheGame()
{
	netCommand4H_StartLoadGame ncslg;
	if(isConnected()) {
		SendEventSync(&ncslg);
	}
	return 1;
}

void PNetCenter::GameIsReady(void)
{
	netCommandC_PlayerReady event2(vMap.getWorldCRC());
	SendEventSync(&event2);
}


std::vector<sGameHostInfo*>& PNetCenter::getGameHostList()
{
	return gameHostList;
}

void PNetCenter::refreshLanGameHostList()
{
	clearGameHostList();
	char txtBufHostName[MAX_PATH];
	char txtBufGameName[MAX_PATH];
	//char textBuffer[MAX_PATH];
	int curTime=clocki();

    /*
	std::vector<INTERNAL_HOST_ENUM_INFO*>::iterator p;
	for(p=internalFoundHostList.begin(); p!=internalFoundHostList.end();){
		if((curTime - (*p)->timeLastRespond) > MAX_TIME_INTERVAL_HOST_RESPOND ) {
			delete *p;
			p=internalFoundHostList.erase(p);
		}
		else {
			IDirectPlay8Address*  pa=(*p)->pHostAddr;
			HRESULT hResult;

/*			DWORD sizeTxtBuf=MAX_PATH;
			WCHAR txtBuf[MAX_PATH];
			DWORD sizeTxtBuf2=MAX_PATH;
			char txtBuf2[MAX_PATH];
			DWORD data;

			DWORD numComponent;
			hResult = pa->GetNumComponents(&numComponent);
			int i;
			const TCHAR * arr;
			for(i=0; i<numComponent; i++){
				DWORD sizeTxtBuf=MAX_PATH;
				DWORD sizeTxtBuf2=MAX_PATH;
				hResult=pa->GetComponentByIndex(i, txtBuf, &sizeTxtBuf, txtBuf2, &sizeTxtBuf2, &data);
				arr=DXGetErrorString9(hResult);
			}*/
/*
			char * pBuf;
			DWORD bufSize;
			DWORD dataType;
			pBuf=NULL; bufSize=0;
			hResult=pa->GetComponentByName(DPNA_KEY_HOSTNAME, pBuf, &bufSize, &dataType);
			xassert(hResult==DPNERR_BUFFERTOOSMALL);
			pBuf=new char[bufSize];
			hResult=pa->GetComponentByName(DPNA_KEY_HOSTNAME, pBuf, &bufSize, &dataType);
			if( FAILED(hResult) ){
				DXTRACE_ERR_MSGBOX( TEXT("PNetCenter::refreshLanGameHostList-GetComponentByName"), hResult );
			}
			xassert(dataType==DPNA_DATATYPE_STRING);
			int nResult = WideCharToMultiByte( CP_ACP, 0, (WCHAR*)pBuf, -1, txtBufHostName, MAX_PATH, NULL, NULL );
			txtBufHostName[MAX_PATH-1]=0;
			delete pBuf;

			pBuf=NULL; bufSize=0;
			hResult=pa->GetComponentByName(DPNA_KEY_PORT, pBuf, &bufSize, &dataType);
			xassert(hResult==DPNERR_BUFFERTOOSMALL);
			pBuf=new char[bufSize];
			hResult=pa->GetComponentByName(DPNA_KEY_PORT, pBuf, &bufSize, &dataType);
			xassert(dataType==DPNA_DATATYPE_DWORD);
			if( FAILED(hResult) ){
				DXTRACE_ERR_MSGBOX( TEXT("PNetCenter::refreshLanGameHostList-GetComponentByName"), hResult );
			}
			DWORD port=*((DWORD*)pBuf);
			delete pBuf;

			std::string txtBufPort = std::to_string(port);
			
			nResult = WideCharToMultiByte( CP_ACP, 0, (*p)->pAppDesc->pwszSessionName, -1, txtBufGameName, MAX_PATH, NULL, NULL );
			txtBufGameName[MAX_PATH-1]=0;

			//поиск среди первых необходимых hostName-ов
			//vector<sGameHostInfo*>::iterator m;
			int m;
			for(m=0; m<needHostList.size(); m++){
				unsigned long ipSrc=inet_addr(gameHostList[m]->hostName.c_str());
				if(ipSrc==INADDR_NONE) continue;
				unsigned long ip2=inet_addr(txtBufHostName);
				if(ip2==INADDR_NONE) continue;
				if(ipSrc==ip2) break;
			}
			if(m!=needHostList.size()){ //адрес нашелся - m индекс
				delete gameHostList[m];
				gameHostList[m]=new sGameHostInfo( (*p)->pAppDesc->guidInstance, txtBufHostName, txtBufPort.c_str(), txtBufGameName, (*p)->gameStatusInfo);
			}
			else { //вставляем
				gameHostList.push_back(new sGameHostInfo( (*p)->pAppDesc->guidInstance, txtBufHostName, txtBufPort.c_str(), txtBufGameName, (*p)->gameStatusInfo));//sGameStatusInfo(4,1, false, 10, 1)
			}

			p++;
		}
	}
 */
}




/////////////////////////////////////////////////////////////////
void PNetCenter::FinishGame(void)
{
	ExecuteInternalCommand(PNC_COMMAND__END_GAME, true);
}

void PNetCenter::StartFindHost(void)
{
	ExecuteInternalCommand(PNC_COMMAND__START_FIND_HOST, true);
}

void PNetCenter::DisconnectAndStartFindHost(void)
{
	ExecuteInternalCommand(PNC_COMMAND__DISCONNECT_AND_ABORT_GAME_AND_END_START_FIND_HOST, true);
}

void PNetCenter::StopServerAndStartFindHost(void)
{
	ExecuteInternalCommand(PNC_COMMAND__STOP_HOST_AND_ABORT_GAME_AND_START_FIND_HOST, true);
}
