#include "StdAfx.h"
#include "P2P_interface.h"
#include "GS_interface.h"


#include "GameShell.h"
#include "Universe.h"

#include "Lmcons.h"

#include "..\terra\terra.h"

#include <algorithm>

#include "dxerr9.h"

#include "P2P_interfaceAux.h"

WCHAR wstrSession[] = L"Session1";
WCHAR wstrPlayer[]  = L"Player1";

LPCTSTR lpszSignatureRQ = "Perimeter MP client. Version 3";
LPCTSTR lpszSignatureRPL = "Perimeter MP server. Version 3";


#define XDP_CHECK_HR(hr, msg) {if(FAILED(hr)) ErrH.Abort(msg);}

#define NO_PERIMETR_DEFAULT_PORT

//#define IP1(x) ((x>>24) & 0xff)
//#define IP2(x) ((x>>16) & 0xff)
//#define IP3(x) ((x>>8) & 0xff)
//#define IP4(x) (x & 0xff)

#define IP1(x) (x & 0xff)
#define IP2(x) ((x>>8) & 0xff)
#define IP3(x) ((x>>16) & 0xff)
#define IP4(x) ((x>>24) & 0xff)

const unsigned int PERIMETER_MAX_NETWORK_PLAYER=20;
// {A0B71D62-8E88-4767-8439-184327615B42}
const GUID guidPerimeterGame = 
{ 0xa0b71d62, 0x8e88, 0x4767, { 0x84, 0x39, 0x18, 0x43, 0x27, 0x61, 0x5b, 0x42 } };

//#define INITGUID
#include "objbase.h"
#include "initguid.h"
#include "dp8sim.h"


//HRESULT WINAPI DirectPlayMessageHandler(PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer)
//{
//	return ((XDPConnection*)pvUserContext)->DirectPlayMessageHandler(dwMessageId, pMsgBuffer);
//}


//////////////////////////////////////////////////////////////////////
//

/*
CRITICAL_SECTION  _csMemLock;

void* XDPmalloc(int size)
{
	CAutoLock _lock(&_csMemLock);

	return new char[size];
}
void XDPfree(void* p)
{
	CAutoLock _lock(&_csMemLock);

	delete p;
}


void* operator new(size_t nSize)
{
	return XDPmalloc(nSize);
}
void operator delete(void *p)
{
	XDPfree(p);
}
*/

//////////////////////////////////////////////////////////////////////

void XDPInit()
{
	CoInitializeEx(0, COINIT_MULTITHREADED);

	//InitializeCriticalSection(&_csMemLock);
}
void XDPClose()
{
	CoUninitialize();

	//DeleteCriticalSection(&_csMemLock);
}

///////////////////////////////////////////////////////////////////////
// XDPConnection
/*
XDPConnection::XDPConnection(PFNDPNMESSAGEHANDLER pfn, void* pData)
{


	///m_internal_queue[XDP_DPNID_PLAYER_GENERAL] = PacketListType();




}
XDPConnection::~XDPConnection()
{
	Close();

	///m_internal_queue.clear();
}
*/

bool PNetCenter::Init(void)
{
    HRESULT hr;
//	hr = CoCreateInstance(CLSID_DirectPlay8Peer, NULL, 
//                          CLSCTX_INPROC_SERVER,
//                          IID_IDirectPlay8Peer, 
//                          (LPVOID*) &m_pDPServer);
	hr = CoCreateInstance(CLSID_DirectPlay8Peer, NULL, 
                          CLSCTX_INPROC_SERVER,
                          IID_IDirectPlay8Peer, 
                          (LPVOID*) &m_pDPPeer);
	XDP_CHECK_HR(hr, "CoCreateInstance");
	if(hr!=S_OK) return 0;

	//m_pDPClient=m_pDPServer;//Пока

    // Init IDirectPlay8Server
	DWORD dwFlags=0;
	if(workMode==PNCWM_LAN) dwFlags|=DPNINITIALIZE_HINT_LANSESSION;
    hr = m_pDPPeer->Initialize(this, ::DirectPlayMessageHandler, dwFlags);
	XDP_CHECK_HR(hr, "Initialize");
	if(hr!=S_OK) return 0;


	DPN_SP_CAPS dpnspCaps;
	memset(&dpnspCaps, 0, sizeof(DPN_SP_CAPS));
	dpnspCaps.dwSize = sizeof(DPN_SP_CAPS);
	hr = m_pDPPeer->GetSPCaps(&CLSID_DP8SP_TCPIP, &dpnspCaps, 0);
	if ( FAILED( hr ) ) {
		//DXTRACE_ERR_MSGBOX( TEXT("GetSPCaps"), hr );
		return 0;
	}
	//if(dpnspCaps.dwFlags&DPNSPCAPS_SUPPORTSDPNSRV) ;// & DPNSPCAPS_SUPPORTSALLADAPTERS


	//Save DP connection default
	DPN_CAPS_EX caps_ex;
	caps_ex.dwSize=sizeof(DPN_CAPS_EX);
	m_pDPPeer->GetCaps((DPN_CAPS*)&caps_ex,0);
	IniManager("Network.ini").putInt("DPInfo", "ConnectTimeout", caps_ex.dwConnectTimeout);
	IniManager("Network.ini").putInt("DPInfo", "ConnectRetries", caps_ex.dwConnectRetries);

	int connectTimeout;
	int connectRetries;
	const char* section= (workMode==PNCWM_LAN) ? "LANSet" : "INETSet";
	if(IniManager("Network.ini", false).getInt(section, "ConnectTimeout", connectTimeout) &&
		IniManager("Network.ini", false).getInt(section, "ConnectRetries", connectRetries)){

		caps_ex.dwConnectTimeout=connectTimeout;
		caps_ex.dwConnectRetries=connectRetries;
		m_pDPPeer->SetCaps((DPN_CAPS*)&caps_ex, 0);
	}

	SetConnectionTimeout(TIMEOUT_DISCONNECT); //30s //3600000

	return 1;
}

