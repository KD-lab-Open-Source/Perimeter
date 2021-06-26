
#define hash hash_
#if defined(_MSC_VER) && (_MSC_VER < 1900)
#define slist slist_
#endif

#ifndef PERIMETER_EXODUS
#include "Peer2/peer/peer.h"
#endif

#undef hash
#undef slist

struct sGamePlayerInfo {
	int vvv;
};

class PNetCenter;
class GameSpyInterface {
public:
	GameSpyInterface(PNetCenter* pPNetCenter);
	~GameSpyInterface();

	enum e_connectResult{
		CR_Ok,
		CR_ConnectErr,
		CR_NickErr
	};
	e_connectResult Connect(const char* playerName);

	PNetCenter* m_pPNetCenter;
//	void FillPlayerList(RoomType roomType);
//	SBServer GetCurrentServer();

	PEER m_peer;
	int m_count;
//	CString m_selectedNick;
//	RoomType m_selectedRoom;

	bool quant();
	std::vector<sGameHostInfo*> gameHostList;
	void clearGameHostList(void){
		std::vector<sGameHostInfo*>::iterator p;
		for(p=gameHostList.begin(); p!=gameHostList.end(); p++){
			delete *p;
		}
		gameHostList.erase(gameHostList.begin(), gameHostList.end());
	}

	std::list<sGamePlayerInfo*> gamePlayerList;
	void clearGamePlayerList(void){
		std::list<sGamePlayerInfo*>::iterator p;
		for(p=gamePlayerList.begin(); p!=gamePlayerList.end(); p++){
			delete *p;
		}
		gamePlayerList.erase(gamePlayerList.begin(), gamePlayerList.end());
	}

	static bool IsCreated() { return instance_; }
	static GameSpyInterface* instance() { xassert(instance_); return instance_; }
	static GameSpyInterface* instance_;

	unsigned int uniqueID;
	unsigned int getUniqueID(void) {
		return uniqueID++;
	}

	bool CreateStagingRoom(const char* gameStagingRoomName, const char* password="");
	bool JoinStagingRoom(GUID ID);	
	unsigned int getHostIP(GUID ID);

	bool serverListingComplete;

	enum e_JoinStagingRoomResult {
		JSRR_Processing,
		JSRR_Ok,
		JSRR_PasswordError,
		JSRR_Error
	};
	e_JoinStagingRoomResult result_joinStagingRoom;

	e_JoinStagingRoomResult JoinStagingRoom(unsigned long ip, const char* password="");

	void StartGame(void);

};