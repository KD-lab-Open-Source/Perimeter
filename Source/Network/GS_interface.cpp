#include "StdAfx.h"
#include "P2P_interface.h"
#include "GS_interface.h"

GameSpyInterface* GameSpyInterface::instance_=0;



static void ReadyChangedCallback(PEER peer, const char * nick, PEERBool ready, void * param)
{
/*	char buffer[256];
	if(ready) sprintf(buffer, "%s is ready\n", nick);
	else sprintf(buffer, "%s is not ready\n", nick);
	OutputDebugString(buffer);
*/
}

static void RoomMessageCallback(PEER peer, RoomType roomType, const char * nick, const char * message, MessageType messageType, void * param)
{
/*	int c;

	c = *RoomToString(roomType);

	char buffer[2048];
	if(messageType == NormalMessage) {
		sprintf(buffer, "%c]%s: %s", c, nick, message);
	}
	else if(messageType == ActionMessage) {
		sprintf(buffer, "%c]%s %s", c, nick, message);
	}
	else {
		sprintf(buffer, "%c]*%s*: %s", c, nick, message);
	}
	int index = dlg->m_chatList.InsertString(-1, buffer);

#if VERBOSE
	OutputDebugString(buffer);
	OutputDebugString("\n");
#endif

	dlg->m_chatList.SetCurSel(index);
	dlg->m_chatList.SetCurSel(-1);
*/
}

static void RoomUTMCallback(PEER peer, RoomType roomType, const char * nick, const char * command, const char * parameters, PEERBool authenticated, void * param)
{
/*
#if VERBOSE
	char buffer[256];
	sprintf(buffer, " UTM %s| %s | %s | %s | %s\n",
		authenticated?"(authenticated) ":"",
		RoomToString(roomType),
		nick,
		command,
		parameters);
	OutputDebugString(buffer);
#endif
*/
}

static void RoomNameChangedCallback(PEER peer, RoomType roomType, void * param)
{
/*#if VERBOSE
	char buffer[256];
	sprintf(buffer, " NAME | %s | %s\n",
		RoomToString(roomType),
		peerGetRoomName(peer, roomType));

	OutputDebugString(buffer);
#endif
*/
}

const char * ModeToString(CHATChannelMode * mode)
{
	static char buffer[256];
	buffer[0] = '\0';

	if(mode->InviteOnly) strcat(buffer, "InviteOnly+");
	if(mode->Private) strcat(buffer, "Private+");
	if(mode->Secret) strcat(buffer, "Secret+");
	if(mode->Moderated) strcat(buffer, "Moderated+");
	if(mode->NoExternalMessages) strcat(buffer, "NoExternalMessages+");
	if(mode->OnlyOpsChangeTopic) strcat(buffer, "OnlyOpsChangeTopic+");

	if(buffer[0]) buffer[strlen(buffer) - 1] = '\0';

	return buffer;
}

static void RoomModeChangedCallback(PEER peer, RoomType roomType, CHATChannelMode * mode, void * param)
{
/*#if VERBOSE
	char buffer[256];
	sprintf(buffer, " MODE | %s | %s\n",
		RoomToString(roomType),
		ModeToString(mode));

	OutputDebugString(buffer);
#endif
*/
}

static void PlayerMessageCallback(PEER peer, const char * nick, const char * message, MessageType messageType, void * param)
{
/*	char buffer[2048];
	if(messageType == NormalMessage) {
		sprintf(buffer, "%s: %s", nick, message);
	}
	else if(messageType == ActionMessage) {
		sprintf(buffer, "%s %s", nick, message);
	}
	else {
		sprintf(buffer, "*%s*: %s", nick, message);
	}
	int index = dlg->m_chatList.InsertString(-1, buffer);

#if VERBOSE
	OutputDebugString(buffer);
	OutputDebugString("\n");
#endif

	dlg->m_chatList.SetCurSel(index);
	dlg->m_chatList.SetCurSel(-1);
*/
}