int PNetCenter::ServerStart(const char* _name, int port)
{
	//m_mode = DP_SERVER;

    HRESULT hr;
    PDIRECTPLAY8ADDRESS pDP8AddrLocal = NULL;

    // Create IDirectPlay8Server
//    hr = CoCreateInstance(CLSID_DirectPlay8Server, NULL, 
//                          CLSCTX_INPROC_SERVER,
//                          IID_IDirectPlay8Server, 
//                          (LPVOID*) &m_pDPServer);
///	hr = CoCreateInstance(CLSID_DirectPlay8Peer, NULL, 
///                          CLSCTX_INPROC_SERVER,
///                          IID_IDirectPlay8Peer, 
///                          (LPVOID*) &m_pDPServer);
///	XDP_CHECK_HR(hr, "CoCreateInstance");
///
///    // Init IDirectPlay8Server
///    hr = m_pDPServer->Initialize(this, ::DirectPlayMessageHandler, 0 );
///	XDP_CHECK_HR(hr, "Initialize");

    hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, 
                           CLSCTX_ALL, IID_IDirectPlay8Address, 
                           (LPVOID*) &pDP8AddrLocal);
	XDP_CHECK_HR(hr, "CoCreateInstance");


	if(flag_NetworkSimulation)
		hr=pDP8AddrLocal->SetSP(&CLSID_NETWORKSIMULATOR_DP8SP_TCPIP);
	else
		hr=pDP8AddrLocal->SetSP(&CLSID_DP8SP_TCPIP);
	XDP_CHECK_HR(hr, "SetSP");
	

    // Add the port to pDP8AddrLocal, if the port is non-zero.
/*    if(port) {
#ifndef NO_PERIMETR_DEFAULT_PORT
        hr = pDP8AddrLocal->AddComponent(DPNA_KEY_PORT, &port, sizeof(port), DPNA_DATATYPE_DWORD);
		XDP_CHECK_HR(hr, "AddComponent");
#endif
    }*/
    if(m_dwPort) {
        hr = pDP8AddrLocal->AddComponent(DPNA_KEY_PORT, &m_dwPort, sizeof(m_dwPort), DPNA_DATATYPE_DWORD);
		//XDP_CHECK_HR(hr, "AddComponent");
		if( FAILED(hr) ){
			DXTRACE_ERR_MSGBOX( TEXT("PNetCenter::ServerStart-AddComponent_PORT"), hr );
		}
    }

	///DWORD dwTraversalMode = DPNA_TRAVERSALMODE_PORTRECOMMENDED;
	///hr = pDP8AddrLocal->AddComponent(DPNA_KEY_TRAVERSALMODE, &dwTraversalMode, sizeof(dwTraversalMode), DPNA_DATATYPE_DWORD);



    WCHAR* pSessionName = new WCHAR[strlen(_name) + 1];
	MultiByteToWideChar(CP_ACP, 0, _name, -1, pSessionName, strlen(_name) + 1);


    DPN_APPLICATION_DESC dpnAppDesc;
    ZeroMemory(&dpnAppDesc, sizeof(DPN_APPLICATION_DESC));
    dpnAppDesc.dwSize           = sizeof( DPN_APPLICATION_DESC );
    dpnAppDesc.dwFlags          = 0;//DPNSESSION_CLIENT_SERVER;
	///dpnAppDesc.dwFlags |= DPNSESSION_MIGRATE_HOST;
	///dpnAppDesc.dwFlags |= DPNSESSION_NODPNSVR;
	if(m_DPSigningLevel==1)			dpnAppDesc.dwFlags|= DPNSESSION_FAST_SIGNED;
	else if(m_DPSigningLevel==2)	dpnAppDesc.dwFlags|= DPNSESSION_FULL_SIGNED;
	if(flag_HostMigrate) dpnAppDesc.dwFlags |= DPNSESSION_MIGRATE_HOST;
	if(flag_NoUseDPNSVR) dpnAppDesc.dwFlags |= DPNSESSION_NODPNSVR;
    dpnAppDesc.guidApplication  = guidPerimeterGame;
    dpnAppDesc.pwszSessionName  = pSessionName;
	dpnAppDesc.dwMaxPlayers		= PERIMETER_MAX_NETWORK_PLAYER;

    //hr = m_pDPServer->Host(&dpnAppDesc, &pDP8AddrLocal, 1, NULL, NULL, NULL, DPNHOST_OKTOQUERYFORADDRESSING );
    hr = m_pDPPeer->Host(&dpnAppDesc, &pDP8AddrLocal, 1, NULL, NULL, NULL, DPNHOST_OKTOQUERYFORADDRESSING );
	XDP_CHECK_HR(hr, "Can't start server");
	flag_connected=1;

	delete [] pSessionName;

    pDP8AddrLocal->Release();

	//for internet получение адреса host-а
/*	IDirectPlay8Address *pDP8AddressHost = NULL;
	DWORD dwNumAddresses = 1;

	hr = m_pDPPeer->GetLocalHostAddresses(&pDP8AddressHost, &dwNumAddresses, DPNGETLOCALHOSTADDRESSES_COMBINED);


	char *szHostAddress = NULL;
	DWORD dwNumHostAddressChars = 0;
	hr = pDP8AddressHost->GetURLA(NULL, &dwNumHostAddressChars);
	szHostAddress = new char [dwNumHostAddressChars];// LocalAlloc(LPTR, dwNumHostAddressChars * sizeof(char));
	hr = pDP8AddressHost->GetURLA(szHostAddress, &dwNumHostAddressChars);
*/


    return 1;
}
//int XDPConnection::Started()
//{
//	return m_mode == DP_SERVER;
//}

