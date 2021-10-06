#include "NetIncludes.h"

#include "P2P_interface.h"

#include "GameShell.h"
#include "Universe.h"

#include <algorithm>

void PNetCenter::setNETIDInClientsDate(const int missionDescriptionIdx, NETID netid)
{
	hostMissionDescription.setChanged();
	ClientMapType::iterator p;
	for(p=m_clients.begin(); p!= m_clients.end(); p++) {
		if((*p)->missionDescriptionIdx==missionDescriptionIdx) {
			(*p)->netidPlayer=netid;
			break;
		}
	}
	if(p==m_clients.end())
		LogMsg("set NETID client-err1\n");
	if(hostMissionDescription.setPlayerNETID(missionDescriptionIdx, netid))
		LogMsg("set NETID client-OK\n");
	else
		LogMsg("set NETID client-err2\n");
}

void PNetCenter::DeleteClientByMissionDescriptionIdx(const int missionDescriptionIdx)
{
	hostMissionDescription.setChanged();

	ClientMapType::iterator p;
	for(p=m_clients.begin(); p!= m_clients.end(); p++) {
		if((*p)->missionDescriptionIdx==missionDescriptionIdx) {
			LogMsg("Client 0x%lX(%s) disconnecting-", (*p)->netidPlayer, (*p)->m_szDescription);
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

void PNetCenter::DeleteClientByNETID(const NETID netid, DWORD dwReason)
{
	if(isHost()){
		hostMissionDescription.setChanged();

		if(m_bStarted){
			//idx == playerID (на всякий случай);
			int idx=hostMissionDescription.findPlayer(netid);
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
			if((*p)->netidPlayer==netid) {
				LogMsg("Client 0x%lX(%s) disconnecting-", (*p)->netidPlayer, (*p)->m_szDescription);
				delete *p;
				m_clients.erase(p);
				break;
			}
		}

		if(hostMissionDescription.disconnectPlayer2PlayerDataByNETID(netid))
			LogMsg("OK\n");
		else
			LogMsg("error in missionDescription\n");
	}
	else {
		if(m_bStarted){
			//idx == playerID (на всякий случай);
			int idx=clientMissionDescription.findPlayer(netid);
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
		int idx=clientMissionDescription.findPlayer(netid);
		xassert(idx!=-1);
		if(idx!=-1){
			//отсылка сообщения о том, что игрок вышел
            ExecuteInterfaceCommand(
                dwReason ? PNC_INTERFACE_COMMAND_INFO_PLAYER_DISCONNECTED : PNC_INTERFACE_COMMAND_INFO_PLAYER_EXIT,
                clientMissionDescription.playersData[idx].name()
            );
		}
		//Удаление игрока из clientMD
		clientMissionDescription.disconnectPlayer2PlayerDataByNETID(netid);
	}

}


void PNetCenter::LockInputPacket(void)
{
	flag_LockIputPacket++;
}

