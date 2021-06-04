#include "StdAfx.h"

#include "P2P_interface.h"
#include "GS_interface.h"

#include "GameShell.h"
#include "Universe.h"

#include "Lmcons.h"

#include "..\terra\terra.h"

#include <algorithm>

#include "P2P_interfaceAux.h"


HRESULT WINAPI DirectPlayMessageHandler(PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer)
{
	return ((PNetCenter*)pvUserContext)->DirectPlayMessageHandler(dwMessageId, pMsgBuffer);
}



HRESULT PNetCenter::DirectPlayMessageHandler(DWORD dwMessageId, PVOID pMsgBuffer)
{

	CAutoLock _lock(&m_GeneralLock);
	switch(dwMessageId) {
	case DPN_MSGID_INDICATE_CONNECT:
		{
			DPNMSG_INDICATE_CONNECT* pMsg = (DPNMSG_INDICATE_CONNECT*)pMsgBuffer;

			//if(strncmp((LPCTSTR)pMsg->pvUserConnectData, lpszSignatureRQ, __min((int)pMsg->dwUserConnectDataSize, strlen(lpszSignatureRQ))) != 0)
			//	return E_FAIL; //bad signature
			
			if(pMsg->dwUserConnectDataSize!=sizeof(sConnectInfo))
				return E_FAIL;
			sConnectInfo clientConnectInfo((unsigned char*)pMsg->pvUserConnectData, pMsg->dwUserConnectDataSize);
			if(!clientConnectInfo.checkOwnCorrect()) 
				return E_FAIL;

			static sDigitalGameVersion hostDGV(true);//создание версии игры
			static sReplyConnectInfo replyConnectInfo;
			pMsg->pvReplyData=&replyConnectInfo;
			pMsg->dwReplyDataSize=sizeof(replyConnectInfo);
			if(hostDGV!=clientConnectInfo.dgv){ //Несоответствующая версия игры
				replyConnectInfo.set(sReplyConnectInfo::CR_ERR_INCORRECT_VERSION, hostDGV);
				return E_FAIL;
			}
			if(m_bStarted) { // Игра запущена
				replyConnectInfo.set(sReplyConnectInfo::CR_ERR_GAME_STARTED, hostDGV);
				return E_FAIL;
			}
			if( (!gamePassword.empty()) && (!clientConnectInfo.isPasswordCorrect(gamePassword.c_str())) ){
				replyConnectInfo.set(sReplyConnectInfo::CR_ERR_INCORRECT_PASWORD, hostDGV);
				return E_FAIL;
			}
			int resultIdx=AddClient(clientConnectInfo.perimeterPlayerData, 0, "");
			if(resultIdx==-1) {// Игра полная
				replyConnectInfo.set(sReplyConnectInfo::CR_ERR_GAME_FULL, hostDGV);
				return E_FAIL;
			}

			pMsg->pvPlayerContext=(void*)resultIdx; //для корректного удаления в DPN_MSGID_INDICATED_CONNECT_ABORTED 
			//pMsg->pvReplyData = (void*)lpszSignatureRPL;
			//pMsg->dwReplyDataSize = strlen(lpszSignatureRPL);
			replyConnectInfo.set(sReplyConnectInfo::CR_OK, hostDGV);
		}
		break;

	case DPN_MSGID_INDICATED_CONNECT_ABORTED:
		{
			DPNMSG_INDICATED_CONNECT_ABORTED* pMsg=(DPNMSG_INDICATED_CONNECT_ABORTED*)pMsgBuffer;

			if(isHost()){
				DeleteClientByMissionDescriptionIdx((unsigned int)pMsg->pvPlayerContext);
			}
		}

		break;
    case DPN_MSGID_DESTROY_PLAYER:
		{
			PDPNMSG_DESTROY_PLAYER pDestroyPlayerMsg;
			pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER)pMsgBuffer;

			///NetHandlerProc(pDestroyPlayerMsg->dpnidPlayer, msg);
			//if(isHost()){
			DeleteClientByDPNID(pDestroyPlayerMsg->dpnidPlayer, pDestroyPlayerMsg->dwReason);
			//}
			break;
		}
	case DPN_MSGID_CONNECT_COMPLETE:
		{
			DPNMSG_CONNECT_COMPLETE* pMsg = (DPNMSG_CONNECT_COMPLETE*)pMsgBuffer;
		    //if( SUCCEEDED(pMsg->hResultCode) ){
			//	if(strncmp((LPCTSTR)pMsg->pvApplicationReplyData , lpszSignatureRPL, __min((int)pMsg->dwApplicationReplyDataSize , strlen(lpszSignatureRPL))) != 0)
			//		InterlockedIncrement(&m_nClientSgnCheckError);
			//}
			if(pMsg->dwApplicationReplyDataSize==sizeof(sReplyConnectInfo)){
				sReplyConnectInfo hostReplyConnectInfo((unsigned char*)pMsg->pvApplicationReplyData, pMsg->dwApplicationReplyDataSize);
				if(hostReplyConnectInfo.checkOwnCorrect()){
					if( (hostReplyConnectInfo.connectResult==sReplyConnectInfo::CR_OK) && ( SUCCEEDED(pMsg->hResultCode)) ){
						ExecuteInterfaceCommand(PNC_INTERFACE_COMMAND_CONNECT_OK);
						return DPN_OK;
					}
					else if(hostReplyConnectInfo.connectResult==sReplyConnectInfo::CR_ERR_INCORRECT_VERSION){
						ExecuteInterfaceCommand(PNC_INTERFACE_COMMAND_CONNECT_ERR_INCORRECT_VERSION);
						return DPN_OK;
					}
					else if(hostReplyConnectInfo.connectResult==sReplyConnectInfo::CR_ERR_GAME_STARTED){
						ExecuteInterfaceCommand(PNC_INTERFACE_COMMAND_CONNECT_ERR_GAME_STARTED);
						return DPN_OK;
					}
					else if(hostReplyConnectInfo.connectResult==sReplyConnectInfo::CR_ERR_GAME_FULL){
						ExecuteInterfaceCommand(PNC_INTERFACE_COMMAND_CONNECT_ERR_GAME_FULL);
						return DPN_OK;
					}
					else if(hostReplyConnectInfo.connectResult==sReplyConnectInfo::CR_ERR_INCORRECT_PASWORD){
						ExecuteInterfaceCommand(PNC_INTERFACE_COMMAND_CONNECT_ERR_PASSWORD);
						return DPN_OK;
					}
				}
				else {
					InterlockedIncrement(&m_nClientSgnCheckError);
				}
			}
			else {
				InterlockedIncrement(&m_nClientSgnCheckError);
			}
			ExecuteInterfaceCommand(PNC_INTERFACE_COMMAND_CONNECT_ERR);
			return DPN_OK;
		}
		break;

    case DPN_MSGID_CREATE_PLAYER:
		{
			PDPNMSG_CREATE_PLAYER pCreatePlayerMsg;
			pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER)pMsgBuffer;


			IDirectPlay8Peer* m_pDP;
			//if(m_mode == DP_SERVER) m_pDP=m_pDPServer;
			//else if(m_mode == DP_CLIENT) m_pDP=m_pDPClient;
			m_pDP=m_pDPPeer;

			DPNID dpnid=pCreatePlayerMsg->dpnidPlayer;

			HRESULT hr;
			DWORD dwSize = 0;
			DPN_PLAYER_INFO* pdpPlayerInfo = NULL;

			// Get the peer info and extract its name 
			hr = DPNERR_CONNECTING;

			// GetPeerInfo might return DPNERR_CONNECTING when connecting, 
			// so just keep calling it if it does
			while( hr == DPNERR_CONNECTING ) 
				hr = m_pDP->GetPeerInfo( dpnid, pdpPlayerInfo, &dwSize, 0 );                                

			if( hr == DPNERR_BUFFERTOOSMALL ) {
				pdpPlayerInfo = (DPN_PLAYER_INFO*) new BYTE[ dwSize ];
				if( NULL == pdpPlayerInfo ) {
					hr = E_OUTOFMEMORY;
					goto LErrorReturn;
				}

				ZeroMemory( pdpPlayerInfo, dwSize );
				pdpPlayerInfo->dwSize = sizeof(DPN_PLAYER_INFO);

				hr = m_pDP->GetPeerInfo( dpnid, pdpPlayerInfo, &dwSize, 0 );
				if( SUCCEEDED(hr) ) {
                    if( pdpPlayerInfo->dwPlayerFlags & DPNPLAYER_LOCAL )
                        m_localDPNID=dpnid;
					if( pdpPlayerInfo->dwPlayerFlags & DPNPLAYER_HOST )
						m_hostDPNID=dpnid;
					//Дополнительно
					if( (pdpPlayerInfo->dwPlayerFlags&DPNPLAYER_LOCAL)==0 && isHost()){//Кривоватое условие
						setDPNIDInClientsDate((unsigned int)pCreatePlayerMsg->pvPlayerContext, pCreatePlayerMsg->dpnidPlayer);
					}

				}

				hr=S_OK;
			}
LErrorReturn:
			//SAFE_DELETE_ARRAY( pdpPlayerInfo );
			if(pdpPlayerInfo) delete [] pdpPlayerInfo;
			return hr;

			break;
		}
	case DPN_MSGID_CREATE_GROUP:
		{
			DPNMSG_CREATE_GROUP* pMsg = (DPNMSG_CREATE_GROUP*)pMsgBuffer;
			///m_dpnidGroupCreating = pMsg->dpnidGroup;
		}
		break;
	case DPN_MSGID_ENUM_HOSTS_QUERY:
		{
			DPNMSG_ENUM_HOSTS_QUERY* pMsg=(DPNMSG_ENUM_HOSTS_QUERY*)pMsgBuffer;
			//AutoLock!!!
			if(isHost()){
				static sGameStatusInfo cGSInf;
				cGSInf.set(hostMissionDescription.playersMaxEasily(), hostMissionDescription.playersAmount(), isGameRun(), 10, hostMissionDescription.worldID());
				xassert(sizeof(cGSInf) <= pMsg->dwMaxResponseDataSize);
				pMsg->pvResponseData=&cGSInf;
				pMsg->dwResponseDataSize=sizeof(cGSInf);
			}
		}
		break;
	case DPN_MSGID_ENUM_HOSTS_RESPONSE:
		{

			DPNMSG_ENUM_HOSTS_RESPONSE* pMsg = (DPNMSG_ENUM_HOSTS_RESPONSE*)pMsgBuffer;

			if(pMsg->pApplicationDescription->guidApplication==guidPerimeterGame){
				vector<INTERNAL_HOST_ENUM_INFO*>::iterator p;
				for(p=internalFoundHostList.begin(); p!=internalFoundHostList.end(); p++){
					if(pMsg->pApplicationDescription->guidInstance == (*p)->pAppDesc->guidInstance){
						if(sizeof(sGameStatusInfo)==pMsg->dwResponseDataSize){
							(*p)->gameStatusInfo=*((sGameStatusInfo*)(pMsg->pvResponseData));
							(*p)->gameStatusInfo.ping=pMsg->dwRoundTripLatencyMS;
							(*p)->timeLastRespond=clocki();
						}
						else xassert(0&& "Invalid enum host responce!");
						break;
					}
				}
				if(p==internalFoundHostList.end()){ //host not found
					INTERNAL_HOST_ENUM_INFO*  pNewHost=new INTERNAL_HOST_ENUM_INFO(pMsg);
					if(sizeof(sGameStatusInfo)==pMsg->dwResponseDataSize){
						pNewHost->gameStatusInfo=*((sGameStatusInfo*)(pMsg->pvResponseData));
						pNewHost->gameStatusInfo.ping=pMsg->dwRoundTripLatencyMS;
						pNewHost->timeLastRespond=clocki();
					}
					else xassert(0&& "Invalid enum host responce!");
					internalFoundHostList.push_back(pNewHost);
				}
			}
			break;
		}

    case DPN_MSGID_TERMINATE_SESSION:
		{
			PDPNMSG_TERMINATE_SESSION pTerminateSessionMsg;
			pTerminateSessionMsg = (PDPNMSG_TERMINATE_SESSION)pMsgBuffer;

			//m_mode = DP_NOBODY;
			flag_connected=0;
			m_bStarted=0;
			if(pTerminateSessionMsg->hResultCode==DPNERR_HOSTTERMINATEDSESSION){
				//dropped
				ExecuteInterfaceCommand(PNC_INTERFACE_COMMAND_CONNECTION_DROPPED);
			}
			else if(pTerminateSessionMsg->hResultCode==DPNERR_CONNECTIONLOST){
				ExecuteInterfaceCommand(PNC_INTERFACE_COMMAND_CONNECTION_FAILED);
			}
			else {
				//Не распознанная ситуация
				ExecuteInterfaceCommand(PNC_INTERFACE_COMMAND_CONNECTION_FAILED);
			}
			//xassert(0 && "DP connect terminate!");
			break;
		}
	case DPN_MSGID_RECEIVE:
		{
			PDPNMSG_RECEIVE pReceiveMsg;
			pReceiveMsg = (PDPNMSG_RECEIVE)pMsgBuffer;

			{
				m_DPPacketList.push_back(XDPacket());
				m_DPPacketList.back().set(pReceiveMsg->dpnidSender, pReceiveMsg->dwReceiveDataSize, pReceiveMsg->pReceiveData);

				InOutNetComBuffer tmp(2048, true);
				tmp.putBufferPacket(pReceiveMsg->pReceiveData, pReceiveMsg->dwReceiveDataSize);
				if(tmp.currentNetCommandID()==NETCOM_4C_ID_CUR_MISSION_DESCRIPTION_INFO){
					netCommand4C_CurrentMissionDescriptionInfo ncCMD(tmp);
					int color1=ncCMD.missionDescription_.playersData[0].colorIndex;
					int color2=ncCMD.missionDescription_.playersData[1].colorIndex;
				}
			}

			///XDP_Message msg=XDPMSG_DateReceive;
			///NetHandlerProc(pReceiveMsg->dpnidSender, msg);

			break;
		}
	case DPN_MSGID_DESTROY_GROUP:
		{
			DPNMSG_DESTROY_GROUP* m=(DPNMSG_DESTROY_GROUP*)pMsgBuffer;
			break;
		}

	case DPN_MSGID_HOST_MIGRATE:
        {
			if((m_state!=PNC_STATE__CLIENT_LOADING_GAME) && (m_state!=PNC_STATE__CLIENT_GAME)){
				//Нужно запустить abort
				ExecuteInterfaceCommand(PNC_INTERFACE_COMMAND_HOST_TERMINATED_GAME);//PNC_INTERFACE_COMMAND_CONNECTION_FAILED);
				break;
			}
			LockInputPacket();

            PDPNMSG_HOST_MIGRATE pHostMigrateMsg = (PDPNMSG_HOST_MIGRATE)pMsgBuffer;


			m_hostDPNID=pHostMigrateMsg->dpnidNewHost;
            if( m_hostDPNID == m_localDPNID ){//Host Я
				{
/*					if(universe() ){
						universe()->stopGame_HostMigrate();//Очистка всех команд текущего кванта
					}
					flag_SkipProcessingGameCommand=1;
					m_DPPacketList.clear();
*/				}
				ExecuteInternalCommand(PNC_COMMAND__STOP_GAME_AND_ASSIGN_HOST_2_MY, false);
			}
			else { // Host не Я
				{
/*					if(universe() ){
						universe()->stopGame_HostMigrate();//Очистка всех команд текущего кванта
					}
					flag_SkipProcessingGameCommand=1;
					m_DPPacketList.clear();
*/				}
				ExecuteInternalCommand(PNC_COMMAND__STOP_GAME_AND_WAIT_ASSIGN_OTHER_HOST, false);
			}
			///NetHandlerProc(pHostMigrateMsg->dpnidNewHost, msg);
            break;
        }
	default:
		{
			switch(dwMessageId){
			case DPN_MSGID_ADD_PLAYER_TO_GROUP	: 
				break;
			case DPN_MSGID_APPLICATION_DESC		: 
				break;
			case DPN_MSGID_ASYNC_OP_COMPLETE	: 
				break;
			case DPN_MSGID_CLIENT_INFO			: 
				break;	
			case DPN_MSGID_CONNECT_COMPLETE		: 
				break;	
			case DPN_MSGID_CREATE_GROUP			: 
				break;	
			case DPN_MSGID_CREATE_PLAYER		: 
				break;		
			case DPN_MSGID_DESTROY_GROUP		: 
				break;		
			case DPN_MSGID_DESTROY_PLAYER		: 
				break;	
			case DPN_MSGID_ENUM_HOSTS_QUERY		: 
				break;	
			case DPN_MSGID_ENUM_HOSTS_RESPONSE	: 
				break;	
			case DPN_MSGID_GROUP_INFO			: 
				break;	
			case DPN_MSGID_HOST_MIGRATE			: 
				break;	
			case DPN_MSGID_INDICATE_CONNECT			: 
				break;
			case DPN_MSGID_INDICATED_CONNECT_ABORTED: 
				break;	
			case DPN_MSGID_PEER_INFO			: 
				break;		
			case DPN_MSGID_RECEIVE				: 
				break;	
			case DPN_MSGID_REMOVE_PLAYER_FROM_GROUP	: 
				break;
			case DPN_MSGID_RETURN_BUFFER			: 
				break;	
			case DPN_MSGID_SEND_COMPLETE		: 
				break;		
			case DPN_MSGID_SERVER_INFO			: 
				break;	
			case DPN_MSGID_TERMINATE_SESSION	: 
				break;		
			// Messages added for DirectX 9
			case DPN_MSGID_CREATE_THREAD		: 
				break;		
			case DPN_MSGID_DESTROY_THREAD		: 
				break;	
			case DPN_MSGID_NAT_RESOLVER_QUERY	: 
				break;	
			}
			int v=0;
		}
		break;

	}

	return S_OK;
}

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
			if(dwReason & DPNDESTROYPLAYERREASON_NORMAL){
				ExecuteInterfaceCommand(PNC_INTERFACE_COMMAND_INFO_PLAYER_EXIT, clientMissionDescription.playersData[idx].name());
			}
			else {
				ExecuteInterfaceCommand(PNC_INTERFACE_COMMAND_INFO_PLAYER_DISCONNECTED, clientMissionDescription.playersData[idx].name());
			}
		}
		//Удаление игрока из clientMD
		clientMissionDescription.disconnectPlayer2PlayerDataByDPNID(dpnid);
	}

}