static void PlayerUTMCallback(PEER peer, const char * nick, const char * command, const char * parameters, PEERBool authenticated, void * param)
{
/*#if VERBOSE
	char buffer[256];
	sprintf(buffer, " UTM %s| %s | %s | %s\n",
		authenticated?"(authenticated) ":"",
		nick,
		command,
		parameters);
	OutputDebugString(buffer);
#endif
*/
}


static void GameStartedCallback(PEER peer, SBServer server, const char * message, void * param)
{
/*	char buffer[256];
	sprintf(buffer, "The game is starting: %s:%d %s",
		SBServerGetPublicAddress(server), SBServerGetIntValue(server, "hostport", 0), message);
	dlg->MessageBox(buffer);
*/
}

static void EnumPlayersCallback(PEER peer, PEERBool success, RoomType roomType, int index, const char * nick, int flags, void * param)
{

	if(!success) {
		xassert(0&&"EnumPlayers failed");
		return;
	}

	if(index == -1)
		return;

	if(!GameSpyInterface::IsCreated()) return;
	GameSpyInterface* pGSI=GameSpyInterface::instance();
	//dlg->m_count++;
	if(flags & PEER_FLAG_HOST) {
		//char buffer[128];
		//sprintf(buffer, "HOST: %s", nick);
		//m_players(roomType).AddString(buffer);
		//pGSI->gamePlayerList.push_back();
	}
	else {
		//m_players(roomType).AddString(nick);
	}
}

static void PlayerJoinedCallback(PEER peer, RoomType roomType, const char * nick, void * param)
{
	if(!GameSpyInterface::IsCreated()) return;
//	dlg->FillPlayerList(roomType);
	if(roomType==StagingRoom){
		GameSpyInterface* pGSI=GameSpyInterface::instance();
		pGSI->clearGamePlayerList();
		if(peerInRoom(peer, roomType))
			peerEnumPlayers(peer, roomType, EnumPlayersCallback, NULL);

		int ip=1000;
		pGSI->m_pPNetCenter->reEnumPlayers(ip);
	}
}


static void PlayerLeftCallback(PEER peer, RoomType roomType, const char * nick, const char * reason, void * param)
{
//	dlg->FillPlayerList(roomType);
}

static void KickedCallback(PEER peer, RoomType roomType, const char * nick, const char * reason, void * param)
{
/*	CString str;
	str.Format("Kicked from %s by %s: \"%s\"", RoomToString(roomType), nick, reason);
	dlg->MessageBox(str);
	ClearPlayersList(m_players(roomType));
*/
}

static void NewPlayerListCallback(PEER peer, RoomType roomType, void * param)
{
//	dlg->FillPlayerList(roomType);
}

static void PlayerChangedNickCallback(PEER peer, RoomType roomType, const char * oldNick, const char * newNick, void * param)
{
/*
#if VERBOSE
	char buffer[256];
	sprintf(buffer, " NICK | %s | %s\n", oldNick, newNick);
	OutputDebugString(buffer);
#endif

	// Update the player list for this room.
	////////////////////////////////////////
	dlg->FillPlayerList(roomType);
*/
}

static void PlayerInfoCallback(PEER peer, RoomType roomType, const char * nick, unsigned int IP, int profileID, void * param)
{
/*#if VERBOSE
	char buffer[256];
	IN_ADDR addr;
	addr.s_addr = IP;
	sprintf(buffer, " INFO | %s | %s | %s | %d\n", RoomToString(roomType), nick?nick:"(END)", inet_ntoa(addr), profileID);
	OutputDebugString(buffer);
#endif
*/
}

