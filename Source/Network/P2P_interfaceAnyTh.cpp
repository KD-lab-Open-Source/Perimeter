#include "StdAfx.h"

#include "P2P_interface.h"



//Запускается из 1 2 3-го потока
//Может вызываться с фдагом waitExecution только из одного потока (сейчас 1-го)
bool PNetCenter::ExecuteInternalCommand(e_PNCInternalCommand ic, bool waitExecution)
{
	if( WaitForSingleObject(hSecondThread, 0) == WAIT_OBJECT_0) {
		return 0;
	}

	if(waitExecution) ResetEvent(hCommandExecuted);

	{
		internalCommandList.push_back(ic);
	}
	if(waitExecution){
		//if(WaitForSingleObject(hCommandExecuted, INFINITE) != WAIT_OBJECT_0) xassert(0&&"Error execute command");
		const unsigned char ha_size=2;
		HANDLE ha[ha_size];
		ha[0]=hSecondThread;
		ha[1]=hCommandExecuted;
		DWORD result=WaitForMultipleObjects(ha_size, ha, FALSE, INFINITE);
		if(result<WAIT_OBJECT_0 || result>= (WAIT_OBJECT_0+ha_size)) {
			xassert(0&&"Error execute command");
		}
	}
	return 1;
}



//Запускается из 2 3-го потока
int PNetCenter::AddClient(PlayerData& pd, const DPNID dpnid, const char* descr)
{
	CAutoLock _lock(&m_GeneralLock); //В этой функции в некоторых вызовах будет вложенный

	int idxPlayerData=-1;
	if(hostMissionDescription.gameType_==GT_createMPGame){
		idxPlayerData=hostMissionDescription.connectNewPlayer2PlayersData(pd, dpnid);
	}
	else if(hostMissionDescription.gameType_==GT_loadMPGame){
		idxPlayerData=hostMissionDescription.connectLoadPlayer2PlayersData(pd, dpnid);
	}
	hostMissionDescription.setChanged();
	if(idxPlayerData!=-1){
		//missionDescription.playersData[idxPlayerData].dpnid=dpnid;
		//missionDescription.playersData[idxPlayerData].flag_playerStartReady=1;

		PClientData* pCD=new PClientData(idxPlayerData, dpnid, descr);
		pCD->backGameInf2List.reserve(20000);//резерв под 20000 квантов
		m_clients.push_back(pCD);

		///m_pConnection->AddPlayerToGroup(m_dpnidGroupGame, dpnid);

		LogMsg("New client 0x%X(%s) for game %s\n", dpnid, descr, m_GameName.c_str());

//		netCommand4C_JoinResponse ncjr(dpnid, DPNID_ALL_PLAYERS_GROUP/*m_dpnidGroupGame*/, NCJRR_OK);
//		SendEvent(ncjr, dpnid);
		return idxPlayerData;
	}
	else {
		LogMsg("client 0x%X(%s) for game %s id denied\n", dpnid, descr, m_GameName.c_str());
		return -1;
	}

}

//Запускается из 1-го(деструктор) и 2-го потока
void PNetCenter::ClearClients()
{
	ClientMapType::iterator i;
	FOR_EACH(m_clients, i)
		delete *i;
	m_clients.clear();
}

//Запускается из 1 и 2-го потока
void PNetCenter::clearInternalFoundHostList(void) 
{
	CAutoLock _lock(&m_GeneralLock); //В этой функции в некоторых вызовах будет вложенный
	vector<INTERNAL_HOST_ENUM_INFO*>::iterator p;
	for(p=internalFoundHostList.begin(); p!=internalFoundHostList.end(); p++){
		delete *p;
	}
	internalFoundHostList.erase(internalFoundHostList.begin(), internalFoundHostList.end());
}


//Запускается из 2 и 3-го потока
/// !!! педается указатель !!! удаление происходит автоматом после осылки !!!
void PNetCenter::PutGameCommand2Queue_andAutoDelete(netCommandGame* pCommand)
{
	pCommand->setCurCommandQuantAndCounter(m_numberGameQuant, hostGeneralCommandCounter);
	m_CommandList.push_back(pCommand);
	hostGeneralCommandCounter++;
	m_nQuantCommandCounter++;
}

void PNetCenter::ClearDeletePlayerGameCommand()
{
	list<netCommand4G_ForcedDefeat*>::iterator p;
	for(p=m_DeletePlayerCommand.begin(); p!=m_DeletePlayerCommand.end(); p++){
		delete *p;
	}
	m_DeletePlayerCommand.clear();
}


bool PNetCenter::ExecuteInterfaceCommand(e_PNCInterfaceCommands ic, const char* str)
{
	{
		interfaceCommandList.push_back(sPNCInterfaceCommand(ic, str));
	}
	return 1;
}


