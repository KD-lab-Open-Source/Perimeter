#include "StdAfx.h"

#include "Runtime.h"
#include "P2P_interface.h"
#include "GS_interface.h"

#include "GameShell.h"
#include "Universe.h"

#include "Lmcons.h"

#include "..\terra\terra.h"

#include <algorithm>



const unsigned int MAX_TIME_WAIT_RESTORE_GAME_AFTER_MIGRATE_HOST=10000;//10sec


void LogMsg(LPCTSTR fmt, ...)
{
#ifndef _FINAL_VERSION_
	va_list val;
	va_start(val, fmt);

	DWORD written;
	TCHAR buf[255];
	wvsprintf(buf, fmt, val);

	static const COORD _80x50 = {80,500};
	static BOOL startup = (AllocConsole(), SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), _80x50));
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), buf, lstrlen(buf), &written, 0);
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
PClientData::PClientData(unsigned int mdIdx, DPNID dpnid, const char* descr)// : out_buffer(1000000), in_buffer(1000000)
{
	missionDescriptionIdx=mdIdx;
//	m_pGame = game;
	dpnidPlayer = dpnid;
	strncpy(m_szDescription, descr, PERIMETER_CLIENT_DESCR_SIZE);
	///m_hReady = CreateEvent(0, TRUE, FALSE, 0);
	m_flag_Ready=0;
//	InitializeCriticalSection(&m_csLock);
	lagQuant=0;
	lastExecuteQuant=0;
	curLastQuant=0;
	lastTimeBackPacket=clocki();
	confirmQuant=0;

	requestPause=0;
	clientPause=0;
	timeRequestPause=0;

}
PClientData::~PClientData()
{
	///CloseHandle(m_hReady);
//	DeleteCriticalSection(&m_csLock);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////

//XBuffer exchangeBuff(1024);

//const char nameEventServerReady="EventServerReady";
///HANDLE hServerReady;//m_hReady;
///HANDLE hStartServer;
///GUID hostGUIDInstance;

void PNetCenter::clearInOutClientHostBuffers()
{
	in_ClientBuf.reset();
	out_ClientBuf.reset();

	in_HostBuf.reset();
	out_HostBuf.reset();
}


//TestThread
/*int flaaaag=0;
DWORD WINAPI TestServerThread(LPVOID lpParameter)
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

//Second thread
DWORD WINAPI InternalServerThread(LPVOID lpParameter)
{
/*	// TEST thread!!!!
	DWORD ThreadId;
	HANDLE  hTestThread=CreateThread( NULL, 0, TestServerThread, lpParameter, 0, &ThreadId);
*/

	PNetCenter* pPNetCenter=(PNetCenter*)lpParameter;
	pPNetCenter->SecondThread();
	return 0;
}
XBuffer BUFFER_LOG(10000,1);

bool PNetCenter::SecondThread(void)
{
	XDPInit();

	Init();

	////xassert(0&&"QQQQQQQQQQQQ");

	StartFindHostDP();
	m_state=PNC_STATE__CLIENT_FIND_HOST;


	//Инициализация завершена - XDPConnection создан
	SetEvent(hSecondThreadInitComplete);

	bool flag_end=0;

	while(!flag_end){
		CAutoLock* _pLock=new CAutoLock(&m_GeneralLock);
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
					m_DPPacketList.clear();
					clearInOutClientHostBuffers();
					m_bStarted = false;
					if(m_state==PNC_STATE__CLIENT_FIND_HOST){
						if(gameSpyInterface && internalIP){
							StartConnect2IP(internalIP);
							m_state=PNC_STATE__INFINITE_CONNECTION_2_IP;
						}
						else 
							m_state=PNC_STATE__CONNECTION;
					}
					else xassert(0&&"Connecting: command order error(not find host state)");
				}
				break;
			case PNC_COMMAND__DISCONNECT_AND_ABORT_GAME_AND_END:
				{
					m_bStarted = false;

					///if(m_state==PNC_STATE__CLIENT_TUNING_GAME || m_state==PNC_STATE__CLIENT_LOADING_GAME || m_state==PNC_STATE__CLIENT_GAME){
					flag_end=1;
					///}
					///else xassert(0&&"Disconnect: command order error(not game)");
				}
				SetEvent(hCommandExecuted);
				break;
			case PNC_COMMAND__START_HOST_AND_CREATE_GAME_AND_STOP_FIND_HOST:
				{
					flag_LockIputPacket=0;
					flag_SkipProcessingGameCommand=0;
					m_DPPacketList.clear();
					clearInOutClientHostBuffers();
					m_bStarted = false;

					////if(WaitForSingleObject(hStartServer, INFINITE) != WAIT_OBJECT_0) xassert(0&&"Network server: run error.");
					m_state=PNC_STATE__HOST_TUNING_GAME; //Необходимо для DPN_MSGID_ENUM_HOSTS_QUERY чтоб сразу выдавал правильную инфу
					StopFindHostDP();

					LogMsg("starting server...");
					if(!isConnected()) {
						//m_pConnection->Init();
						ServerStart(m_GameName.c_str(), PERIMETER_DEFAULT_PORT);//
						//SetConnectionTimeout(TIMEOUT_DISCONNECT); //30s //3600000
					}
					LogMsg("started\n");


					GameInfo* pGameInfo = new GameInfo;
					//pGameInfo->id = m_dpnidGroupGame;
					strncpy(pGameInfo->Name, m_GameName.c_str(), PERIMETER_CONTROL_NAME_SIZE );

					SetServerInfo(pGameInfo, sizeof(GameInfo));
					delete pGameInfo;

				//	pNewGame->AddClient(nccg.createPlayerData_, 0/*dpnid*/, nccg.computerName_);
				//	pNewGame->StartGame();

					///hostGUIDInstance=getHostGUIDInstance();
					///m_dpnidGroupGame = m_pConnection->CreateGroup();

					////SetEvent(hServerReady);

					ClearClients();
					if(AddClient(internalPlayerData, m_localDPNID, m_GameName.c_str())==-1){
						xassert(0&&"Error connect host 2 missionDescription");
						ErrH.Abort("Network: General error 2!");
					}


					//PNCS_CREATING_GAME
					LogMsg("Start player list : \n");
					//m_flag_PlayerListReady=0;


					LogMsg("New game <%s> world=%d. for start...\n", m_GameName.c_str(), -1/*m_nWorldID*/);

					hostMissionDescription.clearAllPlayerGameReady();
					hostMissionDescription.setChanged();

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
						Init();//Close DirectPlay-я выполняет полную деинициализацию
					}
					StartFindHostDP();
					m_state=PNC_STATE__CLIENT_FIND_HOST;
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
							SendEvent(com, DPNID_ALL_PLAYERS_GROUP, true);
						}
					}
				}
				break;
			case PNC_COMMAND__START_FIND_HOST:
				{
					StartFindHostDP();
					m_state=PNC_STATE__CLIENT_FIND_HOST;
				}
				SetEvent(hCommandExecuted);
				break;
			case PNC_COMMAND__STOP_HOST_AND_ABORT_GAME_AND_END:
				{
					///m_pConnection->DestroyGroup(m_dpnidGroupGame);
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

		if(isHost()){
			HostReceiveQuant();
		}
		else {//Client receive quant!
			ClientPredReceiveQuant();
		}

		delete _pLock;

		if(flag_end) break; //для быстрого выхода

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



	StopFindHostDP();

	SetConnectionTimeout(1);//Для быстрого завершения
	//if(m_pConnection->Connected()) m_pConnection->Close();
	Close();

	XDPClose();


	return 0;
}






