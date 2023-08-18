#ifndef __P2P_INTERFACE_H__
#define __P2P_INTERFACE_H__

#include <Localization.h>
#include "NetIncludes.h"
#include "NetComEventBuffer.h"
#include "CommonEvents.h"
#include "ServerList.h"

/**
 
 Notes and info about networking:
 
 Single player games don't use PNetCenter, this is only (re)created when multiplayer menu is opened.
 
 Game follows Server/Client architecture, where the host/server that creates the lobby is the authority
 in the multiplayer game session.

 Both Host and Client use PNetCenter but some parts are specific to the role of game in the network game. 

 ---

 Events act as messages between host and client, some messages are used by both (4G) while others are for host (4H)
 or to client (4C) only. To see a list of event IDs check terEventID enum in CommonEvents.h
 
 Events destined for oneself are directly fed into in-buffer without passing to network stack, there is no a internal
 client/socket connected to server in host machine. 

 ---
 
 The contact is established by clients when socket is opened to the listening port at server, then the client must send
 a NetConnectionInfo to provide the details about the client such as the version, system arch, player name, password etc
 
 The server must always respond with NetConnectionInfoResponse even when the connection can't be kept because is already
 maxed or other reasons such as unstable connection or incompatible arch. This lets client know what the reason of
 refusal was and to inform the player. In case of success the server sends NetConnectionInfoResponse with OK and the
 client is added into server list of players.
 
 ---
 
 Player actions are sent to server in events form which are then processed and/or relayed to the rest of clients
 when advancing a quant, a quant is the logic step/update unit in game. The clients must acknowledge the quant once
 finished and provide a signature, this signature is checked by server to know if desync occurred.
 
 Things that cause desync:
 - Different computation result giving different values in a operation
 - Doing or not doing something compared to the rest of clients
 
 Examples of things that can cause problems:
 - Using math functions like sin/cos/atan... from std library instead of a common one, as each OS or libc implementation
 may provide different results.
 - Sort operations that result in stalemate ordering (same values between elements compare) since some sort implementation
 may decide to place them differently from others, this has occurred on auto targeting when sorted by same distance as
 some units where selected differently due to the array order being different between clients.
 - Code similar to Vect3(terLogicRNDfrnd(), terLogicRNDfrnd(), terLogicRNDfrnd()) as order which terLogicRNDfrnd are called
 per arg might be different, first call might go to z instead of x like it happens in MacOS M1, assigning x and y individually
 before calling Vect3 constructor solved it.
 - Anything that may depend on render/visual side of game, locale, or any value that is not synchronized and same between clients.

 Once a small desync occurs the effect cascades as next values are affected by previous ones until the effects are magnified
 and entire game becomes different, this is why is important to ensure all results are the same and all operations are done
 in same way and order, as long as the result of a function doesn't differ ever and function doesn't produce any different
 side effect  it doesn't matter if the way of computing is different. This allows having game working between different
 CPU architectures and OS'es.
 
 All floating operations must conform IEEE-754 and code must be compiled with proper flags to ensure this. 
 (for example SSE in x86)

 ---

 Current network stack is the following:
 
+---------------------------------------------------+
|                                                   |
|                     Host Game                     |
|                                                   |
+---------------------------------------------------+
|                                                   |
|                     PNetCenter                    |
|                                                   |
+---------------------------------------------------+
|                                                   |
|                NetConnectionHandler               |
|                                                   |
+---------------+-+---------------+-+---------------+
|               | |               | |               |
| NetConnection | | NetConnection | |      ...      |
|               | |               | |               |
+---------------+ +---------------+ +---------------+
|               | |               | |               |
|    SDL_net    | |    SDL_net    | |      ...      |
|               | |               | |               |
+---------------+ +---------------+ +---------------+
|               | |               | |               |
|  OS/Hardware  | |  OS/Hardware  | |      ...      |
|               | |               | |               |
+-------^-------+ +-------^-------+ +-------^-------+
        |                 |                 |
        |                 |                 |
        |                 |                 |  <-------- TCP/IPv4
        |                 |                 |
        |                 |                 |
+-------v-------+ +-------v-------+ +-------v-------+
|               | |               | |               |
|  OS/Hardware  | |  OS/Hardware  | |      ...      |
|               | |               | |               |
+---------------+ +---------------+ +---------------+
|               | |               |         .
|    SDL_net    | |    SDL_net    |         .
|               | |               |         .
+---------------+ +---------------+
|               | |               |
| NetConnection | | NetConnection |
|               | |               |
+---------------+ +---------------+
|               | |               |
| NetCo.Handler | | NetCo.Handler |
|               | |               |
+---------------+ +---------------+
|               | |               |
|   PNetCenter  | |   PNetCenter  |
|               | |               |
+---------------+ +---------------+
|               | |               |
|  Client Game  | |  Client Game  |
|               | |               |
+---------------+ +---------------+

 */

