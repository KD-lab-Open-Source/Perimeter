#include "StdAfx.h"

#include "P2P_interface.h"

#include "GameShell.h"
#include "Universe.h"

#include <algorithm>

#include "P2P_interfaceAux.h"

void PNetCenter::setDPNIDInClientsDate(const int missionDescriptionIdx, DPNID dpnid)
{
	hostMissionDescription.setChanged();
	ClientMapType::iterator p;
	for(p=m_clients.begin(); p!= m_clients.end(); p++) {
		if((*p)->missionDescriptionIdx==missionDescriptionIdx) {
			(*p)->dpnidPlayer=dpnid;
			break;
		}
	}
	if(p==m_clients.end())
		LogMsg("set DPNID client-err1\n");
	if(hostMissionDescription.setPlayerDPNID(missionDescriptionIdx, dpnid))
		LogMsg("set DPNID client-OK\n");
	else
		LogMsg("set DPNID client-err2\n");
}

void PNetCenter::DeleteClientByMissionDescriptionIdx(const int missionDescriptionIdx)
{
	hostMissionDescription.setChanged();

	ClientMapType::iterator p;
	for(p=m_clients.begin(); p!= m_clients.end(); p++) {
		if((*p)->missionDescriptionIdx==missionDescriptionIdx) {
			LogMsg("Client 0x%X(%s) disconnecting-", (*p)->dpnidPlayer, (*p)->m_szDescription);
			delete *p;
			m_clients.erase(p);
			break;
		}
	}
	if(hostMissionDescription.disconnectPlayer2PlayerDataByIndex(missionDescriptionIdx))
		LogMsg("OK\n");
	else
		LogMsg("error in missionDescription\n");
}

void PNetCenter::DeleteClientByDPNID(const DPNID dpnid, DWORD dwReason)
{
	if(isHost()){
		hostMissionDescription.setChanged();

		if(m_bStarted){
			//idx == playerID (на всякий случай);
			int idx=hostMissionDescription.findPlayer(dpnid);
			xassert(idx!=-1);
			if(idx!=-1){
				int playerID=hostMissionDescription.playersData[idx].playerID;
				netCommand4G_ForcedDefeat* pncfd=new netCommand4G_ForcedDefeat(playerID);
				//PutGameCommand2Queue_andAutoDelete(pncfd);
				m_DeletePlayerCommand.push_back(pncfd);
			}
		}


		ClientMapType::iterator p;
		for(p=m_clients.begin(); p!= m_clients.end(); p++) {
			if((*p)->dpnidPlayer==dpnid) {
				LogMsg("Client 0x%X(%s) disconnecting-", (*p)->dpnidPlayer, (*p)->m_szDescription);
				delete *p;
				m_clients.erase(p);
				break;
			}
		}

		if(hostMissionDescription.disconnectPlayer2PlayerDataByDPNID(dpnid))
			LogMsg("OK\n");
		else
			LogMsg("error in missionDescription\n");
	}
	else {
		if(m_bStarted){
			//idx == playerID (на всякий случай);
			int idx=clientMissionDescription.findPlayer(dpnid);
			xassert(idx!=-1);
			if(idx!=-1){
				int playerID=clientMissionDescription.playersData[idx].playerID;
				netCommand4G_ForcedDefeat* pncfd=new netCommand4G_ForcedDefeat(playerID);
				//PutGameCommand2Queue_andAutoDelete(pncfd);
				//Нужно в случае миграции хоста
				m_DeletePlayerCommand.push_back(pncfd);
			}
		}
	}
	if(m_bStarted){
		int idx=clientMissionDescription.findPlayer(dpnid);
		xassert(idx!=-1);
		if(idx!=-1){
			//отсылка сообщения о том, что игрок вышел
            ExecuteInterfaceCommand(
                dwReason ? PNC_INTERFACE_COMMAND_INFO_PLAYER_DISCONNECTED : PNC_INTERFACE_COMMAND_INFO_PLAYER_EXIT,
                clientMissionDescription.playersData[idx].name()
            );
		}
		//Удаление игрока из clientMD
		clientMissionDescription.disconnectPlayer2PlayerDataByDPNID(dpnid);
	}

}


void PNetCenter::LockInputPacket(void)
{
	flag_LockIputPacket++;
}