GUID PNetCenter::getHostGUIDInstance()
{
    HRESULT hr;
    DWORD dwSize = 0;
    //hr = m_pDPServer->GetApplicationDesc( NULL, &dwSize, NULL );
    hr = m_pDPPeer->GetApplicationDesc( NULL, &dwSize, NULL );
	

    // DirectPlay should return BufferTooSmall to give the correct allocation size
//    if( hr != DPNERR_BUFFERTOOSMALL )
//       return hr;

    // Allocate the memory
    DPN_APPLICATION_DESC* pAppDesc = (DPN_APPLICATION_DESC*) new BYTE[ dwSize ];

//    if( NULL == pAppDesc )
//        return E_OUTOFMEMORY;

    // Initialize the struct
    ZeroMemory( pAppDesc, dwSize );
    pAppDesc ->dwSize = sizeof(DPN_APPLICATION_DESC);
    
    // Get group info
	//hr = m_pDPServer->GetApplicationDesc( pAppDesc, &dwSize, NULL );
	hr = m_pDPPeer->GetApplicationDesc( pAppDesc, &dwSize, NULL );

	return pAppDesc->guidInstance;
}


void PNetCenter::SetConnectionTimeout(int ms)
{
/*	DPN_CAPS caps;
	caps.dwSize = sizeof(DPN_CAPS);

	switch(m_mode) {
	case DP_SERVER:
		m_pDPServer->GetCaps(&caps, 0);
		caps.dwTimeoutUntilKeepAlive = ms;
		caps.dwConnectRetries =1;
		m_pDPServer->SetCaps(&caps, 0);
		break;

	case DP_CLIENT:
		m_pDPClient->GetCaps(&caps, 0);
		caps.dwTimeoutUntilKeepAlive = ms;
		m_pDPClient->SetCaps(&caps, 0);
		break;
	}
*/
	//m_pDPPeer->GetCaps(&caps, 0);
	//caps.dwTimeoutUntilKeepAlive = ms;
	//caps.dwConnectRetries =1;
	//m_pDPPeer->SetCaps(&caps, 0);

	const int MaxSendRetryInterval=5000;//Default direct play retry interval
	const int NumSendRetry=ms/MaxSendRetryInterval;
	const int TimeoutUntilKeepAlive=MaxSendRetryInterval*NumSendRetry/2;
	DPN_CAPS_EX caps_ex;
	caps_ex.dwSize=sizeof(DPN_CAPS_EX);
	m_pDPPeer->GetCaps((DPN_CAPS*)&caps_ex,0);
	caps_ex.dwNumSendRetries=NumSendRetry;
	caps_ex.dwMaxSendRetryInterval=MaxSendRetryInterval;
	caps_ex.dwTimeoutUntilKeepAlive=TimeoutUntilKeepAlive;
	m_pDPPeer->SetCaps((DPN_CAPS*)&caps_ex, 0);

}
int PNetCenter::GetConnectionTimeout(void)
{
	//DPN_CAPS caps;
	//caps.dwSize = sizeof(DPN_CAPS);
	//m_pDPPeer->GetCaps(&caps, 0);
	//return caps.dwTimeoutUntilKeepAlive;
	DPN_CAPS_EX caps_ex;
	caps_ex.dwSize=sizeof(DPN_CAPS_EX);
	m_pDPPeer->GetCaps((DPN_CAPS*)&caps_ex,0);
	return caps_ex.dwTimeoutUntilKeepAlive;
}

/*void XDPConnection::GetActivePlayers(list<DPNID>& players)
{
	if(m_mode == DP_SERVER)
	{
		HRESULT hr;
		DWORD dwNumPlayers = 0;
		DPNID* aPlayers = 0;

	    // Enumerate all the connected players
	    while(1)
		{
			//hr = m_pDPServer->EnumPlayersAndGroups( aPlayers, &dwNumPlayers, DPNENUM_PLAYERS );
			hr = m_pDPPeer->EnumPlayersAndGroups( aPlayers, &dwNumPlayers, DPNENUM_PLAYERS );
			
			if(SUCCEEDED(hr))
				break;

			if(FAILED(hr) && hr != DPNERR_BUFFERTOOSMALL)
				return;

			if(aPlayers)
				delete aPlayers;

			aPlayers = new DPNID[dwNumPlayers];
		}

		for(int i=0; i<dwNumPlayers; i++)
			if(m_internal_queue.find(aPlayers[i]) != m_internal_queue.end())
				players.push_back(aPlayers[i]);

		if(aPlayers)
			delete aPlayers;
	}
}*/