extern const int PNC_DESYNC_RESTORE_ATTEMPTS;
extern const int PNC_DESYNC_RESTORE_MODE_FULL;

#ifdef PERIMETER_DEBUG
#define LogMsg(...) fprintf(stdout, __VA_ARGS__)
#else
#define LogMsg(...)
#endif

class CAutoLock
{
    SDL_mutex* m_pCs;

public:
    CAutoLock(SDL_mutex* pCs) : m_pCs(pCs) {
        SDL_LockMutex(m_pCs);
    }
    ~CAutoLock() {
        SDL_UnlockMutex(m_pCs);
    }
};

// {DF006380-BF70-4397-9A18-51133CEEE3B6}

int InternalServerThread(void* lpParameter);

enum e_PNCDesyncState {
    PNC_DESYNC_NONE,
    PNC_DESYNC_DETECTED,
    PNC_DESYNC_NOTIFIED,
    PNC_DESYNC_ACKNOLEDGED,
    PNC_DESYNC_SENT_RESTORE,
    PNC_DESYNC_RESTORE_FINISHED,
    PNC_DESYNC_RESTORE_FAILED
};

struct PClientData
{
	char playerName[PLAYER_MAX_NAME_LEN];
	bool m_flag_Ready;
	unsigned int clientGameCRC;
	NETID  netidPlayer;

	//OutputEventBuffer out_buffer;
	//InputEventBuffer  in_buffer;
	bool requestPause;
	bool clientPause;
	unsigned int timeRequestPause;

	std::list<netCommand4H_BackGameInformation*> backGameInfList;
	unsigned int lagQuant;
	unsigned int lastExecuteQuant;

	std::vector<netCommand4H_BackGameInformation2> backGameInf2List;
	unsigned int curLastQuant;

    unsigned int lastTimeBackPacket;
    size_t latency = 0;
    size_t last_time_latency_response = 0;

	unsigned int confirmQuant;
    
    //Desync recovery vars
    int desync_amount;
    int desync_last_time;
    e_PNCDesyncState desync_state;
    std::unique_ptr<MissionDescription> desync_missionDescription;
    XBuffer desync_netlog;

	PClientData(const char* name, NETID netid);
	~PClientData();

};

enum e_PPStatus{
	PLAYER,
	AI
};
struct PPlayerData
{
	e_PPStatus	status;
	PClientData* player;

	PPlayerData(e_PPStatus s){
		status = s; player = 0;
	}
};

enum e_PNCInterfaceCommands {
	PNC_INTERFACE_COMMAND_NONE,
	PNC_INTERFACE_COMMAND_CONNECTION_FAILED,
	PNC_INTERFACE_COMMAND_CONNECTION_DROPPED,
	PNC_INTERFACE_COMMAND_INFO_PLAYER_EXIT,
	PNC_INTERFACE_COMMAND_INFO_PLAYER_DISCONNECTED,
    PNC_INTERFACE_COMMAND_INFO_MESSAGE,

	PNC_INTERFACE_COMMAND_HOST_TERMINATED_GAME,
	PNC_INTERFACE_COMMAND_CRITICAL_ERROR_GAME_TERMINATED,
    PNC_INTERFACE_COMMAND_DESYNC

};
struct sPNCInterfaceCommand {
	e_PNCInterfaceCommands icID;
    std::unique_ptr<LocalizedText> text = nullptr;

    sPNCInterfaceCommand() {
        icID = PNC_INTERFACE_COMMAND_NONE;
    }
	explicit sPNCInterfaceCommand(e_PNCInterfaceCommands _icID, std::unique_ptr<LocalizedText> text_ = nullptr){
		icID=_icID;
		if (text_) text=std::move(text_);
	}
};

