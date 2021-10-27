#include "NetIncludes.h"

#include "P2P_interface.h"

extern SDL_threadID net_thread_id;

//Запускается из 1 2 3-го потока
//Может вызываться с фдагом waitExecution только из одного потока (сейчас 1-го)
//Runs from 1 2 3rd thread
//Can be called with the waitExecution flag from only one thread (now the 1st one) 
bool PNetCenter::ExecuteInternalCommand(e_PNCInternalCommand ic, bool waitExecution)
{
	if( WaitForSingleObject(hSecondThread, 0) == WAIT_OBJECT_0) {
		return false;
	}

	if(waitExecution) ResetEvent(hCommandExecuted);

	{
		internalCommandList.push_back(ic);
	}
	if (waitExecution) {
#ifdef PERIMETER_DEBUG
        //Ensure is not being called from net thread since it will deadlock
        xassert(net_thread_id != SDL_ThreadID() && "Waiting execution from net thread!");
#endif
        
		//if(WaitForSingleObject(hCommandExecuted, INFINITE) != WAIT_OBJECT_0) xassert(0&&"Error execute command");
		const unsigned char ha_size=2;
		HANDLE ha[ha_size];
		ha[0]=hSecondThread;
		ha[1]=hCommandExecuted;
		uint32_t result=WaitForMultipleObjects(ha_size, ha, false, INFINITE);
		if(result<WAIT_OBJECT_0 || result>= (WAIT_OBJECT_0+ha_size)) {
			xassert(0&&"Error execute command");
		}
	}
	return true;
}



//Запускается из 2 3-го потока
int PNetCenter::AddClient(PlayerData& pd)
{
	CAutoLock _lock(m_GeneralLock); //В этой функции в некоторых вызовах будет вложенный

	int idxPlayerData=-1;
	if (hostMissionDescription.gameType_==GT_createMPGame) {
		idxPlayerData=hostMissionDescription.connectNewPlayer2PlayersData(pd);
	} else if(hostMissionDescription.gameType_==GT_loadMPGame) {
		idxPlayerData=hostMissionDescription.connectLoadPlayer2PlayersData(pd);
	}
	
    hostMissionDescription.setChanged();
    
	if (0 <= idxPlayerData) {
		//missionDescription.playersData[idxPlayerData].netid=netid;
		//missionDescription.playersData[idxPlayerData].flag_playerStartReady=1;

		PClientData* pCD=new PClientData(idxPlayerData, pd.name(), pd.netid);
		pCD->backGameInf2List.reserve(20000);//резерв под 20000 квантов
		m_clients.push_back(pCD);

		LogMsg("New client %d %s for game %s\n", idxPlayerData, pd.name(), m_GameName.c_str());

//		netCommand4C_JoinResponse ncjr(netid, NETID_ALL_PLAYERS_GROUP/*m_netidGroupGame*/, NCJRR_OK);
//		SendEvent(ncjr, netid);
		return idxPlayerData;
	} else {
		LogMsg("Client %s for game %s id denied\n", pd.name(), m_GameName.c_str());
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
	std::list<netCommand4G_ForcedDefeat*>::iterator p;
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