void PNetCenter::RemovePlayer(DPNID dpnid)
{
//	if(m_mode == DP_SERVER)
	{
		//HRESULT hr = m_pDPServer->DestroyClient(dpnid, 0, 0, 0);
		//HRESULT hr = m_pDPServer->DestroyPeer(dpnid, 0, 0, 0);
		if(isHost() && dpnid==m_localDPNID && dpnid==m_hostDPNID){
			ExecuteInternalCommand(PNC_COMMAND__END_GAME, false);
			ExecuteInterfaceCommand(PNC_INTERFACE_COMMAND_HOST_TERMINATED_GAME);
		}
		else {
			char destroyInfo[]={'t', '1', 0};
			HRESULT hr = m_pDPPeer->DestroyPeer(dpnid, destroyInfo, sizeof(destroyInfo), 0);
			if( FAILED( hr ) )
				DXTRACE_ERR_MSGBOX( TEXT("DestroyPeer(RemovePlayer)"), hr );
		}


		/*int k;
		if(hr==DPNERR_INVALIDPARAM)
			k=0;
		else if(hr==DPNERR_INVALIDPLAYER)
			k=1;
		else if(hr==DPNERR_NOTHOST)
			k=2;
		else if(hr==S_OK)
			k=3;*/

	}

}
/*
DPNID XDPConnection::CreateGroup()
{
	if(m_mode == DP_SERVER)
	{
		DPN_GROUP_INFO dgi;
		memset(&dgi, 0, sizeof(DPN_GROUP_INFO));
		dgi.dwSize = sizeof(DPN_GROUP_INFO);
		dgi.dwGroupFlags = DPNGROUP_AUTODESTRUCT;

		m_dpnidGroupCreating = -1;
		//if(SUCCEEDED(m_pDPServer->CreateGroup(&dgi, 0, 0, 0, DPNCREATEGROUP_SYNC))) return m_dpnidGroupCreating;
		if(SUCCEEDED(m_pDPPeer->CreateGroup(&dgi, 0, 0, 0, DPNCREATEGROUP_SYNC))) return m_dpnidGroupCreating;
		
	}

	return 0;
}
void XDPConnection::DestroyGroup(DPNID dpnid)
{
	//if(m_mode == DP_SERVER) m_pDPServer->DestroyGroup(dpnid, 0, 0, DPNDESTROYGROUP_SYNC);
	if(m_mode == DP_SERVER) m_pDPPeer->DestroyGroup(dpnid, 0, 0, DPNDESTROYGROUP_SYNC);

}
void XDPConnection::AddPlayerToGroup(DPNID group, DPNID player)
{
	if(m_mode == DP_SERVER) {
		DPNHANDLE hAsync;
		//m_pDPServer->AddPlayerToGroup(group, player, 0, &hAsync, 0);
		m_pDPPeer->AddPlayerToGroup(group, player, 0, &hAsync, 0);
	}
}
void XDPConnection::DelPlayerFromGroup(DPNID group, DPNID player)
{
	if(m_mode == DP_SERVER) {
		DPNHANDLE hAsync;
		//m_pDPServer->RemovePlayerFromGroup(group, player, 0, &hAsync, 0);
		m_pDPPeer->RemovePlayerFromGroup(group, player, 0, &hAsync, 0);
	}
}*/

void PNetCenter::SetServerInfo(void* pb, int sz)
{
//	if(m_mode == DP_SERVER)
	{
/*		DPN_PLAYER_INFO dpi;
		memset(&dpi, 0, sizeof(DPN_PLAYER_INFO));
		dpi.dwSize = sizeof(DPN_PLAYER_INFO);
		dpi.dwInfoFlags = DPNINFO_DATA;
		dpi.pvData = pb;
		dpi.dwDataSize = sz;

		DPNHANDLE hAsync;

		HRESULT hr = m_pDPServer->SetServerInfo(&dpi, 0, &hAsync, 0);
		if(hr != DPNSUCCESS_PENDING)
			ErrH.Abort("Cant set server info!");
*/
	}
}
int PNetCenter::GetServerInfo(void* pb)
{
/*	if(m_mode != DP_CLIENT)
		return 0;

	DWORD dwSize = 0;

	HRESULT hr = m_pDPClient->GetServerInfo(0, &dwSize, 0);

	DPN_PLAYER_INFO* pDpi = new DPN_PLAYER_INFO[dwSize];
	pDpi->dwSize = sizeof(DPN_PLAYER_INFO);

	m_pDPClient->GetServerInfo(pDpi, &dwSize, 0);

	memcpy(pb, pDpi->pvData, dwSize);
	delete pDpi;

	return dwSize;
*/
	return 0;
}

bool PNetCenter::GetConnectionInfo(DPN_CONNECTION_INFO& info)
{
/*	if(!Connected() || (m_mode  != DP_CLIENT))
		return 0;

	memset(&info, 0, sizeof(DPN_CONNECTION_INFO));
	info.dwSize = sizeof(DPN_CONNECTION_INFO);

	return SUCCEEDED(m_pDPClient->GetConnectionInfo(&info, 0));
*/
	return 0;
}
void PNetCenter::TerminateSession()
{
}

void PNetCenter::Close(bool flag_immediatle)
{
/*	switch(m_mode) {
	case DP_SERVER:
		m_pDPServer->Close(DPNCLOSE_IMMEDIATE);
		RELEASE(m_pDPServer);
		break;
	case DP_CLIENT:
		m_pDPClient->Close(DPNCLOSE_IMMEDIATE);
		RELEASE(m_pDPClient);
		break;
	}
	RELEASE(m_pDPServer);*/
	if(flag_immediatle) m_pDPPeer->Close(DPNCLOSE_IMMEDIATE);
	else m_pDPPeer->Close(0);
	RELEASE(m_pDPPeer);

	//m_mode = DP_NOBODY;
	flag_connected=0;
}