void PNetCenter::Start()
{
}


void PNetCenter::UpdateBattleData()
{
	//!!! Сервер не может нормально сконфигурить игру
	///MissionDescription battle(m_missionName.c_str());

	if(hostMissionDescription.gameType_==GT_createMPGame){
		//Random xchg
		////random_shuffle(missionDescription.playersData, missionDescription.playersData+missionDescription.playersAmount);
		hostMissionDescription.shufflePlayers();
	}
	else if(hostMissionDescription.gameType_==GT_loadMPGame){
	}
	hostMissionDescription.packPlayerIDs();
	for(int i=0; i<hostMissionDescription.playerAmountScenarioMax; i++){
		if(hostMissionDescription.playersData[i].realPlayerType==REAL_PLAYER_TYPE_PLAYER){
			hostMissionDescription.activePlayerID=hostMissionDescription.playersData[i].playerID;
			netCommand4C_StartLoadGame nccsl(hostMissionDescription);
			SendEvent(nccsl, hostMissionDescription.playersData[i].dpnid);//m_dpnidGroupGame);
		}
	}

	LogMsg("Sent battle info\n");
}

void PNetCenter::UpdateCurrentMissionDescription4C()
{
	MissionDescription curMD=hostMissionDescription;
	for(int i=0; i<curMD.playerAmountScenarioMax; i++){
		if(curMD.playersData[i].realPlayerType==REAL_PLAYER_TYPE_PLAYER){
			curMD.activePlayerID=curMD.playersData[i].playerID;
			netCommand4C_CurrentMissionDescriptionInfo nccmd(curMD);
			SendEvent(nccmd, curMD.playersData[i].dpnid);//m_dpnidGroupGame);
		}
	}
}
void PNetCenter::CheckClients()
{
	ClientMapType::iterator i = m_clients.begin();
	while(i != m_clients.end())
	{
		///if(WaitForSingleObject(i->second->m_hReady, 0) == WAIT_TIMEOUT)
		if(!((*i)->m_flag_Ready))
		{
			LogMsg("Client 0x%X(%s) is not ready. removing.\n", (*i)->dpnidPlayer, (*i)->m_szDescription);

			///m_pConnection->DelPlayerFromGroup(m_dpnidGroupGame, i->second->dpnidPlayer);
			RemovePlayer((*i)->dpnidPlayer);

			delete *i;
			m_clients.erase(i++);
		}
		else
			i++;
	}
}
void PNetCenter::WaitForAllClientsReady(int ms)
{
/*
	HANDLE hh[16];
	int k = 0;
	ClientMapType::iterator i;
	FOR_EACH(m_clients, i)
		hh[k++] = i->second->m_hReady;

	WaitForMultipleObjects(k, hh, TRUE, ms);
*/
}
void PNetCenter::ResetAllClients()
{
	unsigned int curTime=clocki();
	ClientMapType::iterator k;
	for(k=m_clients.begin(); k!=m_clients.end(); k++){
		///ResetEvent(i->second->m_hReady);
		(*k)->m_flag_Ready=0;
		(*k)->lastTimeBackPacket=curTime;//Необходимо для корректного начального таймаута
		(*k)->backGameInf2List.clear();
	}
}

void PNetCenter::DumpClients()
{
	LogMsg("Dumping clients---------------------------\n");
	ClientMapType::iterator i;
	FOR_EACH(m_clients, i)
	{
		LogMsg("Client 0x%X(%s)\n", (*i)->dpnidPlayer, (*i)->m_szDescription);
	}
	LogMsg("End of clients---------------------------\n");
}





bool PNetCenter::AddClientToMigratedHost(const DPNID _dpnid, unsigned int _curLastQuant, unsigned int _confirmQuant)
{
	//поиск есть-ли такой клиент
	ClientMapType::iterator p;
	for(p=m_clients.begin(); p!=m_clients.end(); p++){
		if( (*p)->dpnidPlayer == _dpnid) return 0;
	}

	int idxPlayerData=hostMissionDescription.findPlayer(_dpnid);
	if(idxPlayerData!=-1){
		PClientData* pPCD=new PClientData(idxPlayerData, _dpnid);
		m_clients.push_back(pPCD);
		//m_pConnection->AddPlayerToGroup(m_dpnidGroupGame, dpnid);
		pPCD->curLastQuant=_curLastQuant;//m_clientNumberGameQuant;
		pPCD->confirmQuant=_confirmQuant;
		hostMissionDescription.playersData[idxPlayerData].flag_playerStartReady=1;

		hostMissionDescription.setChanged();

		LogMsg("ReJoin client 0x%X for game %s\n", _dpnid, m_GameName.c_str());

		return 1;
	}
	else {
		LogMsg("client 0x%X for game %s id denied\n", _dpnid, m_GameName.c_str());
		return 0;
	}
}