void PNetCenter::LockInputPacket(void)
{
	flag_LockIputPacket++;
}


PNetCenter::INTERNAL_HOST_ENUM_INFO::INTERNAL_HOST_ENUM_INFO(DPNMSG_ENUM_HOSTS_RESPONSE* pDpn)
{
	timeLastRespond=0;
    pDpn->pAddressSender->Duplicate(&pHostAddr);
    pDpn->pAddressDevice->Duplicate(&pDeviceAddr);

    pAppDesc = new DPN_APPLICATION_DESC;
    ZeroMemory(pAppDesc, sizeof(DPN_APPLICATION_DESC) );
    memcpy(pAppDesc, pDpn->pApplicationDescription, sizeof(DPN_APPLICATION_DESC));
    if(pDpn->pApplicationDescription->pwszSessionName ) {
        pAppDesc->pwszSessionName = new WCHAR[ wcslen(pDpn->pApplicationDescription->pwszSessionName)+1 ];
        wcscpy(pAppDesc->pwszSessionName, pDpn->pApplicationDescription->pwszSessionName );
    }
}
PNetCenter::INTERNAL_HOST_ENUM_INFO::~INTERNAL_HOST_ENUM_INFO()
{
	if(pAppDesc)
	{
		if(pAppDesc->pwszSessionName)
			delete pAppDesc->pwszSessionName;

		delete pAppDesc;
	}
	RELEASE(pHostAddr);
	RELEASE(pDeviceAddr);
}


bool PNetCenter::IsEmptyInternalCommandList(void)
{
	return internalCommandList.empty();
}