int PNetCenter::Connect(GUID _hostID) //const char* lpszHost, int port)
{
	//m_mode = DP_CLIENT;

	HRESULT hr;

	m_nClientSgnCheckError = 0;

    // Create IDirectPlay8Client
//    hr = CoCreateInstance(CLSID_DirectPlay8Client, 0, 
//                          CLSCTX_INPROC_SERVER,
//                          IID_IDirectPlay8Client, 
//                          (LPVOID*) &m_pDPClient);
///	hr = CoCreateInstance(CLSID_DirectPlay8Peer, 0, 
///							CLSCTX_INPROC_SERVER,
///							IID_IDirectPlay8Peer, 
///							(LPVOID*) &m_pDPClient);
///	XDP_CHECK_HR(hr, "CoCreateInstance");
///
///    // Init IDirectPlay8Client
///    hr = m_pDPClient->Initialize(this, ::DirectPlayMessageHandler, 0);
///	XDP_CHECK_HR(hr, "Initialize");

//	FindHost(lpszHost);
//	if(!m_pLastHostFound)
//		return 0;

	INTERNAL_HOST_ENUM_INFO*  pHostFound=0;
	vector<INTERNAL_HOST_ENUM_INFO*>::iterator p;

	{
		CAutoLock _Lock(&m_GeneralLock); //! Lock
		for(p=internalFoundHostList.begin(); p!=internalFoundHostList.end(); p++){
			if((*p)->pAppDesc->guidInstance==_hostID) {
				pHostFound=*p;
				break;
			}
		}
	}
	if(p==internalFoundHostList.end()) {
//		FindHost("");
//		CAutoLock _Lock(&m_GeneralLock); //! Lock
//		for(p=internalFoundHostList.begin(); p!=internalFoundHostList.end(); p++){
//			if((*p)->pAppDesc->guidInstance==_hostID) {
//				pHostFound=*p;
//				break;
//			}
//		}
//		if(p==internalFoundHostList.end()) 
			return 0;
	}


    // Set the peer info
    DPN_PLAYER_INFO dpPlayerInfo;
    ZeroMemory(&dpPlayerInfo, sizeof(DPN_PLAYER_INFO));
    dpPlayerInfo.dwSize = sizeof(DPN_PLAYER_INFO);
    dpPlayerInfo.dwInfoFlags = DPNINFO_NAME;
    dpPlayerInfo.pwszName = wstrPlayer;

    //hr = m_pDPClient->SetClientInfo(&dpPlayerInfo, 0, 0, DPNOP_SYNC);
    //hr = m_pDPClient->SetPeerInfo(&dpPlayerInfo, 0, 0, DPNOP_SYNC);
    hr = m_pDPPeer->SetPeerInfo(&dpPlayerInfo, 0, 0, DPNOP_SYNC);
	XDP_CHECK_HR(hr, "SetPeerInfo");

    // Connect to an existing session.
//    hr = m_pDPClient->Connect(m_pLastHostFound->pAppDesc,    // the application desc
//                              m_pLastHostFound->pHostAddr,   // address of the host of the session
//                              m_pLastHostFound->pDeviceAddr, // address of the local device the enum responses were received on
//                              0, 0,                      // DPN_SECURITY_DESC, DPN_SECURITY_CREDENTIALS
//                              lpszSignatureRQ, strlen(lpszSignatureRQ), // user data, user data size
//                              0, 0,                      // async context, async handle,
//                              DPNCONNECT_SYNC); // flags
//	hr = m_pDPClient->Connect(pHostFound->pAppDesc,    // the application desc
//								pHostFound->pHostAddr,   // address of the host of the session
//								pHostFound->pDeviceAddr, // address of the local device the enum responses were received on
//								0, 0,                      // DPN_SECURITY_DESC, DPN_SECURITY_CREDENTIALS
//								lpszSignatureRQ, strlen(lpszSignatureRQ), // user data, user data size
//								0, 0, 0,                      // player context, async context, async handle,
//								DPNCONNECT_SYNC); // flags
	//init connectPlayer data
	static sConnectInfo connectInfo;
	static sDigitalGameVersion dgv(true);
	connectInfo.set(internalPlayerData, gamePassword.c_str(), dgv);

	hr = m_pDPPeer->Connect(pHostFound->pAppDesc,    // the application desc
								pHostFound->pHostAddr,   // address of the host of the session
								pHostFound->pDeviceAddr, // address of the local device the enum responses were received on
								0, 0,                      // DPN_SECURITY_DESC, DPN_SECURITY_CREDENTIALS
								&connectInfo, sizeof(connectInfo), // user data, user data size
								0, 0, 0,                      // player context, async context, async handle,
								DPNCONNECT_SYNC); // flags
	///XDP_CHECK_HR(hr, "DP Connect filed");
    if( FAILED( hr ) ){
        DXTRACE_ERR_MSGBOX( TEXT("PNetCenter::Connect-Connect"), hr );
		flag_connected=0;
		return 0;
	}
	flag_connected=1;

	xassert( (m_nClientSgnCheckError==0) &&"Signature check error!");

	return 1;
}

int PNetCenter::Connect(unsigned int ip)//, int port
{
	//if(!ip)
	//	return Connect((const char*)0, port);

	///ServerStart("qqq", PERIMETER_DEFAULT_PORT);
	char ip_string[17];
	memset(ip_string, 0, 17);
	sprintf(ip_string, "%d.%d.%d.%d", IP1(ip), IP2(ip), IP3(ip), IP4(ip));

	{
		///clearFoundHostList();//Вызывается в StopFindHost
		StopFindHostDP();
	}

	///Sleep(2000);

	///FindHost(ip_string);
	StartFindHostDP(ip_string);
	unsigned int CONST_MAX_TIME_SLEEP=10000;
	unsigned int CONST_MIN_TIME_SLEEP=400;
	unsigned int STEP_TESTING=200;
	unsigned int time=0;
	while(time<CONST_MAX_TIME_SLEEP){
		time+=STEP_TESTING;
		Sleep(STEP_TESTING);
		{
			CAutoLock _Lock(&m_GeneralLock); //! Lock
			if(!internalFoundHostList.empty()) {
				if(time > CONST_MIN_TIME_SLEEP) break;
			}
		}
	}

	///reStartFindPlayers(ip);
	///Sleep(7000);


//	if(!m_pLastHostFound)
//		return 0;
	GUID hostID;
	{
		CAutoLock _Lock(&m_GeneralLock); //! Lock
		if(internalFoundHostList.empty()) return 0;
		else hostID=(*internalFoundHostList.begin())->pAppDesc->guidInstance;
	}
	return Connect(hostID);
}

void PNetCenter::StartConnect2IP(unsigned int ip)//, int port
{
	char ip_string[17];
	memset(ip_string, 0, 17);
	sprintf(ip_string, "%d.%d.%d.%d", IP1(ip), IP2(ip), IP3(ip), IP4(ip));

	{
		///clearFoundHostList();//Вызывается в StopFindHost
		StopFindHostDP();
	}

	StartFindHostDP(ip_string);
}

