#ifndef __CONNECTIONDP_H__
#define __CONNECTIONDP_H__

const unsigned int MAX_TIME_INTERVAL_HOST_RESPOND=10000;
const unsigned int ENUMERATION_HOST_RETRY_INTERVAL=400;

////////////////////////////////////////////////////////////////////////

class InOutNetComBuffer;

class CAutoLock
{
    SDL_mutex* m_pCs;

public:
	CAutoLock(SDL_mutex* pCs){
		m_pCs = pCs;
		SDL_LockMutex(m_pCs);
	}
	~CAutoLock(){
		SDL_UnlockMutex(m_pCs);
	}
};


////////////////////////////////////////////////////////////////////////
// XDPacket

struct XDPacket
{
	int   size;
	unsigned char* buffer;
	NETID netid;

	FORCEINLINE XDPacket(){
		size = 0;
		buffer = 0;
		netid = 0;
	}
	FORCEINLINE XDPacket(NETID _netid, int _size, const void* cb){
		netid=_netid;
		size= _size;
		buffer = new unsigned char[size];
		memcpy(buffer, cb, size);
	}
	FORCEINLINE ~XDPacket(){
		if(buffer)
			delete buffer;
	}
	FORCEINLINE void set(NETID _netid, int _size, const void* cb){
		netid=_netid;
		size = _size;
		if(buffer) delete buffer;
		buffer = new unsigned char[size];
		memcpy(buffer, cb, size);
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////
// XDPConnection

void XDPInit();
void XDPClose();

//#define XDP_NETID_PLAYER_GENERAL 0xffffffff

///typedef void (*PFNEVENTPROC)(NETID, void* pData, XDP_Message msg);

/*
class XDPConnection
{
	//directplay server stuff
	//IDirectPlay8Server* m_pDPServer;
	NETID               m_netidGroupCreating;

	//directplay client stuff
	//IDirectPlay8Client*  m_pDPClient;


	///CRITICAL_SECTION     m_csLock;
	CRITICAL_SECTION     m_csLockReSend;
	CRITICAL_SECTION     m_csLockInternalQueue;

	CRITICAL_SECTION     m_LockHostList;



	typedef list<XDPacket> PacketListType;
	typedef hash_map<NETID, PacketListType> DPQueueType;

	DPQueueType m_internal_queue;

	void FindHost(const char* lpszHost);

public:


	//enum DPMode {
	//	DP_NOBODY=0,
	//	DP_CLIENT, 
	//	DP_SERVER
	//};

	//DPMode   m_mode;

	XDPConnection(PFNDPNMESSAGEHANDLER pfn, void* pData);
	~XDPConnection();

	bool Init(void);
	bool StartFindHost(const char* lpszHost);
	void StopFindHost(void);

	void reStartFindPlayers(int IP);


	int ServerStart(const char* _name, int port);
	//int Started();

	void SetConnectionTimeout(int ms);
	int GetConnectionTimeout(void);

	GUID getHostGUIDInstance();

	//int Connect(int ip, int port);
	int Connect(GUID hostID);//const char* lpszHost, int port);
	int Connect(unsigned int ip);//, int port

	bool isConnected();

	int Send(const char* buffer, int size, NETID netid);
	int Receive(char* buffer, int size, NETID netid);

	void InitPlayerQueue(NETID netid);
	//void GetActivePlayers(list<NETID>& players);
	void RemovePlayer(NETID netid);

	//NETID CreateGroup();
	//void DestroyGroup(NETID netid);
	//void AddPlayerToGroup(NETID group, NETID player);
	//void DelPlayerFromGroup(NETID group, NETID player);

	void SetServerInfo(void* pb, int sz);
	int  GetServerInfo(void* pb);

	bool GetConnectionInfo(DPN_CONNECTION_INFO& info);

	void Close();

	///HRESULT DirectPlayMessageHandler(DWORD dwMessageId, PVOID pMsgBuffer);





	//void refreshGroupMember();
	//list<NETID> gameGroupMemberList;

};
*/


#endif //__CONNECTIONDP_H__