/*
static void FlagsToString(int flags, CString & str)
{
	if(!flags) {
		str = "(none)";
		return;
	}

	str = "";

	if(flags & PEER_FLAG_STAGING) str += "staging+";
	if(flags & PEER_FLAG_READY) str += "ready+";
	if(flags & PEER_FLAG_PLAYING) str += "playing+";
	if(flags & PEER_FLAG_AWAY) str += "away+";
	if(flags & PEER_FLAG_HOST) str += "host+";
	if(flags & PEER_FLAG_OP) str += "op+";
	if(flags & PEER_FLAG_VOICE) str += "voice+";

	if(!str.IsEmpty()) str.Delete(str.GetLength() - 1);
}*/

static void PlayerFlagsChangedCallback(PEER peer, RoomType roomType, const char * nick, int oldFlags, int newFlags, void * param)
{
/*#if VERBOSE
	char buffer[256];
	CString strOldFlags;
	CString strNewFlags;

	FlagsToString(oldFlags, strOldFlags);
	FlagsToString(newFlags, strNewFlags);

	sprintf(buffer, " FLAGS | %s | %s | %s=>%s\n", RoomToString(roomType), nick, strOldFlags, strNewFlags);
	OutputDebugString(buffer);
#endif

	// If the host flag changed, refresh the room.
	//////////////////////////////////////////////
	if((oldFlags ^ newFlags) & PEER_FLAG_HOST)
		dlg->FillPlayerList(roomType);
*/
}

static int FindPingPlayersNick(const char * nick)
{
/*	CListBox & list = dlg->m_pingPlayers;
	int count;
	int i;
	CString text;
	int start;

	count = list.GetCount();
	for(i = 0 ; i < count ; i++)
	{
		list.GetText(i, text);

		start = text.Find('-');
		ASSERT(start != -1);

		if(text.Mid(start + 1).CompareNoCase(nick) == 0)
			return i;
	}
*/
	return -1;
}

static void PingCallback(PEER peer, const char * nick, int ping, void * param)
{
/*#if VERBOSE
	char buffer[256];
	sprintf(buffer, " PING | %s | %d\n", nick, ping);
	OutputDebugString(buffer);
#endif
*/
}


static void CrossPingCallback(PEER peer, const char * nick1, const char * nick2, int crossPing, void * param)
{
/*#if VERBOSE
	char buffer[256];
	sprintf(buffer, "XPING | %s<->%s | %d\n", nick1, nick2, crossPing);
	OutputDebugString(buffer);
#endif*/
}

static void GlobalKeyChangedCallback(PEER peer, const char * nick, const char * key, const char * value, void * param)
{
/*#if VERBOSE
	char buffer[256];
	sprintf(buffer, "KEY | %s | %s = %s\n", nick, key, value);
	OutputDebugString(buffer);
#endif*/
}