bool PNetCenter::QuantConnect2IP()
{
/*	unsigned int CONST_MAX_TIME_SLEEP=10000;
	unsigned int CONST_MIN_TIME_SLEEP=400;
	unsigned int STEP_TESTING=200;
	unsigned int time=0;
	while(time<CONST_MAX_TIME_SLEEP){
		time+=STEP_TESTING;
		Sleep(STEP_TESTING);
		{
			CAutoLock _Lock(&m_GeneralLock); //! Lock
			if(!internalFoundHostList.empty()) {
				if(time > CONST_MIN_TIME_SLEEP) break;
			}
		}
	}*/

	GUID hostID;
	{
		CAutoLock _Lock(&m_GeneralLock); //! Lock
		if(internalFoundHostList.empty()) return 0;
		else hostID=(*internalFoundHostList.begin())->pAppDesc->guidInstance;
	}
	return Connect(hostID);
}

bool PNetCenter::isConnected()
{
	return flag_connected;//(m_mode != DP_NOBODY);
}


int PNetCenter::Send(const char* buffer, int size, DPNID dpnid, bool flag_guaranted)
{

	DPNHANDLE hAsync;
	DPN_BUFFER_DESC bufferDesc;
	bufferDesc.dwBufferSize = size;
	bufferDesc.pBufferData  = (BYTE*)buffer;

/*	switch(m_mode)
	{
	case DP_CLIENT:
		//m_pDPClient->Send(&bufferDesc, 1, 0, 0, &hAsync, DPNSEND_GUARANTEED);
		//m_pDPClient->SendTo(m_hostDPNID, &bufferDesc, 1, 0, 0, &hAsync, DPNSEND_GUARANTEED);
		m_pDPPeer->SendTo(m_hostDPNID, &bufferDesc, 1, 0, 0, &hAsync, DPNSEND_GUARANTEED);
		break;

	case DP_SERVER:
		//m_pDPServer->SendTo(dpnid == 0xFFFFFFFF ? DPNID_ALL_PLAYERS_GROUP : dpnid, &bufferDesc, 1,
		//					0, 0, &hAsync, DPNSEND_NOLOOPBACK|DPNSEND_GUARANTEED);
		m_pDPPeer->SendTo(dpnid == 0xFFFFFFFF ? DPNID_ALL_PLAYERS_GROUP : dpnid, &bufferDesc, 1,
							0, 0, &hAsync, DPNSEND_NOLOOPBACK|DPNSEND_GUARANTEED);
		break;
	}*/
	if(flag_guaranted){
		m_pDPPeer->SendTo(dpnid, &bufferDesc, 1,
								0, NULL, &hAsync, DPNSEND_NOLOOPBACK|DPNSEND_GUARANTEED);
	}
	else {
		m_pDPPeer->SendTo(dpnid, &bufferDesc, 1,
								1000, NULL, &hAsync, DPNSEND_COALESCE|DPNSEND_NOLOOPBACK); //1000ms
	}

	return size;
}

bool PNetCenter::FindHost(const char* lpszHost)
{
	bool result=1;

    HRESULT hr;

    DPN_APPLICATION_DESC   dpnAppDesc;
    IDirectPlay8Address*   pDP8AddressHost  = NULL;
    IDirectPlay8Address*   pDP8AddressLocal = NULL;
    WCHAR*                 wszHostName      = NULL;

    // Create the local device address object
    hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, 
                          CLSCTX_ALL, IID_IDirectPlay8Address,
                          (LPVOID*) &pDP8AddressLocal);
	XDP_CHECK_HR(hr, "CoCreateInstance");

    // Set IP service provider
	if(flag_NetworkSimulation)
		hr=pDP8AddressLocal->SetSP(&CLSID_NETWORKSIMULATOR_DP8SP_TCPIP);
    else 
		hr=pDP8AddressLocal->SetSP(&CLSID_DP8SP_TCPIP);
	XDP_CHECK_HR(hr, "SetSP");

    // Create the remote host address object
    hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, 
                          CLSCTX_ALL, IID_IDirectPlay8Address,
                          (LPVOID*) &pDP8AddressHost);
	XDP_CHECK_HR(hr, "CoCreateInstance");

    // Set IP service provider
	if(flag_NetworkSimulation)
		hr=pDP8AddressHost->SetSP(&CLSID_NETWORKSIMULATOR_DP8SP_TCPIP);
	else 
		hr=pDP8AddressHost->SetSP(&CLSID_DP8SP_TCPIP);
	XDP_CHECK_HR(hr, "SetSP");

    // Set the remote host name (if provided)
    if(lpszHost && strlen(lpszHost)) {
        wszHostName = new WCHAR[strlen(lpszHost) + 1];
		MultiByteToWideChar(CP_ACP, 0, lpszHost, -1, wszHostName, strlen(lpszHost) + 1);

        hr = pDP8AddressHost->AddComponent(DPNA_KEY_HOSTNAME, wszHostName, 
                                           (wcslen(wszHostName) + 1)*sizeof(WCHAR), 
                                           DPNA_DATATYPE_STRING);
		XDP_CHECK_HR(hr, "AddComponent");


#ifndef NO_PERIMETR_DEFAULT_PORT
		DWORD port=PERIMETER_DEFAULT_PORT;
		hr = pDP8AddressHost->AddComponent( DPNA_KEY_PORT, &port, sizeof(port), DPNA_DATATYPE_DWORD );
		XDP_CHECK_HR(hr, "AddComponent");
#endif

		/*
        // If a port was specified in the IP string, then add it.
        // Games will typically hard code the port so the user need not know it
        if( dwPort != 0 )
        {
            hr = pDP8AddressHost->AddComponent( DPNA_KEY_PORT, 
                                                &dwPort, sizeof(dwPort),
                                                DPNA_DATATYPE_DWORD );
            if( FAILED(hr) )
            {
                DXTRACE_ERR( TEXT("AddComponent"), hr );
                goto LCleanup;
            }
        }
		*/
    }

	if(m_dwPort) {
		hr = pDP8AddressHost->AddComponent(DPNA_KEY_PORT, &m_dwPort, sizeof(m_dwPort), DPNA_DATATYPE_DWORD);
		//XDP_CHECK_HR(hr, "AddComponent");
		if( FAILED(hr) ){
			DXTRACE_ERR_MSGBOX( TEXT("PNetCenter::FindHost-AddComponent_PORT"), hr );
		}
	}


    ZeroMemory(&dpnAppDesc, sizeof(DPN_APPLICATION_DESC));
    dpnAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
    dpnAppDesc.guidApplication = guidPerimeterGame;
	//if(flag_HostMigrate) dpnAppDesc.dwFlags |= DPNSESSION_MIGRATE_HOST; //Похоже тут это делать нельзя (только при создании Ноsta)
	//if(flag_NoUseDPNSVR) dpnAppDesc.dwFlags |= DPNSESSION_NODPNSVR; //Не знаю - можно или нет


    // Enumerate all StressMazeApp hosts running on IP service providers
    //hr = m_pDPClient->EnumHosts(&dpnAppDesc, pDP8AddressHost, 
    //                            pDP8AddressLocal, NULL, 
    //                            0, 1, 0, 0, this, 
    //                            0, DPNENUMHOSTS_SYNC);
    hr = m_pDPPeer->EnumHosts(&dpnAppDesc, pDP8AddressHost, pDP8AddressLocal, 
								NULL, 
                                0, 1, 0, 0, this, 
                                0, DPNENUMHOSTS_SYNC);
	// This will be returned if the ip address is is invalid. 
    if(hr != DPNERR_INVALIDDEVICEADDRESS &&  hr != DPNERR_ADDRESSING) {
		XDP_CHECK_HR(hr, "EnumHosts");
		result=0;
	}