enum e_PNCInternalCommand{
	PNC_COMMAND__START_HOST_AND_CREATE_GAME_AND_STOP_FIND_HOST,
	PNC_COMMAND__STOP_HOST_AND_ABORT_GAME_AND_START_FIND_HOST,

	//PNCC_START_FIND_HOST,
	PNC_COMMAND__CONNECT_2_HOST_AND_STOP_FIND_HOST,
	PNC_COMMAND__DISCONNECT_AND_ABORT_GAME_AND_END_START_FIND_HOST,
	PNC_COMMAND__DISCONNECT_AND_ABORT_GAME_AND_END,

	//Client back commands
	PNC_COMMAND__CLIENT_STARTING_LOAD_GAME,
	PNC_COMMAND__CLIENT_STARTING_GAME,

	//Special command
	PNC_COMMAND__STOP_GAME_AND_ASSIGN_HOST_2_MY,
	PNC_COMMAND__STOP_GAME_AND_WAIT_ASSIGN_OTHER_HOST,

	PNC_COMMAND__END,
	PNC_COMMAND__ABORT_PROGRAM,
	
	PNC_COMMAND__END_GAME,
	PNC_COMMAND__START_FIND_HOST,
    PNC_COMMAND__DESYNC

};

#define PNC_State_Host (0x80)
#define PNC_State_GameRun (0x40)
enum e_PNCState{
	PNC_STATE__NONE=0,

	PNC_STATE__CLIENT_FIND_HOST=1,

	PNC_STATE__CONNECTION=2,
	PNC_STATE__CLIENT_TUNING_GAME=4,
	PNC_STATE__CLIENT_LOADING_GAME=PNC_State_GameRun|5,
	PNC_STATE__CLIENT_GAME=PNC_State_GameRun|6,

	PNC_STATE__CLIENT_RESTORE_GAME_AFTE_CHANGE_HOST_PHASE_0=PNC_State_GameRun|7,
	PNC_STATE__CLIENT_RESTORE_GAME_AFTE_CHANGE_HOST_PHASE_AB=PNC_State_GameRun|8,

	//special state

	PNC_STATE__END=9,

	PNC_STATE__NET_CENTER_CRITICAL_ERROR=10,

	PNC_STATE__HOST_TUNING_GAME=						PNC_State_Host	|11,
	PNC_STATE__HOST_LOADING_GAME=	PNC_State_GameRun	|PNC_State_Host	|12,
	PNC_STATE__HOST_GAME=			PNC_State_GameRun	|PNC_State_Host|13,

	PNC_STATE__NEWHOST_PHASE_0=PNC_State_GameRun|PNC_State_Host|14,
	PNC_STATE__NEWHOST_PHASE_A=PNC_State_GameRun|PNC_State_Host|15,
	PNC_STATE__NEWHOST_PHASE_B=PNC_State_GameRun|PNC_State_Host|16,

    //Desync states
    PNC_STATE__CLIENT_DESYNC=PNC_State_GameRun|17,
    PNC_STATE__HOST_DESYNC=PNC_State_GameRun|PNC_State_Host|18,
    PNC_STATE__HOST_SENDING_GAME=PNC_State_Host|20,

	// Состояние завершения
	PNC_STATE__ENDING_GAME=19
};

enum e_PNCStateClient{
	PNC_STATE_CLIENT__NONE=0,

	PNC_STATE_CLIENT__FIND_HOST=1,


	PNC_STATE_CLIENT__TUNING_GAME=2,
	PNC_STATE_CLIENT__LOADING_GAME=3,
	PNC_STATE_CLIENT__GAME=4,

	PNC_STATE_CLIENT__RESTORE_GAME_AFTE_CHANGE_HOST,
	//special state

	PNC_STATE_CLIENT__END,

	PNC_STATE__CLIENT_NET_CENTER_CRITICAL_ERROR
};

enum e_PNCStateHost{
	PNC_STATE_HOST_NONE=0,


	PNC_STATE_HOST__END,

	PNC_STATE_HOST__NET_CENTER_CRITICAL_ERROR,

	PNC_STATE_HOST__TUNING_GAME=PNC_State_Host|2,
	PNC_STATE_HOST__LOADING_GAME=PNC_State_Host|3,
	PNC_STATE_HOST__GAME=PNC_State_Host|4,