static void RoomKeyChangedCallback
(
	PEER peer,
	RoomType roomType,
	const char * nick,
	const char * key,
	const char * value,
	void * param
)
{
#if VERBOSE
	char buffer[256];
	sprintf(buffer, "KEY | %s | %s | %s = %s\n", nick, RoomToString(roomType), key, value);
	OutputDebugString(buffer);
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////
static const char * KeyTypeToString(qr2_key_type type)
{
	switch(type) {
	case key_server:
		return "server";
	case key_player:
		return "player";
	case key_team:
		return "team";
	}

	xassert(0&& "GameSpy: invalid type - qr2_key_type");
	return "Unkown key type";
}

static const char * ErrorTypeToString(qr2_error_t error)
{
	switch(error) {
	case e_qrnoerror:
		return "noerror";
	case e_qrwsockerror:
		return "wsockerror";
	case e_qrbinderror:
		return "rbinderror";
	case e_qrdnserror:
		return "dnserror";
	case e_qrconnerror:
		return "connerror";
	case e_qrnochallengeerror:
		return "nochallengeerror";
	}

	xassert(0&&"GameSpy: invalid type - qr2_key_type");
	return "Unknown error type";
}

static void QRServerKeyCallback(PEER peer, int key, qr2_buffer_t buffer, void * param)
{
//#if VERBOSE
//	char verbose[256];
//	sprintf(verbose, "QR_SERVER_KEY | %d\n", key);
//	OutputDebugString(verbose);
//#endif
	if(!GameSpyInterface::IsCreated()) return;
	GameSpyInterface* pGSI=GameSpyInterface::instance();

	switch(key) {
	case GAMEVER_KEY:
		qr2_buffer_add(buffer, pGSI->m_pPNetCenter->getGameVer());
		break;
	case HOSTNAME_KEY:
		qr2_buffer_add(buffer, pGSI->m_pPNetCenter->getGameName());
		break;
	case NUMPLAYERS_KEY:
		qr2_buffer_add_int(buffer, pGSI->m_pPNetCenter->getNumPlayers());
		break;
	case MAXPLAYERS_KEY:
		qr2_buffer_add_int(buffer, pGSI->m_pPNetCenter->getMaxPlayers());
		break;
	case GAMETYPE_KEY:
		qr2_buffer_add(buffer, "Deathmatch");
		break;
	case HOSTPORT_KEY:
		qr2_buffer_add_int(buffer, pGSI->m_pPNetCenter->getHostPort());
		break;
	case MAPNAME_KEY:
		qr2_buffer_add(buffer, pGSI->m_pPNetCenter->getMissionName());
		break;
	case GAMEMODE_KEY:
		switch (pGSI->m_pPNetCenter->getGameState()){
		case PNetCenter::PGS_OPEN_WAITING:
			qr2_buffer_add(buffer, "openwaiting");
			break;
		case PNetCenter::PGS_CLOSE_WAITING:
			qr2_buffer_add(buffer, "closedwaiting");
			break;
		default:// PNetCenter::PGS_CLOSED_PLAYING:
			qr2_buffer_add(buffer, "closedplaying");
			break;
		}
		//qr2_buffer_add(buffer, "openplaying");
		break;
	case PASSWORD_KEY:
		qr2_buffer_add_int(buffer, pGSI->m_pPNetCenter->isPassword());
		break;
/*	case TIMELIMIT_KEY:
		qr2_buffer_add_int(buffer, 100);
		break;
	case FRAGLIMIT_KEY:
		qr2_buffer_add_int(buffer, 0);
		break;
	case TEAMPLAY_KEY:
		qr2_buffer_add_int(buffer, 0);
		break;*/
	default:
		qr2_buffer_add(buffer, "");
		break;
	}
}

static void QRPlayerKeyCallback(PEER peer, int key, int index, qr2_buffer_t buffer, void * param)
{
/*#if VERBOSE
	char verbose[256];
	sprintf(verbose, "QR_PLAYER_KEY | %d | %d\n", key, index);
	OutputDebugString(verbose);
#endif

	switch(key) {
	case PLAYER__KEY:
		qr2_buffer_add(buffer, "Johnny McJohnson");
		break;
	case PING__KEY:
		qr2_buffer_add_int(buffer, 17);
		break;
	default:
		qr2_buffer_add(buffer, "");
		break;
	}
*/
}

///////////////////////////////////////////////////////////////////////////////////////////
static void QRTeamKeyCallback(PEER peer, int key, int index, qr2_buffer_t buffer, void * param)
{
/*#if VERBOSE
	char verbose[256];
	sprintf(verbose, "QR_TEAM_KEY | %d | %d\n", key, index);
	OutputDebugString(verbose);
#endif

	// we don't report teams, so this shouldn't get called
	qr2_buffer_add(buffer, "");
*/
}

static void QRKeyListCallback(PEER peer, qr2_key_type type, qr2_keybuffer_t keyBuffer, void * param)
{
//#if VERBOSE
//	char verbose[256];
//	sprintf(verbose, "QR_KEY_LIST | %s\n", KeyTypeToString(type));
//	OutputDebugString(verbose);
//#endif

	// register the keys we use
	switch(type) {
	case key_server:
		if(!peerIsAutoMatching(peer)) {
			qr2_keybuffer_add(keyBuffer, HOSTPORT_KEY);
			qr2_keybuffer_add(keyBuffer, MAPNAME_KEY);
			qr2_keybuffer_add(keyBuffer, GAMETYPE_KEY);
			qr2_keybuffer_add(keyBuffer, GAMEVER_KEY);
			qr2_keybuffer_add(keyBuffer, NUMPLAYERS_KEY);
			qr2_keybuffer_add(keyBuffer, MAXPLAYERS_KEY);
			qr2_keybuffer_add(keyBuffer, GAMEMODE_KEY);
			qr2_keybuffer_add(keyBuffer, PASSWORD_KEY);

			///qr2_keybuffer_add(keyBuffer, TIMELIMIT_KEY);
			///qr2_keybuffer_add(keyBuffer, FRAGLIMIT_KEY);
			///qr2_keybuffer_add(keyBuffer, TEAMPLAY_KEY);
		}
		break;
	case key_player:
		// no custom player keys
		break;
	case key_team:
		// no custom team keys
		break;
	}

}

static int QRCountCallback(PEER peer, qr2_key_type type, void * param)
{
/*#if VERBOSE
	char verbose[256];
	sprintf(verbose, "QR_COUNT | %s\n", KeyTypeToString(type));
	OutputDebugString(verbose);
#endif

	if(type == key_player)
		return 1;
	else if(type == key_team)
		return 0;

	return 0;
*/
	return 0;
}

static void QRAddErrorCallback(PEER peer, qr2_error_t error, char * errorString, void * param)
{
/*#if VERBOSE
	char verbose[256];
	sprintf(verbose, "QR_ADD_ERROR | %s | %s\n", ErrorTypeToString(error), errorString);
	OutputDebugString(verbose);
#endif

	CString str;
	str.Format("Reporting error: %s (%s)", ErrorTypeToString(error), errorString);
	dlg->MessageBox(str);
*/
}

static void QRNatNegotiateCallback(PEER peer, int cookie, void * param)
{
/*#if VERBOSE
	char verbose[256];
	sprintf(verbose, "QR_NAT_NEGOTIATE | 0x%08X\n", cookie);
	OutputDebugString(verbose);
#endif

	CString str;
	str.Format("Received Nat Negotiate Cookie: 0x%08X", cookie);
	dlg->MessageBox(str);
*/
}


//////////////////////////////////////////////////////////////////////////////////////////////
void ListingGamesCallback(PEER peer, PEERBool success, const char * name, SBServer server, PEERBool staging, int msg, int progress, void * param)
{
	//int nIndex;

	if(!GameSpyInterface::IsCreated()) return;
	GameSpyInterface* pGSI=GameSpyInterface::instance();

	if(!success) {
		xassert("ListingGames failed");
		pGSI->serverListingComplete=true;
		return;
	}

	//dlg->m_progress.SetPos(progress);

	switch(msg){
	case PEER_CLEAR:
		{
			pGSI->serverListingComplete=false;
			pGSI->clearGameHostList();
		}
		break;
	case PEER_COMPLETE:
		{
			pGSI->serverListingComplete=true;
		}
		break;
	case PEER_ADD:
		{
			GUID guid;
			guid.Data1=pGSI->getUniqueID();
			guid.Data2=0;
			guid.Data3=0;
			for(int k=0; k<sizeof(guid.Data4); k++) guid.Data4[k]=0;
			bool gameRun;
			if(staging) gameRun=0;
			else gameRun=1;
			pGSI->gameHostList.push_back(new sGameHostInfo(guid, "", "", name, sGameStatusInfo(4,1, gameRun, 10, -1), server));
		}
		break;
	case PEER_REMOVE:
		{
			vector<sGameHostInfo*>::iterator p;
			for(p=pGSI->gameHostList.begin(); p!=pGSI->gameHostList.end(); ){
				if( server==(*p)->pData) {
					delete *p;
					p=pGSI->gameHostList.erase(p);
				}
				else p++;
			}
		}
		break;
	case PEER_UPDATE:
		{
			vector<sGameHostInfo*>::iterator p;
			for(p=pGSI->gameHostList.begin(); p!=pGSI->gameHostList.end(); p++){
				if( server== (*p)->pData ) {
					bool gameRun;
					if(staging) gameRun=0;
					else gameRun=1;
					GUID guid=(*p)->gameHostGUID;
					sGameStatusInfo gsi(4, 1, gameRun, 10, -1);
					(*p)->set(guid, "", "", name, gsi, server);
				}
			}
		}
		break;
	default:
		break;
	}

//	char buffer[32];
//	sprintf(buffer, "Rooms: %d", dlg->m_rooms.GetCount());
//	dlg->m_roomsBox.SetWindowText(buffer);
}


static bool result_Disconnect=0;
static void DisconnectedCallback ( PEER peer, const char * reason, void * param )
{
	result_Disconnect=1;
//	CString message = "Disconnected: ";
//	message += reason;
//	dlg->MessageBox(message);
}

#ifdef _MULTIPLAYER_DEMO_
const char PERIMETER_GAME_TITLE[]="perimeterd";
#else
const char PERIMETER_GAME_TITLE[]="perimeter";
#endif

const char PERIMETER_GAME_PASSWORD[]="FRYbdA";
extern "C"{
	extern PEERBool piOldMangleStagingRooms;
}
GameSpyInterface::GameSpyInterface(PNetCenter* pPNetCenter)
{
	xassert(!instance_);
	instance_ = this;

	m_pPNetCenter=pPNetCenter;

	serverListingComplete=false;

	piOldMangleStagingRooms=PEERTrue;//Для идентификации сервера ТОЛЬКО по IP адресу

	uniqueID=1;//начальное значение

	PEERCallbacks callbacks;
	memset(&callbacks, 0, sizeof(PEERCallbacks));
	callbacks.disconnected = DisconnectedCallback;
	callbacks.readyChanged = ReadyChangedCallback;
	callbacks.roomMessage = RoomMessageCallback;
	callbacks.roomUTM = RoomUTMCallback;
	callbacks.roomNameChanged = RoomNameChangedCallback;
	callbacks.roomModeChanged = RoomModeChangedCallback;
	callbacks.playerMessage = PlayerMessageCallback;
	callbacks.playerUTM = PlayerUTMCallback;
	callbacks.gameStarted = GameStartedCallback;
	callbacks.playerJoined = PlayerJoinedCallback;
	callbacks.playerLeft = PlayerLeftCallback;
	callbacks.kicked = KickedCallback;
	callbacks.newPlayerList = NewPlayerListCallback;
	callbacks.playerChangedNick = PlayerChangedNickCallback;
	callbacks.playerInfo = PlayerInfoCallback;
	callbacks.playerFlagsChanged = PlayerFlagsChangedCallback;
	callbacks.ping = PingCallback;
	callbacks.crossPing = CrossPingCallback;
	callbacks.globalKeyChanged = GlobalKeyChangedCallback;
	callbacks.roomKeyChanged = RoomKeyChangedCallback;
	callbacks.qrServerKey = QRServerKeyCallback;
	callbacks.qrPlayerKey = QRPlayerKeyCallback;
	callbacks.qrTeamKey = QRTeamKeyCallback;
	callbacks.qrKeyList = QRKeyListCallback;
	callbacks.qrCount = QRCountCallback;
	callbacks.qrAddError = QRAddErrorCallback;
	callbacks.qrNatNegotiateCallback = QRNatNegotiateCallback;
	callbacks.param = NULL;

	m_peer = peerInitialize(&callbacks);
	if(!m_peer) {
		xassert(0&&"GameSpy: Init falure");//dlg->MessageBox("Init: failure");
		return;
	}


	PEERBool pingRooms[NumRooms];
	PEERBool crossPingRooms[NumRooms];
	pingRooms[TitleRoom] = PEERTrue;
	pingRooms[GroupRoom] = PEERTrue;
	pingRooms[StagingRoom] = PEERTrue;
	crossPingRooms[TitleRoom] = PEERFalse;
	crossPingRooms[GroupRoom] = PEERFalse;
	crossPingRooms[StagingRoom] = PEERTrue;

	//PEERBool result = peerSetTitle(m_peer, "gmtest", "HA6zkS", "gmtest", "HA6zkS", 0, 30, PEERTrue, pingRooms, crossPingRooms);
	PEERBool result = peerSetTitle(m_peer, PERIMETER_GAME_TITLE, PERIMETER_GAME_PASSWORD, PERIMETER_GAME_TITLE, PERIMETER_GAME_PASSWORD, 0, 30, PEERTrue, pingRooms, crossPingRooms);
	if(!result) {
		xassert(0&&"GameSpy: Failed to set the title");
		return;
	}
}

GameSpyInterface::~GameSpyInterface()
{
	if(m_peer) {
		peerStopGame(m_peer);
		peerDisconnect(m_peer);
		peerShutdown(m_peer);
		m_peer = 0;
	}

	xassert(instance_);
	instance_ = 0;
}

static bool result_connectResult=0;
static void ConnectCallback(PEER peer, PEERBool success, void * param)
{
	//if(success) ::MessageBox(0, "Connect: success", "Diagnostic", MB_OK);
	//else ::MessageBox(0, "Connect: failure","Diagnostic",MB_OK);
	result_connectResult=1;
}

static bool result_NickError=0;
static void NickErrorCallback(PEER peer, int type, const char * nick, void * param)
{
	//xassert(0&&"Nick error");
	//peerRetryWithNick(peer, NULL);
	peerDisconnect(peer);
	//peerRetryWithNick(peer,"");
	result_NickError=1;
}


GameSpyInterface::e_connectResult GameSpyInterface::Connect(const char* playerName)
{
	bool m_blocking=true;
	result_connectResult=0;
	result_NickError=0;
	peerConnect(m_peer, playerName, 0, NickErrorCallback, ConnectCallback, this, (PEERBool)m_blocking);
	if(result_NickError) return CR_NickErr;
	if(!result_connectResult) return CR_ConnectErr;


	//m_roomsBox.SetWindowText("Rooms:");
	//ClearRoomsList(m_rooms);
	//m_progress.SetPos(0);
	const unsigned char keys[] = { NUMPLAYERS_KEY, MAXPLAYERS_KEY, HOSTPORT_KEY, MAPNAME_KEY, GAMEMODE_KEY, PASSWORD_KEY };
	peerStartListingGames(m_peer, keys, sizeof(keys), "", ListingGamesCallback, NULL);//m_filter
	return CR_Ok;
}


static void JoinRoomCallback(PEER peer, PEERBool success, PEERJoinResult result, RoomType roomType, void * param)
{
	if(!GameSpyInterface::IsCreated()) return;
	GameSpyInterface* pGSI=GameSpyInterface::instance();
	if(success && result==PEERJoinSuccess){
		pGSI->result_joinStagingRoom=GameSpyInterface::JSRR_Ok;
	}
	else{
		if(result==PEERBadPassword)
			pGSI->result_joinStagingRoom=GameSpyInterface::JSRR_PasswordError;
		else 
			pGSI->result_joinStagingRoom=GameSpyInterface::JSRR_Error;
	}
}



bool GameSpyInterface::CreateStagingRoom(const char* gameStagingRoomName, const char* password)
{
//	bool m_blocking=true;
//	result_joinStagingRoom=JSRR_Processing;
//	peerCreateStagingRoom(m_peer, gameStagingRoomName/*m_name*/, NETWORK_PLAYERS_MAX, password/*m_password*/, JoinRoomCallback, NULL, (PEERBool)m_blocking);
//	if(result_joinStagingRoom==JSRR_Ok) return 1;
//	else return 0;
	peerStartReporting(m_peer);
	return 1;
}

bool GameSpyInterface::JoinStagingRoom(GUID ID)
{
	vector<sGameHostInfo*>::iterator p;
	for(p=gameHostList.begin(); p!=gameHostList.end(); p++){
		if( ID== (*p)->gameHostGUID ) {
			SBServer server=(SBServer)(*p)->pData;

			bool m_blocking=true;
			result_joinStagingRoom=JSRR_Processing;
			peerJoinStagingRoom(m_peer, server, ""/*m_password*/, JoinRoomCallback, NULL, (PEERBool)m_blocking);
			if(result_joinStagingRoom==JSRR_Ok) return 1;
			else return 0;
		}
	}
	return 0;
}

#include "Peer2\peer\peerMain.h"
#include "Peer2\peer\peerMangle.h"
GameSpyInterface::e_JoinStagingRoomResult GameSpyInterface::JoinStagingRoom(unsigned long ip, const char* password)
{
//	char room[PI_ROOM_MAX_LEN];
//	piMangleStagingRoom(room, PERIMETER_GAME_TITLE, ip, 0, 0);
//	bool m_blocking=true;
//	peerJoinStagingRoomByChannel(m_peer, room, ""/*m_password*/, JoinRoomCallback, NULL, (PEERBool)m_blocking);
//	return 1;

//	const unsigned int SLEEP_STEP=100;
//	const unsigned int MAX_SLEEP_TIME=10000;
//	int sleepTime=0;
//	do{
//		vector<sGameHostInfo*>::iterator p;
//		for(p=gameHostList.begin(); p!=gameHostList.end(); p++){
//			SBServer server=(SBServer)(*p)->pData;
//			if(server->publicip==ip){
//				bool m_blocking=true;
//				peerJoinStagingRoom(m_peer, server, ""/*m_password*/, JoinRoomCallback, NULL, (PEERBool)m_blocking);
//				return 1;
//			}
//		}
//		quant();
//		Sleep(SLEEP_STEP);
//	} while(sleepTime<MAX_SLEEP_TIME);
//	return 0;

/*
	const unsigned int SLEEP_STEP=100;
	while(!serverListingComplete){
		quant();
		Sleep(SLEEP_STEP);
	}
	vector<sGameHostInfo*>::iterator p;
	for(p=gameHostList.begin(); p!=gameHostList.end(); p++){
		SBServer server=(SBServer)(*p)->pData;
		if(server->publicip==ip){
			bool m_blocking=true;
			result_joinStagingRoom=JSRR_Processing;
			peerJoinStagingRoom(m_peer, server, password, JoinRoomCallback, NULL, (PEERBool)m_blocking);
			//if(result_joinStagingRoom==JSRR_Ok) return 1;
			//else return 0;
			return result_joinStagingRoom;// должно быть или JSRR_Ok или JSRR_PasswordError
		}
	}
	//return 0;
	return JSRR_Error;
*/
	if(m_peer) peerStartPlaying(m_peer);
	return JSRR_Ok;
}


unsigned int GameSpyInterface::getHostIP(GUID ID)
{
	vector<sGameHostInfo*>::iterator p;
	for(p=gameHostList.begin(); p!=gameHostList.end(); p++){
		if( ID== (*p)->gameHostGUID ) {
			SBServer server=(SBServer)(*p)->pData;
			return server->publicip;
		}
	}
	return 0;
}


bool GameSpyInterface::quant(void)
{
	if(m_peer) peerThink(m_peer);
	if(result_Disconnect) return 0;
	else return 1;
}

extern "C"{
extern void piStopReporting(PEER peer);
}
void GameSpyInterface::StartGame(void)
{
	//if(m_peer) peerStartGame(m_peer, "", PEER_STOP_REPORTING);
	if(m_peer) piStopReporting(m_peer);
}