//    hr = m_pDPPeer->EnumHosts(&dpnAppDesc, pDP8AddressHost, pDP8AddressLocal, 
//								NULL, 
//                                0, 
//								INFINITE, 
//								ENUMERATION_HOST_RETRY_INTERVAL, 
//								INFINITE, 
//								this, 
//                                &m_hEnumAsyncOp, 
//								0);
//	if(hr!=DPNSUCCESS_PENDING) result=0;

    RELEASE(pDP8AddressHost);
    RELEASE(pDP8AddressLocal);

	if(wszHostName)
		delete wszHostName;
	return result;
}

bool PNetCenter::StartFindHostDP(const char* lpszHost)
{
	clearInternalFoundHostList();

	bool result=1;
    HRESULT hr;

    DPN_APPLICATION_DESC   dpnAppDesc;
    IDirectPlay8Address*   pDP8AddressHost  = NULL;
    IDirectPlay8Address*   pDP8AddressLocal = NULL;
    WCHAR*                 wszHostName      = NULL;

    // Create the local device address object
    hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, 
                          CLSCTX_ALL, IID_IDirectPlay8Address,
                          (LPVOID*) &pDP8AddressLocal);
	XDP_CHECK_HR(hr, "CoCreateInstance");

    // Set IP service provider
	if(flag_NetworkSimulation)
		hr=pDP8AddressLocal->SetSP(&CLSID_NETWORKSIMULATOR_DP8SP_TCPIP);
	else 
		hr=pDP8AddressLocal->SetSP(&CLSID_DP8SP_TCPIP);
	XDP_CHECK_HR(hr, "SetSP");

    // Create the remote host address object
    hr = CoCreateInstance(CLSID_DirectPlay8Address, NULL, 
                          CLSCTX_ALL, IID_IDirectPlay8Address,
                          (LPVOID*) &pDP8AddressHost);
	XDP_CHECK_HR(hr, "CoCreateInstance");

    // Set IP service provider
	if(flag_NetworkSimulation)
		hr=pDP8AddressHost->SetSP(&CLSID_NETWORKSIMULATOR_DP8SP_TCPIP);
	else
		hr=pDP8AddressHost->SetSP(&CLSID_DP8SP_TCPIP);
	XDP_CHECK_HR(hr, "SetSP");

    // Set the remote host name (if provided)
	if(needHostList.empty()){
		if(lpszHost && strlen(lpszHost)) {
			wszHostName = new WCHAR[strlen(lpszHost) + 1];
			MultiByteToWideChar(CP_ACP, 0, lpszHost, -1, wszHostName, strlen(lpszHost) + 1);

			hr = pDP8AddressHost->AddComponent(DPNA_KEY_HOSTNAME, wszHostName, 
											   (wcslen(wszHostName) + 1)*sizeof(WCHAR), 
											   DPNA_DATATYPE_STRING);
			XDP_CHECK_HR(hr, "AddComponent");

#ifndef NO_PERIMETR_DEFAULT_PORT
			DWORD dwPort=PERIMETER_DEFAULT_PORT;
			hr = pDP8AddressHost->AddComponent( DPNA_KEY_PORT, &dwPort, sizeof(dwPort), DPNA_DATATYPE_DWORD );
			if( FAILED(hr) ){
				DXTRACE_ERR_MSGBOX( TEXT("PNetCenter::StartFindHostDP-AddComponent_PORT"), hr );
			}
#endif
		}
		if(m_dwPort) {
			hr = pDP8AddressHost->AddComponent(DPNA_KEY_PORT, &m_dwPort, sizeof(m_dwPort), DPNA_DATATYPE_DWORD);
			//XDP_CHECK_HR(hr, "AddComponent");
			if( FAILED(hr) ){
				DXTRACE_ERR_MSGBOX( TEXT("PNetCenter::StartFindHostDP-AddComponent_PORT"), hr );
			}
		}

		//DPN_SP_CAPS dpspCaps;
		//ZeroMemory( &dpspCaps, sizeof(DPN_SP_CAPS) );
		//dpspCaps.dwSize = sizeof(DPN_SP_CAPS);
		//m_pDPClient->GetSPCaps(&CLSID_DP8SP_TCPIP, &dpspCaps, 0);

		ZeroMemory(&dpnAppDesc, sizeof(DPN_APPLICATION_DESC));
		dpnAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
		dpnAppDesc.guidApplication = guidPerimeterGame;
		//if(flag_HostMigrate) dpnAppDesc.dwFlags |= DPNSESSION_MIGRATE_HOST; //Похоже тут это делать нельзя (только при создании Ноsta)
		//if(flag_NoUseDPNSVR) dpnAppDesc.dwFlags |= DPNSESSION_NODPNSVR; //Не знаю - можно или нет


		DPNHANDLE m_hEnumAsyncOp;
		hr = m_pDPPeer->EnumHosts(&dpnAppDesc, pDP8AddressHost, pDP8AddressLocal, 
									NULL, 
									0, 
									INFINITE, 
									0,//default//ENUMERATION_HOST_RETRY_INTERVAL, 
									INFINITE, 
									this, 
									&m_hEnumAsyncOp, 
									0);
		// This will be returned if the ip address is is invalid. 
		//if(hr != DPNERR_INVALIDDEVICEADDRESS &&  hr != DPNERR_ADDRESSING) 
		//	XDP_CHECK_HR(hr, "EnumHosts");
		if(hr!=DPNSUCCESS_PENDING) result=0;
		else m_hEnumAsyncOp_Arr.push_back(m_hEnumAsyncOp);
		delete wszHostName;

    }
	else {
		vector<string>::iterator p;
		for(p=needHostList.begin(); p!=needHostList.end(); p++){
			wszHostName = new WCHAR[strlen(p->c_str()) + 1];
			MultiByteToWideChar(CP_ACP, 0, p->c_str(), -1, wszHostName, strlen(p->c_str()) + 1);

			hr = pDP8AddressHost->AddComponent(DPNA_KEY_HOSTNAME, wszHostName, 
											   (wcslen(wszHostName) + 1)*sizeof(WCHAR), 
											   DPNA_DATATYPE_STRING);
			XDP_CHECK_HR(hr, "AddComponent");
			if(m_dwPort) {
				hr = pDP8AddressHost->AddComponent(DPNA_KEY_PORT, &m_dwPort, sizeof(m_dwPort), DPNA_DATATYPE_DWORD);
				//XDP_CHECK_HR(hr, "AddComponent");
				if( FAILED(hr) ){
					DXTRACE_ERR_MSGBOX( TEXT("PNetCenter::StartFindHostDP-AddComponent_PORT"), hr );
				}
			}

			ZeroMemory(&dpnAppDesc, sizeof(DPN_APPLICATION_DESC));
			dpnAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
			dpnAppDesc.guidApplication = guidPerimeterGame;
			//if(flag_HostMigrate) dpnAppDesc.dwFlags |= DPNSESSION_MIGRATE_HOST; //Похоже тут это делать нельзя (только при создании Ноsta)
			//if(flag_NoUseDPNSVR) dpnAppDesc.dwFlags |= DPNSESSION_NODPNSVR; //Не знаю - можно или нет


			DPNHANDLE m_hEnumAsyncOp;
			hr = m_pDPPeer->EnumHosts(&dpnAppDesc, pDP8AddressHost, pDP8AddressLocal, 
										NULL, 
										0, 
										INFINITE, 
										0,//default//ENUMERATION_HOST_RETRY_INTERVAL, 
										INFINITE, 
										this, 
										&m_hEnumAsyncOp, 
										0);
			if(hr!=DPNSUCCESS_PENDING) result=0;
			else m_hEnumAsyncOp_Arr.push_back(m_hEnumAsyncOp);

			delete wszHostName;
		}
	}

    RELEASE(pDP8AddressHost);
    RELEASE(pDP8AddressLocal);

	//if(wszHostName)
	//	delete wszHostName;
	return result;
}