void PNetCenter::SendEvent(netCommandGeneral& event, DPNID dpnid, bool flag_guaranted)
{
	//if(isHost()){
		if(dpnid!=m_localDPNID){
			out_HostBuf.putNetCommand(&event);
			out_HostBuf.send(*this, dpnid, flag_guaranted);
		}
		if( (dpnid==m_localDPNID) || (dpnid==DPNID_ALL_PLAYERS_GROUP /*m_dpnidGroupGame*/) ){
			in_ClientBuf.putNetCommand(&event);
		}
	//}

}

//void PNetCenter::StartGame()
//{
	//m_flag_PlayerListReady=1;
//}

void PNetCenter::ClearCommandList()
{
	list<netCommandGeneral*>::iterator m;
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
		if(gameSpyInterface){ //Internet
			unsigned int IP;
			if(internalIP) IP=internalIP;
			else IP=gameSpyInterface->getHostIP(m_gameHostID);
			if(IP==0) {
				//ErrH.Abort("Unable to find multiplayer server(err2)");
				ExecuteInternalCommand(PNC_COMMAND__DISCONNECT_AND_ABORT_GAME_AND_END_START_FIND_HOST, false);
			}
			if(!Connect(IP)){
				//ErrH.Abort("Unable to find multiplayer server");
				ExecuteInternalCommand(PNC_COMMAND__DISCONNECT_AND_ABORT_GAME_AND_END_START_FIND_HOST, false);
			}
		}
		else {
			if(internalIP) {
				if(!Connect(internalIP)) {
					ExecuteInternalCommand(PNC_COMMAND__DISCONNECT_AND_ABORT_GAME_AND_END_START_FIND_HOST, false);
					//ErrH.Abort("Unable to find multiplayer server");
				}
			}
			else {
				if(!Connect(m_gameHostID)) {
					ExecuteInternalCommand(PNC_COMMAND__DISCONNECT_AND_ABORT_GAME_AND_END_START_FIND_HOST, false);
					//ErrH.Abort("Unable to find multiplayer server");
				}
			}
		}
		//SetConnectionTimeout(TIMEOUT_DISCONNECT);//30s//3600000
		StopFindHostDP();
		m_state=PNC_STATE__CLIENT_TUNING_GAME;
		SetEvent(hCommandExecuted);
		break;
	case PNC_STATE__INFINITE_CONNECTION_2_IP:
		if(QuantConnect2IP()){
			StopFindHostDP();
			m_state=PNC_STATE__CLIENT_TUNING_GAME;
			SetEvent(hCommandExecuted);
		}
		break;
	case PNC_STATE__HOST_TUNING_GAME:
		{
			CAutoLock _Lock(&m_GeneralLock); //! Lock

			m_numberGameQuant= 1;//!

			if(hostMissionDescription.isChanged()){
				UpdateCurrentMissionDescription4C();
				hostMissionDescription.clearChanged();
			}
			///if(m_flag_PlayerListReady)//end waite
			if(hostMissionDescription.isAllRealPlayerStartReady()){

				LogMsg("Game Started\n");

				//LockAllPlayers
				UpdateBattleData();
				//ReleaseAllPlayers


				LogMsg("Wait for all clients ready. \n");
				///WaitForAllClientsReady(nWaitClientsReady*1000);

				//Для удаления игроков отпавших при загрузке
				ClearDeletePlayerGameCommand();

				m_state=PNC_STATE__HOST_LOADING_GAME;
			}
		}
		break;
	case PNC_STATE__HOST_LOADING_GAME: 
		{
			CAutoLock _Lock(&m_GeneralLock); //! Lock

			bool flag_ready=1;
			ClientMapType::iterator i;
			for(i=m_clients.begin(); i!=m_clients.end(); i++){
				flag_ready&=(*i)->m_flag_Ready;
				if(flag_ready) 
					if( (*m_clients.begin())->clientGameCRC != (*i)->clientGameCRC ) {
						XBuffer buf;
						buf < "Game of the player " <= (*i)->dpnidPlayer < "does not meet to game of the player " <= (*m_clients.begin())->dpnidPlayer;
						xxassert(0 , buf);
					}
			}
			if(flag_ready){
				CheckClients();
				DumpClients();


				///terEventBeginCommand ev_begin(m_nQuantDelay/100, 0);
				///SendEvent(ev_begin, m_dpnidGroupGame);

				///netCommandStartGame event;
				///SendEvent(event, DPNID_ALL_PLAYERS_GROUP/*m_dpnidGroupGame*/);
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
			CAutoLock _Lock(&m_GeneralLock); //! Lock

			///list<netCommand4H_BackGameInformation*>::iterator m;
			vector<netCommand4H_BackGameInformation2> & firstList=(*m_clients.begin())->backGameInf2List;
			while(!firstList.empty()) { //проверка что первый список не пустой
				ClientMapType::iterator k=m_clients.begin();
				k++;
				unsigned int countCompare=0;
				for(; k!=m_clients.end(); k++){
					vector<netCommand4H_BackGameInformation2> &  secondList=(*k)->backGameInf2List;
					if(!secondList.empty()){
						if( *firstList.begin() == *secondList.begin() ) countCompare++;//if( (**firstList.begin()).equalVData(**secondList.begin()) )
						else {
							if( (*firstList.begin()).quant_ != (*secondList.begin()).quant_ ) {
								//xassert(0 && "Unmatched number quants");
								XStream f("outnet.log", XS_OUT);
								f.write(BUFFER_LOG.address(), BUFFER_LOG.tell());
								f < currentVersion < "\r\n";
								f < "Unmatched number quants !" < "\n";
								vector<netCommand4H_BackGameInformation2>::iterator q;
								for(q=firstList.begin(); q!=firstList.end(); q++){
									f < "HostQuant=" <= (*q).quant_ < " " <= (*q).replay_ < " " <= (*q).state_< "\n";
								}
								for(q=secondList.begin(); q!=secondList.end(); q++){
									f < "ClientQuant=" <= (*q).quant_ < " " <= (*q).replay_ < " " <= (*q).state_ < "\n";
								}
								f.close();

								XBuffer to(1024,1);
								to < "Unmatched number quants !" < "N1=" <= (*firstList.begin()).quant_ < " N2=" <=(*secondList.begin()).quant_;
								::MessageBox(0, to, "Error network synchronization", MB_OK|MB_ICONERROR);
								ExecuteInternalCommand(PNC_COMMAND__ABORT_PROGRAM, false);
								return;
							}
							else {
								// Сравнение для netCommand4H_BackGameInformation2
								if( (*firstList.begin()).signature_ != (*secondList.begin()).signature_ ){
									SendEvent(netCommand4C_SaveLog(), DPNID_ALL_PLAYERS_GROUP/*m_dpnidGroupGame*/);
									XBuffer to(1024,1);
									XStream f("outnet.log", XS_OUT);
									f < currentVersion < "\r\n";
									f < "Unmatched game quants !" < "on Quant=" <= (*firstList.begin()).quant_;
									f.close();

									to < "Unmatched game quants !" < "on Quant=" <= (*firstList.begin()).quant_;
									::MessageBox(0, to, "Error network synchronization", MB_OK|MB_ICONERROR);
									ExecuteInternalCommand(PNC_COMMAND__ABORT_PROGRAM, false);
									return;
								}
							}
							///else xassert(0 && "Нераспознанная десинхронизация");
						}
					}
					else goto end_while_01; //break; //завершение while если один из спмсков кончится раньше первого
				}
				if(countCompare+1>=m_clients.size()) {
						quantConfirmation=(*(*m_clients.begin())->backGameInf2List.begin()).quant_;
					//erase begin elements
					for(k=m_clients.begin(); k!=m_clients.end(); k++){
						vector<netCommand4H_BackGameInformation2> &  secondList=(*k)->backGameInf2List;
						BUFFER_LOG <= (*secondList.begin()).quant_ < " " <= (*secondList.begin()).replay_ < " " <= (*secondList.begin()).state_< "\n";
						///delete *secondList.begin();
						secondList.erase(secondList.begin());
					}
				}
			}

end_while_01:;

			string notResponceClientList;
			ClientMapType::iterator k;
			unsigned int maxInternalLagQuant=0;
			unsigned short minClientExecutionQuat=m_numberGameQuant;
			for(k=m_clients.begin(); k!=m_clients.end(); k++){
				if((*k)->lagQuant > maxInternalLagQuant) {//Для подгонки всех клиентов
					maxInternalLagQuant=(*k)->lagQuant; 
				}
				if((*k)->lastExecuteQuant < minClientExecutionQuat) {//Для подгонки всех клиентов
					minClientExecutionQuat=(*k)->lastExecuteQuant;
				}
				if(clocki() > ((*k)->lastTimeBackPacket + TIMEOUT_CLIENT_OR_SERVER_RECEIVE_INFORMATION)){
					DPNID d=(*k)->dpnidPlayer;
					int n=hostMissionDescription.findPlayer(d);
					if(n!=-1) { notResponceClientList+=hostMissionDescription.playersData[n].name(); notResponceClientList+=' '; }
				}
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
			for(k=m_clients.begin(); k!=m_clients.end(); k++){
				flag_requestPause|=(*k)->requestPause;
				flag_changePause|=((*k)->requestPause!=(*k)->clientPause);
				(*k)->clientPause=(*k)->requestPause;
				if((*k)->requestPause){
					xassert(curPlayer<NETWORK_PLAYERS_MAX);
					if(curPlayer<NETWORK_PLAYERS_MAX){
						int idx=hostMissionDescription.findPlayer((*k)->dpnidPlayer);
						xassert(idx!=-1);
						if(idx!=-1) playersIDArr[curPlayer++]=hostMissionDescription.playersData[idx].playerID;
					}
				}
			}
			for(k=m_clients.begin(); k!=m_clients.end(); k++){
				if((*k)->requestPause && ((clocki()-(*k)->timeRequestPause) > MAX_TIME_PAUSE_GAME) ) {
					RemovePlayer((*k)->dpnidPlayer); //Полное удаление по DPN_MSGID_DESTROY_PLAYER
					break;//по одному клиенту за квант!
				}
			}

			if(hostPause && (!flag_requestPause) ){
				//Отмена паузы
				netCommand4C_Pause ncp(playersIDArr, false);
				SendEvent(ncp, DPNID_ALL_PLAYERS_GROUP);
				hostPause=false;
			}
			else if(flag_changePause && flag_requestPause){
				netCommand4C_Pause ncp(playersIDArr, true);
				SendEvent(ncp, DPNID_ALL_PLAYERS_GROUP);
				hostPause=true;
			}
			if(hostPause)
				break;

			//перенесение всех команд удаления в список комманд на выполнение
			list<netCommand4G_ForcedDefeat*>::iterator p;
			for(p=m_DeletePlayerCommand.begin(); p!=m_DeletePlayerCommand.end(); p++){
				PutGameCommand2Queue_andAutoDelete(*p);
			}
			m_DeletePlayerCommand.clear();


			//Установка последней команде, признака, последняя
			if(!m_CommandList.empty()){
				list<netCommandGeneral*>::iterator p=m_CommandList.end();
				p--; //последняя команда
				if((*p)->EventID==NETCOM_4G_ID_UNIT_COMMAND || (*p)->EventID==NETCOM_4G_ID_REGION || (*p)->EventID==NETCOM_4G_ID_FORCED_DEFEAT){
					(static_cast<netCommandGame*>(*p))->setFlagLastCommandInQuant();
				}
				else xassert("Error in commands list on server");
			}

			///hostGeneralCommandCounter+=m_CommandList.size(); //сейчас добавляется при PutGameCommand2Queue_andAutoDelete

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
				//Или сверху или это
				netCommandNextQuant* pcom=new netCommandNextQuant(m_numberGameQuant, m_nQuantCommandCounter, hostGeneralCommandCounter, quantConfirmation);
				m_CommandList.push_back(pcom);
			}


	//		if(!DbgPause())
	//		{
				list<netCommandGeneral*>::iterator i;
				FOR_EACH(m_CommandList, i)
				{
					if((**i).EventID==NETCOM_ID_NEXT_QUANT || (**i).EventID==NETCOM_4C_ID_CLIENT_IS_NOT_RESPONCE){
						SendEvent(**i, DPNID_ALL_PLAYERS_GROUP/*m_dpnidGroupGame*/, false);//Не гарантированная доставка!!!
					}
					else { 
						SendEvent(**i, DPNID_ALL_PLAYERS_GROUP/*m_dpnidGroupGame*/);
					}
					delete *i;
				}

				m_CommandList.clear();
	//		}


			m_numberGameQuant++;
		}
		break;
	case PNC_STATE__CLIENT_TUNING_GAME: 
		break;
	case PNC_STATE__CLIENT_LOADING_GAME:
		ClearDeletePlayerGameCommand(); /// ???????
		break;
	case PNC_STATE__CLIENT_GAME:
		{
		}
		break;
	case PNC_STATE__END:
		{
		}
		break;
	case PNC_STATE__NEWHOST_PHASE_0:
		{
			//Ожидание обработки всех комманд игрой
			{
				CAutoLock _Lock(&m_GeneralLock); //! Lock
				if(universe()){
					if(in_ClientBuf.currentNetCommandID()!=NETCOM_ID_NONE) break;
					if(universe()->allowedRealizingQuant > universe()->lastRealizedQuant) break;
					//по идее безопасно вызывать т.к. верхнее условие выполнится только при прошедшем кванте, но лучше переделать
					universe()->stopGame_HostMigrate();//Очистка всех команд текущего кванта
				}
				flag_SkipProcessingGameCommand=1;
				//По идее список пуст т.к. только выполнился ClientPredReceiveQuant
				m_DPPacketList.clear();
			}

			///m_dpnidGroupGame=m_groupDPNID;
			//Разблокирование входных пакетов, заблокированных при поступлении сообщения о смене Host-а
			//Необходимо для того, чтобы не пропустить сообщение о reJoin-е
			UnLockInputPacket();

			//Уничтожение всех не выполнившихся команд ?
			///////CAutoLock lock(&m_ClientInOutBuffLock);

			ClearClientData();
			//Т.к. миграция разрешается только после START_LOAD_GAME clientMissionDescription корректен
			hostMissionDescription=clientMissionDescription;
			hostMissionDescription.clearAllPlayerStartReady();
			//missionDescription.clearAllPlayerGameReady();
			//clientMissionDescription

			m_beginWaitTime=clocki();

			//очистка InHostBuffer в него пришли уже не те подтверждения!
			{
				CAutoLock _Lock(&m_GeneralLock); //! Lock
				clearInOutClientHostBuffers();
			}
			if(AddClientToMigratedHost(m_localDPNID, universe()->getCurrentGameQuant(), universe()->getConfirmQuant()) ){
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
			CAutoLock _Lock(&m_GeneralLock); //! Lock

			const unsigned int MAX_TIME_4_HOST_MIGRATE=10000;//10s

			bool result=1;
			///m_pConnection->refreshGroupMember();
			///list<DPNID>::iterator p;
			///for(p=m_pConnection->gameGroupMemberList.begin(); p!=m_pConnection->gameGroupMemberList.end(); p++){
			///	result&=(missionDescription.isPlayerStartReady(*p));
			///}
			if(hostMissionDescription.isAllRealPlayerStartReady()){
				result=1;
			}
			else result=0;
			
			if( result==1 ) { //если все существующие в группе игроки
				unsigned int maxConfirmedQuant=0;
				///unsigned int maxQuant=0;
				///unsigned int minQuant=UINT_MAX;
				///DPNID maxQuantClientDPNID=0;
				ClientMapType::iterator p;
				for(p=m_clients.begin(); p!=m_clients.end(); p++){
					if((*p)->confirmQuant > maxConfirmedQuant){
						maxConfirmedQuant=(*p)->confirmQuant;
					}
					/*if((*p)->curLastQuant >= maxQuant){
						maxQuant=(*p)->curLastQuant;
						maxQuantClientDPNID=(*p)->dpnidPlayer;
					}
					if((*p)->curLastQuant <= minQuant){
						minQuant=(*p)->curLastQuant;
					}*/
				}
				netCommand4C_sendLog2Host ncslh(maxConfirmedQuant+1);
				SendEvent(ncslh, DPNID_ALL_PLAYERS_GROUP);

				///minQuant+=1; //Запрос следующего кванта за мин квантом
				///netCommand4C_RequestLastQuantsCommands nc(minQuant);
				///SendEvent(nc, maxQuantClientDPNID);
				dpnidClientWhichWeWait=-1;//maxQuantClientDPNID;


				m_beginWaitTime=clocki();
				m_state=PNC_STATE__NEWHOST_PHASE_B;
			}
		}
		break;
	case PNC_STATE__NEWHOST_PHASE_B:
		{
			//if( ((clocki()-m_beginWaitTime) > MAX_TIME_WAIT_RESTORE_GAME_AFTER_MIGRATE_HOST) ) {
			//}
			if( (dpnidClientWhichWeWait==-1) || (hostMissionDescription.findPlayer(dpnidClientWhichWeWait)==-1) ){ //игрок выбыл до прихода нужных хосту игровых комманд
				unsigned int maxQuant=0;
				unsigned int minQuant=UINT_MAX;
				DPNID maxQuantClientDPNID=0;
				ClientMapType::iterator p;
				for(p=m_clients.begin(); p!=m_clients.end(); p++){
					if((*p)->curLastQuant >= maxQuant){
						maxQuant=(*p)->curLastQuant;
						maxQuantClientDPNID=(*p)->dpnidPlayer;
					}
					if((*p)->curLastQuant <= minQuant){
						minQuant=(*p)->curLastQuant;
					}
				}
				minQuant+=1; //Запрос следующего кванта за мин квантом
				netCommand4C_RequestLastQuantsCommands nc(minQuant);
				SendEvent(nc, maxQuantClientDPNID);
				dpnidClientWhichWeWait=maxQuantClientDPNID;
			}
		}
		break;
	case PNC_STATE__CLIENT_RESTORE_GAME_AFTE_CHANGE_HOST_PHASE_0:
		{
			//Ожидание обработки всех комманд игрой
			{
				CAutoLock _Lock(&m_GeneralLock); //! Lock
				if(universe()){
					if(in_ClientBuf.currentNetCommandID()!=NETCOM_ID_NONE) break;
					if(universe()->allowedRealizingQuant > universe()->lastRealizedQuant) break;
					//по идее безопасно вызывать т.к. верхнее условие выполнится только при прошедшем кванте, но лучше переделать
					universe()->stopGame_HostMigrate();//Очистка всех команд текущего кванта
				}
				flag_SkipProcessingGameCommand=1;
				//По идее список пуст т.к. только выполнился ClientPredReceiveQuant
				m_DPPacketList.clear();
			}

			//Разблокирование входных пакетов, заблокированных при поступлении сообщения о смене Host-а
			UnLockInputPacket();

			//очистка out_ClientBuf в него пришли уже не те подтверждения!
			{
				CAutoLock _Lock(&m_GeneralLock); //! Lock
				clearInOutClientHostBuffers();
			}
			m_state=PNC_STATE__CLIENT_RESTORE_GAME_AFTE_CHANGE_HOST_PHASE_AB;
			ClearClientData();
			m_beginWaitTime=clocki();
		}
		break;
	case PNC_STATE__CLIENT_RESTORE_GAME_AFTE_CHANGE_HOST_PHASE_AB:
		{
			CAutoLock _Lock(&m_GeneralLock); //! Lock для senda

			static unsigned char bandPassFilter=0;
			if((bandPassFilter&0x7)==0){//каждый 8й квант
				netCommand4H_ReJoinRequest ncrjr(universe()->getCurrentGameQuant(), universe()->getConfirmQuant() );
				SendEvent(ncrjr, m_hostDPNID);
				///m_state=PNC_STATE__CLIENT_GAME;
			}
			bandPassFilter++;

			if((clocki()-m_beginWaitTime) < MAX_TIME_WAIT_RESTORE_GAME_AFTER_MIGRATE_HOST){
			}
			else { //все время вышло
			}
		}
		break;
	case PNC_STATE__NET_CENTER_CRITICAL_ERROR:
		{
			//Close(false);
			//ExecuteInternalCommand(PNC_COMMAND__END, false);
		}
		break;
	case PNC_STATE__ENDING_GAME:
		{
			StopFindHostDP();
			Close(false);
			Init();//Close DirectPlay-я выполняет полную деинициализацию
			CAutoLock _Lock(&m_GeneralLock); //! Lock
			ClearClients();
			m_state=PNC_STATE__NONE;
		}
		SetEvent(hCommandExecuted);
		break;
	default:
		{
		}
		break;
							 
	}
	
}

void PNetCenter::ClientPredReceiveQuant()
{
	if(!out_ClientBuf.isEmpty()) out_ClientBuf.send(*this, m_hostDPNID);

	///PutInputPacket2NetBuffer(in_ClientBuf);
	if(flag_LockIputPacket) return; //return 0;
	int cnt=0;
	list<XDPacket>::iterator p=m_DPPacketList.begin();
	while(p!=m_DPPacketList.end()){
		if(p->dpnid==m_hostDPNID){

			//отфильтровывание команды
			InOutNetComBuffer tmp(2048, true);
			tmp.putBufferPacket(p->buffer, p->size);
			if(tmp.currentNetCommandID()==NETCOM_4C_ID_START_LOAD_GAME){
				ExecuteInternalCommand(PNC_COMMAND__CLIENT_STARTING_LOAD_GAME, false);
			}
			else if(tmp.currentNetCommandID()==NETCOM_ID_NEXT_QUANT){
				if(m_state==PNC_STATE__CLIENT_RESTORE_GAME_AFTE_CHANGE_HOST_PHASE_AB){
					m_state=PNC_STATE__CLIENT_GAME;
				}
			}

			//комманды клиенту
			if(in_ClientBuf.putBufferPacket(p->buffer, p->size)){
				p=m_DPPacketList.erase(p);
				cnt++;
			}
			else break;
		}
		else {
			xassert(0&&"Commands not from a host!");
			p=m_DPPacketList.erase(p);
		}
	}
	///return (cnt!=0);

}

void PNetCenter::HostReceiveQuant()
{
	if(flag_LockIputPacket) return; //return 0;

	DPNID dpnid=m_localDPNID;
	do { //Первый проход для внутреннего клиента


		//NETCOM_4H_ID_CHANGE_REAL_PLAYER_TYPE
		//netCommand4H_ChangeRealPlayerType

		///ClientMapType::iterator i;
		///FOR_EACH(m_clients, i)
		///	i->second->NetHandlerProc(dpnid);


		ClientMapType::iterator p;
		for(p=m_clients.begin(); p!=m_clients.end(); p++){
			//if(in_buffer.receive(*m_pConnection, dpnid)) 
			/////////////in_HostBuf.receive(*m_pConnection, dpnid);
			if(dpnid==(*p)->dpnidPlayer) {
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
								SendEvent(ncp, DPNID_ALL_PLAYERS_GROUP);
								(*p)->requestPause=true;
							}
							else if((*p)->requestPause && (!nc_rp.pause) ){
								int playersIDArr[NETWORK_PLAYERS_MAX];
								for(int i=0; i<NETWORK_PLAYERS_MAX; i++) playersIDArr[i]=netCommand4C_Pause::NOT_PLAYER_ID;
								netCommand4C_Pause ncp(playersIDArr, false);
								SendEvent(ncp, DPNID_ALL_PLAYERS_GROUP);
								(*p)->requestPause=false;
							}*/

						}
						break;
					case NETCOM_4H_ID_CHANGE_REAL_PLAYER_TYPE:
						{
							netCommand4H_ChangeRealPlayerType ncChRT(in_HostBuf);
							if(m_state!=PNC_STATE__HOST_TUNING_GAME) break;

							hostMissionDescription.setChanged();
							if(dpnid==m_hostDPNID){
								xassert(ncChRT.idxPlayerData_ < NETWORK_PLAYERS_MAX);
								if( ncChRT.idxPlayerData_!=hostMissionDescription.findPlayer(m_hostDPNID) ){//Проверка на то, что меняется не у Host-а
									if(ncChRT.newRealPlayerType_==REAL_PLAYER_TYPE_PLAYER) ncChRT.newRealPlayerType_=REAL_PLAYER_TYPE_OPEN; //Дополнительная проверка
									if(hostMissionDescription.playersData[ncChRT.idxPlayerData_].realPlayerType==REAL_PLAYER_TYPE_PLAYER){
										//Отбрасывание игрока
										DPNID delPlayerDPNID=hostMissionDescription.playersData[ncChRT.idxPlayerData_].dpnid;
										hostMissionDescription.disconnect2PlayerData(ncChRT.idxPlayerData_);
										hostMissionDescription.playersData[ncChRT.idxPlayerData_].realPlayerType=REAL_PLAYER_TYPE_CLOSE;
										RemovePlayer(delPlayerDPNID); //Полное удаление по DPN_MSGID_DESTROY_PLAYER

									}
									else if(hostMissionDescription.playersData[ncChRT.idxPlayerData_].realPlayerType==REAL_PLAYER_TYPE_AI){ //Если был AI
										if(ncChRT.newRealPlayerType_==REAL_PLAYER_TYPE_OPEN || ncChRT.newRealPlayerType_==REAL_PLAYER_TYPE_CLOSE){
											//Закрывать AI
											hostMissionDescription.disconnect2PlayerData(ncChRT.idxPlayerData_);
										}
									}
									else { //Если был Close Или Open
										if(ncChRT.newRealPlayerType_==REAL_PLAYER_TYPE_AI){ 
											hostMissionDescription.connectAI2PlayersData(ncChRT.idxPlayerData_);
										}
										else if(ncChRT.newRealPlayerType_==REAL_PLAYER_TYPE_OPEN || ncChRT.newRealPlayerType_==REAL_PLAYER_TYPE_CLOSE){
											hostMissionDescription.playersData[ncChRT.idxPlayerData_].realPlayerType=ncChRT.newRealPlayerType_;
										}
									}
								}
							}
						}
						break;
					case NETCOM_4H_ID_CHANGE_PLAYER_BELLIGERENT:
						{
							netCommand4H_ChangePlayerBelligerent  ncChB(in_HostBuf);
							if(m_state!=PNC_STATE__HOST_TUNING_GAME) break;

							hostMissionDescription.setChanged();
							xassert(ncChB.idxPlayerData_ < NETWORK_PLAYERS_MAX);
							//Host может менять у любого
							if(dpnid==m_hostDPNID) hostMissionDescription.changePlayerBelligerent(ncChB.idxPlayerData_, ncChB.newBelligerent_);//
							else hostMissionDescription.changePlayerBelligerent(dpnid, ncChB.newBelligerent_);//
						}
						break;

					case NETCOM_4H_ID_CHANGE_PLAYER_COLOR:
						{
							netCommand4H_ChangePlayerColor  ncChC(in_HostBuf);
							if(m_state!=PNC_STATE__HOST_TUNING_GAME) break;

							hostMissionDescription.setChanged();
							xassert(ncChC.idxPlayerData_ < NETWORK_PLAYERS_MAX);
							//Host может менять цвет любого
							if(dpnid==m_hostDPNID) hostMissionDescription.changePlayerColor(ncChC.idxPlayerData_, ncChC.newColor_);
							else hostMissionDescription.changePlayerColor(dpnid, ncChC.newColor_);
						}
						break;
					case NETCOM_4H_ID_CHANGE_PLAYER_DIFFICULTY:
						{
							netCommand4H_ChangePlayerDifficulty ncChD(in_HostBuf);
							if(m_state!=PNC_STATE__HOST_TUNING_GAME) break;

							hostMissionDescription.setChanged();
							xassert(ncChD.idxPlayerData_ < NETWORK_PLAYERS_MAX);
							//Host может менять у любого
							if(dpnid==m_hostDPNID) hostMissionDescription.changePlayerDifficulty(ncChD.idxPlayerData_, ncChD.difficulty_);
							else hostMissionDescription.changePlayerDifficulty(dpnid, ncChD.difficulty_);
						}
						break;
					case NETCOM_4H_ID_CHANGE_PLAYER_CLAN:
						{
							netCommand4H_ChangePlayerClan ncChC(in_HostBuf);
							if(m_state!=PNC_STATE__HOST_TUNING_GAME) break;

							hostMissionDescription.setChanged();
							xassert(ncChC.idxPlayerData_ < NETWORK_PLAYERS_MAX);
							//Host может менять у любого
							if(dpnid==m_hostDPNID) hostMissionDescription.changePlayerClan(ncChC.idxPlayerData_, ncChC.clan_);
							else hostMissionDescription.changePlayerClan(dpnid, ncChC.clan_);
						}
						break;
					case NETCOM_4H_ID_CHANGE_PLAYER_HANDICAP:
						{
							netCommand4H_ChangePlayerHandicap ncChH(in_HostBuf);
							if(m_state!=PNC_STATE__HOST_TUNING_GAME) break;

							hostMissionDescription.setChanged();
							xassert(ncChH.idxPlayerData_ < NETWORK_PLAYERS_MAX);
							//Host может менять у любого
							if(dpnid==m_hostDPNID) hostMissionDescription.changePlayerHandicap(ncChH.idxPlayerData_, ncChH.handicap_);
							else hostMissionDescription.changePlayerHandicap(dpnid, ncChH.handicap_);
						}
						break;
					case NETCOM_4H_ID_CHANGE_MAP:
						{
							netCommand4H_ChangeMap nc_changeMap(in_HostBuf);
							if(m_state!=PNC_STATE__HOST_TUNING_GAME) break;

							hostMissionDescription.setChanged();
							if(dpnid==m_hostDPNID){//только Host может менять карту
								int i;
								for(i=0; i< nc_changeMap.missionDescription_.playerAmountScenarioMax; i++){
									nc_changeMap.missionDescription_.playersData[i]=hostMissionDescription.playersData[i];
								}
								for(i; i< NETWORK_PLAYERS_MAX; i++){
									if(hostMissionDescription.playersData[i].realPlayerType==REAL_PLAYER_TYPE_PLAYER){
										DPNID delPlayerDPNID=hostMissionDescription.playersData[i].dpnid;
										hostMissionDescription.disconnect2PlayerData(i);
										//hostMissionDescription.playersData[i].realPlayerType=REAL_PLAYER_TYPE_CLOSE;
										RemovePlayer(delPlayerDPNID); //Полное удаление по DPN_MSGID_DESTROY_PLAYER
									}
								}
								hostMissionDescription=nc_changeMap.missionDescription_;
							}
						}
						break;
					case NETCOM_4H_ID_START_LOAD_GAME:
						{
							hostMissionDescription.setChanged();

							netCommand4H_StartLoadGame ncslg(in_HostBuf);
							hostMissionDescription.setPlayerStartReady(dpnid);
							///StartGame();
						}
						break;
					case NETCOMC_ID_PLAYER_READY:
						{
							hostMissionDescription.setChanged();

							netCommandC_PlayerReady event(in_HostBuf);
							(*p)->m_flag_Ready=1;
							(*p)->clientGameCRC=event.gameCRC_;

							LogMsg("Player 0x%X(%s) (GCRC=0x%X) reported ready\n", dpnid, (*p)->m_szDescription, (*p)->clientGameCRC);
							///SetEvent(m_hReady);
						}
						break;

					case NETCOM_4G_ID_UNIT_COMMAND:
						{
							netCommand4G_UnitCommand * pCommand = new netCommand4G_UnitCommand(in_HostBuf);
							//pCommand->setCurCommandQuant(m_numberGameQuant);
							//m_CommandList.push_back(pCommand);
							//m_nQuantCommandCounter++;
							PutGameCommand2Queue_andAutoDelete(pCommand);
						}
						break;
					case NETCOM_4G_ID_REGION:
						{
							netCommand4G_Region* pCommand=new netCommand4G_Region(in_HostBuf);
							//pCommand->setCurCommandQuant(m_numberGameQuant);
							//m_CommandList.push_back(pCommand);
							//m_nQuantCommandCounter++;
							PutGameCommand2Queue_andAutoDelete(pCommand);
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
					case NETCOM_4H_ID_ALIFE_PACKET:
						{
							netCommand4H_AlifePacket nc(in_HostBuf);
							(*p)->lastTimeBackPacket=clocki();
						}
						break;
					case NETCOM_4H_ID_RESPONCE_LAST_QUANTS_COMMANDS:
						{
							xassert(m_state==PNC_STATE__NEWHOST_PHASE_B);
							netCommand4H_ResponceLastQuantsCommands nci(in_HostBuf);
							if(m_state!=PNC_STATE__NEWHOST_PHASE_B) break;

							vector<netCommandGame*> tmpListGameCommands;

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
							SendEvent(ncContinueGame, DPNID_ALL_PLAYERS_GROUP/*m_dpnidGroupGame*/);
							//Выполнение команд, которые не у всех были выполнены
							for(m_numberGameQuant=nci.beginQuantCommandTransmit; m_numberGameQuant<=nci.endQuantCommandTransmit; m_numberGameQuant++){
								m_nQuantCommandCounter=0;
								vector<netCommandGame*>::iterator p;
								for(p=tmpListGameCommands.begin(); p!=tmpListGameCommands.end(); p++){
									if((*p)->curCommandQuant_==m_numberGameQuant) {
										SendEvent(**p, DPNID_ALL_PLAYERS_GROUP/*m_dpnidGroupGame*/);
										m_nQuantCommandCounter++;
									}
								}
								//netCommandNextQuant netCommandNextQuant(m_numberGameQuant, m_nQuantCommandCounter, netCommandNextQuant::NOT_QUANT_CONFIRMATION);
								//SendEvent(netCommandNextQuant, DPNID_ALL_PLAYERS_GROUP/*m_dpnidGroupGame*/);

							}
							hostGeneralCommandCounter=nci.finGeneraCommandCounter;
							netCommandNextQuant netCommandNextQuant(nci.endQuantCommandTransmit, m_nQuantCommandCounter, hostGeneralCommandCounter, netCommandNextQuant::NOT_QUANT_CONFIRMATION);
							SendEvent(netCommandNextQuant, DPNID_ALL_PLAYERS_GROUP/*m_dpnidGroupGame*/);

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
							SendEvent(ncp, DPNID_ALL_PLAYERS_GROUP);

							m_state=PNC_STATE__HOST_GAME;
						}
						break;
					case NETCOM_4G_ID_CHAT_MESSAGE:
						{
							netCommand4G_ChatMessage nc_ChatMessage(in_HostBuf);
							SendEvent(nc_ChatMessage, DPNID_ALL_PLAYERS_GROUP);
						}
						break;
					case EVENT_ID_SERVER_TIME_CONTROL:
						{
							terEventControlServerTime event(in_HostBuf);

		///					m_pGame->SetGameSpeedScale(event.scale, dpnidPlayer);
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
		if(p==m_clients.end()){ //Ни один существующий клиент не соответствует DPNID
			while(in_HostBuf.currentNetCommandID()) {
				if(in_HostBuf.currentNetCommandID()==NETCOMC_ID_JOIN_REQUEST){
					//HandleNewPlayer(dpnid);
					netCommandC_JoinRequest nc(in_HostBuf);
				}
				else if(in_HostBuf.currentNetCommandID()==NETCOM_4H_ID_REJOIN_REQUEST && m_state==PNC_STATE__NEWHOST_PHASE_A){
					netCommand4H_ReJoinRequest nc(in_HostBuf);
					AddClientToMigratedHost(dpnid, nc.currentLastQuant, nc.confirmedQuant);
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
		
	}while(PutInputPacket2NetBuffer(in_HostBuf, dpnid)!=0);
}


void PNetCenter::UnLockInputPacket(void)
{
	if(flag_LockIputPacket) flag_LockIputPacket--;
}

bool PNetCenter::PutInputPacket2NetBuffer(InOutNetComBuffer& netBuf)
{
/*	if(flag_LockIputPacket) return 0;
	int cnt=0;

	list<XDPacket>::iterator p=m_DPPacketList.begin();
	while(p!=m_DPPacketList.end()){
		if(netBuf.putBufferPacket(p->buffer, p->size)){
			p=m_DPPacketList.erase(p);
			cnt++;
		}
		else break;
	}
	return (cnt!=0);*/
	return 1;
}

bool PNetCenter::PutInputPacket2NetBuffer(InOutNetComBuffer& netBuf, DPNID& returnDPNID)
{
	if(flag_LockIputPacket) return 0;

	int cnt=0;
	list<XDPacket>::iterator p=m_DPPacketList.begin();
	if(p!=m_DPPacketList.end()){
		returnDPNID=p->dpnid;
		while(p!=m_DPPacketList.end()){
			if(returnDPNID==p->dpnid){
				if(netBuf.putBufferPacket(p->buffer, p->size)){
					p=m_DPPacketList.erase(p);
					cnt++;
				}
				else break;
			}
			else p++;
		}
	}
	return (cnt!=0);
}