	PNC_STATE_NEWHOST__WAIT_ALL_CLIENT_GAME_STATUS=PNC_State_Host|5,
	PNC_STATE_NEWHOST__WAIT_GAME_DATA=PNC_State_Host|6
};

struct InputPacket;

class PNetCenter {
private:
    ServerList serverList;
    NetConnectionHandler connectionHandler;

public:
	std::list<e_PNCInternalCommand> internalCommandList;
	HANDLE hSecondThreadInitComplete;
	HANDLE hCommandExecuted;
	std::list<sPNCInterfaceCommand*> interfaceCommandList;

	//typedef hash_map<NETID, PClientData*> ClientMapType;
	typedef std::vector<PClientData*> ClientMapType;

	e_PNCState m_state;
	const char* getStrState() const;

	//int m_quantPeriod;
	size_t m_nextQuantTime;

    //Use server listing to get servers or broadcast server to listing
    bool publicServerHost;

	//bool flag_missionDescriptionUpdate;
	MissionDescription* hostMissionDescription = nullptr;

	MissionDescription* clientMissionDescription = nullptr; //Only 1

	//string m_missionName;
	
	ClientMapType  m_clients;
	//NETID          m_netidGroupGame; //Этот NETID используется для отправки сообщений всей группе

	//int            m_nServerPause;
	int            m_nQuantCommandCounter;

	//int            m_nWorldID;
    std::string    m_GameName;
    std::string    m_PlayerName;
	bool           m_bStarted;

	std::list<netCommandGeneral*>   m_CommandList;
    std::list<netCommandGame*>   m_QueuedGameCommands;

	void ClearCommandList();


	///HANDLE             m_hPlayerListReady;
	//bool m_flag_PlayerListReady;
	//list<PPlayerData>  m_PlayerStartList;


	PNetCenter();
	~PNetCenter();

	void DisconnectAndStartFindHost();
	void StopServerAndStartFindHost();


	void UpdateBattleData();
    void SendBattleData();
	void UpdateCurrentMissionDescription4C();
	void CheckClients();
	void DumpClients();
	void ResetAllClients();

//	void QuantTimeProc();
	//Host !!!
	void SendEvent(netCommandGeneral& event, NETID destination);
	void PutGameCommand2Queue_andAutoDelete(NETID netid, netCommandGame* pCommand);


///	void SetGameSpeedScale(float scale, NETID netidFrom);

	int AddClient(PlayerData& pd);
	void ClearClients();
    void ExitClient(NETID netid);
	void DeleteClient(NETID netid, bool normalExit);

	//void StartGame();
	void LLogicQuant();

    SDL_mutex* m_GeneralLock;

	//Game(first) Thread
	InOutNetComBuffer  in_ClientBuf;
	InOutNetComBuffer  out_ClientBuf;
	bool flag_SkipProcessingGameCommand;//Нужно при миграции Host-а

	InOutNetComBuffer  in_HostBuf;
	InOutNetComBuffer  out_HostBuf;
	void clearInOutClientHostBuffers();
    void ClearQueuedGameCommands();

	bool ExecuteInternalCommand(e_PNCInternalCommand ic, bool waitExecution);
	bool ExecuteInterfaceCommand(e_PNCInterfaceCommands ic, std::unique_ptr<LocalizedText> text = nullptr);

	void CreateGame(const NetAddress& connection, const std::string& gameName, MissionDescription* mission, const std::string& playerName, const std::string& password="");

	void JoinGame(const NetAddress& connection, const std::string& playerName, const std::string& password="");

	std::vector<GameHostInfo>& getGameHostList();

	void SendEvent(const netCommandGeneral* event);
	void SendEventSync(const netCommandGeneral* event);



	void ServerTimeControl(float scale);
	void changePlayerBelligerent(int idxPlayerData, terBelligerent newBelligerent);
	void changePlayerColor(int idxPlayerData, int newColorIndex, bool direction);
	void changeRealPlayerType(int idxPlayerData, RealPlayerType newRealPlayerType);
	void changePlayerDifficulty(int idxPlayerData, Difficulty difficulty);
	void changePlayerClan(int idxPlayerData, int clan);
	void changePlayerHandicap(int idxPlayerData, int handicap);
    void changePlayerSeat(int idxPlayerData);
	void changeMap(const char* missionName);