void PNetCenter::StopFindHostDP(void)
{
    HRESULT hr;
//	vector<DPNHANDLE>::iterator p;
//	for(p=m_hEnumAsyncOp_Arr.begin(); p!=m_hEnumAsyncOp_Arr.end(); p++){
//		hr = m_pDPPeer->CancelAsyncOperation( *p, 0);//в случае когда есть хандлеры флаг не нужен ! DPNCANCEL_ENUM;
//		if( FAILED(hr) ){
//			DXTRACE_ERR_MSGBOX( TEXT("PNetCenter::StopFindHostDP-error cancel operation"), hr );
//		}
//	}
	hr = m_pDPPeer->CancelAsyncOperation( NULL, DPNCANCEL_ENUM);//в случае когда указывается флаг хандлеры не нужны(отменяются все) ! ;
	if( FAILED(hr) ){
		DXTRACE_ERR_MSGBOX( TEXT("PNetCenter::StopFindHostDP-error cancel operation"), hr );
	}
	m_hEnumAsyncOp_Arr.clear();


	Sleep(20);//Для завершения back фукции DP(3-й поток)
	clearInternalFoundHostList();
	///clearGameHostList();
}






/*
void XDPConnection::refreshGroupMember()
{
	gameGroupMemberList.clear();
	HRESULT hr;
	DWORD dwSize = 0;
	DPNID * pArrDPNID;
	hr = m_pDPPeer->EnumPlayersAndGroups(NULL, &dwSize, DPNENUM_PLAYERS);
	if( FAILED(hr) && hr != DPNERR_BUFFERTOOSMALL ) xassert(0&&"DirectPlay-EnumPlayersAndGroups error 1");
	pArrDPNID = new DPNID [dwSize];
	ZeroMemory( pArrDPNID, sizeof(DPNID)*dwSize );
	hr = m_pDPPeer->EnumPlayersAndGroups(pArrDPNID, &dwSize, DPNENUM_PLAYERS);
	if( FAILED(hr) ) xassert(0&&"DirectPlay-EnumPlayersAndGroups error 2");
	int i;
	for(i=0; i<dwSize; i++) gameGroupMemberList.push_back(pArrDPNID[i]);
	delete [] pArrDPNID;

}
*/