	void StartLoadTheGame(bool state);
	void GameIsReady();


	void P2PIQuant();
	void HandlerInputNetCommand();

	//NETID m_playerNETID; //Информация на клиенте присланный его NETID


	HANDLE hSecondThread;
	//Second Thread
	bool SecondThread();
	void ClearClientData(){
		ClientMapType::iterator p;
		for(p=m_clients.begin(); p!=m_clients.end(); p++){
			delete *p;
		}
		m_clients.clear();
	}
	bool AddClientToMigratedHost(const NETID _netid, unsigned int _curLastQuant, unsigned int _confirmQuant);


	MissionDescription lobbyMissionDescription;
	const MissionDescription& getLobbyMissionDescription() {
		return lobbyMissionDescription;
	};

	unsigned int m_numberGameQuant; //Кванты на хосте Кванты считаются с 1-цы!
	void HostReceiveQuant();
	void ClientPredReceiveQuant();

    uint64_t last_latency_status = 0;

	unsigned int m_quantInterval;

	unsigned int m_beginWaitTime;

	unsigned int lastTimeServerPacket;

	unsigned int TIMEOUT_CLIENT_OR_SERVER_RECEIVE_INFORMATION;
	unsigned int TIMEOUT_DISCONNECT;
	unsigned int MAX_TIME_PAUSE_GAME;

	bool isHost(void){
		if(m_state&PNC_State_Host) return 1;
		else return 0;
	}
	bool isGameRun(void){
		if(m_state&PNC_State_GameRun) return 1;
		else return 0;
	}

    bool isSaveGame() {
        return lobbyMissionDescription.gameType_ == GT_MULTI_PLAYER_LOAD;
    }

    NETID	m_hostNETID;
    NETID	m_localNETID;
    bool flag_connected;

	void FinishGame(void);
	void StartFindHost(void);

	bool Init();
    bool ServerStart();
	void SetConnectionTimeout(int ms);
	void RemovePlayer(NETID netid);
    
	void Close(bool flag_immediate=1);
	bool Connect();

	bool isConnected() const;
    size_t Send(const char* buffer, size_t size, NETID destination);

	unsigned int flag_LockIputPacket;
	void LockInputPacket(void);
	void UnLockInputPacket(void);
    void ClearInputPacketList();

	std::list<InputPacket*> m_InputPacketList;
	bool PutInputPacket2NetBuffer(InOutNetComBuffer& netBuf, NETID& returnNETID);

    NetAddress hostConnection;

	//Host Date
	size_t hostGeneralCommandCounter;
	unsigned int quantConfirmation;

	NETID netidClientWhichWeWait; //netid игрока которому хост при миграции посылает команду прислать игровые комманды; нужен чтобы в случае выхода переслать комманду другому

	//Host info //TODO originally for gamespy, we should use this for public listed hosts in future
	const char* getMissionName();
	const char* getGameName();
	const char* getGameVer();
	int getNumPlayers();
	int getMaxPlayers();
	int getHostPort();
    bool hasPassword() {
        return !gamePassword.empty();
    }
	enum e_perimeterGameState{
		PGS_OPEN_WAITING,
		PGS_CLOSE_WAITING,
		PGS_CLOSED_PLAYING
	};
	e_perimeterGameState getGameState(void){
		switch(m_state){
		case PNC_STATE__HOST_TUNING_GAME:
			return PGS_OPEN_WAITING;
		case PNC_STATE__HOST_GAME:
			return PGS_CLOSED_PLAYING;
		default:
			return PGS_CLOSED_PLAYING;
		}
	}
	std::string gamePassword;

	//Network settings
	bool flag_NetworkSimulation;
	bool flag_HostMigrate;
	bool flag_NoUseDPNSVR;
	int m_DPSigningLevel;//0-none 1-fast signed 2-full signed

	//Pause client
	bool setPause(bool pause);
	bool isPause();

	bool hostPause;
	bool clientPause;
	bool clientInPacketPause;

	//Chat
	void chatMessage(bool clanOnly, const std::string& text, const std::string& locale);

    //NetConnection stuff
    void handleIncomingClientConnection(NetConnection* connection);
};



#endif //__P2P_INTERFACE_H__