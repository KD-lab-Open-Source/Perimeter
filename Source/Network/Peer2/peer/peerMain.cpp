/*
GameSpy Peer SDK 
Dan "Mr. Pants" Schoenblum
dan@gamespy.com

Copyright 1999-2001 GameSpy Industries, Inc

18002 Skypark Circle
Irvine, California 92614
949.798.4200 (Tel)
949.798.4299 (Fax)
devsupport@gamespy.com
*/

/*************
** INCLUDES **
*************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "peer.h"
#include "peerMain.h"
#include "peerCallbacks.h"
#include "peerGlobalCallbacks.h"
#include "peerOperations.h"
#include "peerPlayers.h"
#include "peerRooms.h"
#include "peerPing.h"
#include "peerSB.h"
#include "peerMangle.h"
#include "peerKeys.h"
#include "peerQR.h"
#include "peerHost.h"
#include "peerAutoMatch.h"

#include <iostream>

/************
** DEFINES **
************/
#define PEER_CONNECTED         assert(connection->connected);

#define PI_CHECK_SHUTDOWN      if(connection->shutdown && (connection->callbackDepth == 0))\
                               {\
                                   peerShutdown(peer);\
                               }

#define PI_INIT_FAILED         {\
                                   piShutdownCleanup(peer);\
                                   return NULL;\
                               }

#define PI_DO_BLOCKING         if(blocking)\
                               {\
                                   do\
                                   {\
                                       msleep(1);\
                                       piThink(peer, opID);\
                                   }\
                                   while(!piCheckBlockingID(peer, opID));\
								   PI_CHECK_SHUTDOWN\
                               }

#define PI_OP_ID               int opID = piGetNextID(peer)
#if 0
int opID;  // for visual assist
#endif

/***************
** PROTOTYPES **
***************/
static void piShutdownCleanup(PEER peer);
static void piDisconnectCleanup(PEER peer);
static void piThink(PEER peer,int ID);

/************
** GENERAL **
************/
static PEERBool piCheckBlockingID
(
	PEER peer,
	int ID
)
{
	return (PEERBool)(piIsOperationFinished(peer, ID) && piIsCallbackFinished(peer, ID));
}

static unsigned int piGetPrivateIP(void)
{
	HOSTENT * host;
	IN_ADDR * addr;
	int i;

	host = getlocalhost();
	if(!host)
		return 0;

	for(i = 0 ; host->h_addr_list[i] ; i++)
	{
		addr = (IN_ADDR *)host->h_addr_list[i];
		if(IsPrivateIP(addr))
			return addr->s_addr;
	}

	return 0;
}

PEER peerInitialize
(
	PEERCallbacks * callbacks
)
{
	PEER peer;
	piConnection * connection;

	assert(callbacks);

	// Init sockets.
	////////////////
	SocketStartUp();

	// Create an object.
	////////////////////
	connection = (piConnection *)gsimalloc(sizeof(piConnection));
	if(!connection)
		return NULL;
	memset(connection, 0, sizeof(piConnection));
	peer = (PEER)connection;

	// Chat.
	////////
	connection->chat = NULL;
	connection->nick[0] = '\0';
	connection->connecting = PEERFalse;
	connection->connected = PEERFalse;

	// Game.
	////////
	connection->privateIP = piGetPrivateIP();
	connection->title[0] = '\0';

	// ID.
	//////
	connection->nextID = 0;

	// Operations.
	//////////////
	if(!piOperationsInit(peer))
		PI_INIT_FAILED

	// Callbacks.
	/////////////
	connection->callbacks = *callbacks;
	if(!piCallbacksInit(peer))
		PI_INIT_FAILED

	// Keys.
	////////
	if(!piKeysInit(peer))
		PI_INIT_FAILED

	// Misc.
	////////
	connection->shutdown = PEERFalse;

	return peer;
}

void peerConnect
(
	PEER peer,
	const char * nick,
	int profileID,
	peerNickErrorCallback nickErrorCallback,
	peerConnectCallback connectCallback,
	void * param,
	PEERBool blocking
)
{
	PEERBool success = PEERTrue;

	PI_OP_ID;
	PEER_CONNECTION;

	assert(nick);
	assert(nick[0]);
	assert(profileID >= 0);
	assert(connectCallback);
	assert(strlen(nick) < PI_NICK_MAX_LEN);

	// Are we already connecting or connected?
	//////////////////////////////////////////
	if(connection->connected || connection->connecting)
		success = PEERFalse;

	// We must have a title set to connect.
	///////////////////////////////////////
	if(success && !connection->title[0])
		success = PEERFalse;

	if(success)
	{
		// Chat.
		////////
		connection->chat = NULL;
		strzcpy(connection->nick, nick, PI_NICK_MAX_LEN);
		connection->connected = PEERFalse;
		connection->connecting = PEERTrue;
		connection->nickErrorCallback = nickErrorCallback;

		// Misc.
		////////
		connection->profileID = profileID;
		connection->disconnect = PEERFalse;

		// Start connecting.
		////////////////////
		if(!piNewConnectOperation(peer, nick, connectCallback, param, opID))
		{
			success = PEERFalse;
			piDisconnectCleanup(peer);
		}
	}

	if(!success)
		piAddConnectCallback(peer, PEERFalse, connectCallback, param, opID);

	PI_DO_BLOCKING;
}

void peerRetryWithNick
(
	PEER peer,
	const char * nick
)
{
	PEER_CONNECTION;

	// Check that we're connecting.
	///////////////////////////////
	assert(connection->connecting);
	if(!connection->connecting)
		return;

	// Set the new nick we're using.
	////////////////////////////////
	if(nick && nick[0])
		strzcpy(connection->nick, nick, PI_NICK_MAX_LEN);

	// Retry with the new nick.
	///////////////////////////
	chatRetryWithNick(connection->chat, nick);
}

PEERBool peerIsConnected(PEER peer)
{
	PEER_CONNECTION;

	return connection->connected;
}

PEERBool piConnectTitle
(
	PEER peer
)
{
	// Rooms.
	/////////
	if(!piRoomsInit(peer))
		return PEERFalse;

	// Players.
	///////////
	if(!piPlayersInit(peer))
		return PEERFalse;

	// Ping.
	// If it fails, keep going.
	///////////////////////////
	piPingInit(peer);

	return PEERTrue;
}

void piDisconnectTitle
(
	PEER peer
)
{
	// Rooms.
	/////////
	piRoomsCleanup(peer);

	// Players.
	///////////
	piPlayersCleanup(peer);

	// Ping.
	////////
	piPingCleanup(peer);

	// AutoMatch.
	/////////////
	piStopAutoMatch(peer);
}

PEERBool peerSetTitle
(
	PEER peer,
	const char * title,
	const char * qrSecretKey,
	const char * sbName,
	const char * sbSecretKey,
	int sbGameVersion,
	int sbMaxUpdates,
	PEERBool natNegotiate,
	PEERBool pingRooms[NumRooms],
	PEERBool crossPingRooms[NumRooms]
)
{
	static PEERBool noPings[NumRooms];
	PEERBool pingTitleRoom;
	PEERBool xpingTitleRoom;

	PEER_CONNECTION;

	assert(title);
	assert(title[0]);
	assert(strlen(title) < PI_TITLE_MAX_LEN);
	assert(qrSecretKey);
	assert(sbName);
	assert(sbName[0]);
	assert(sbSecretKey);

	// Check if a title is set.
	///////////////////////////
	if(connection->title[0])
		peerClearTitle(peer);

	// Game.
	////////
	strcpy(connection->title, title);

	// Null pings means don't do pings.
	///////////////////////////////////
	if(!pingRooms)
		pingRooms = noPings;
	if(!crossPingRooms)
		crossPingRooms = noPings;

	// If staying in the title room, leave the room's ping setting alone.
	/////////////////////////////////////////////////////////////////////
	if(connection->stayInTitleRoom)
	{
		pingTitleRoom = connection->pingRoom[TitleRoom];
		xpingTitleRoom = connection->xpingRoom[TitleRoom];
	}

	// Save our ping settings.
	//////////////////////////
	memcpy(connection->pingRoom, pingRooms, sizeof(PEERBool) * NumRooms);
	memcpy(connection->xpingRoom, crossPingRooms, sizeof(PEERBool) * NumRooms);

	// If staying in the title room, leave the room's ping setting alone.
	/////////////////////////////////////////////////////////////////////
	if(connection->stayInTitleRoom)
	{
		connection->pingRoom[TitleRoom] = pingTitleRoom;
		connection->xpingRoom[TitleRoom] = xpingTitleRoom;
	}

	// Save SB settings.
	////////////////////
	strzcpy(connection->sbName, sbName, PI_SB_LEN);
	strzcpy(connection->sbSecretKey, sbSecretKey, PI_SB_LEN);
	connection->sbGameVersion = sbGameVersion;
	connection->sbMaxUpdates = sbMaxUpdates;

	// Init SB.
	///////////
	if(!piSBInit(peer))
		return PEERFalse;

	// If we're already connected, do the connect stuff.
	////////////////////////////////////////////////////
	if(connection->connected)
	{
		if(!piConnectTitle(peer))
		{
			peerClearTitle(peer);
			return PEERFalse;
		}
	}

	// Hosting.
	///////////
	strcpy(connection->qrSecretKey, qrSecretKey);
	piStopHosting(peer, PEERTrue);
	connection->hosting = PEERFalse;
	connection->playing = PEERFalse;
	connection->natNegotiate = natNegotiate;

	// Game states.
	///////////////
	connection->ready = PEERFalse;

	// Make sure the "stay in title room" setting is cleared.
	/////////////////////////////////////////////////////////
	connection->stayInTitleRoom = PEERFalse;

	return PEERTrue;
}

void peerClearTitle
(
	PEER peer
)
{
	PEER_CONNECTION;

	// Stop hosting if we are.
	//////////////////////////
	piStopHosting(peer, PEERTrue);

	// Cleanup SB.
	//////////////
	piSBCleanup(peer);

	// Cleanup title stuff.
	///////////////////////
	piDisconnectTitle(peer);

	// Cleanup game.
	////////////////
	connection->title[0] = '\0';

	// Cleanup qr secret key.
	/////////////////////////
	connection->qrSecretKey[0] = '\0';
}

const char * peerGetTitle
(
	PEER peer
)
{
	PEER_CONNECTION;

	if(!connection->title[0])
		return NULL;

	return connection->title;
}

static void piDisconnectCleanup
(
	PEER peer
)
{
	PEER_CONNECTION;

	// Chat.
	////////
	if(connection->chat)
		chatDisconnect(connection->chat);
	connection->chat = NULL;
	connection->nick[0] = '\0';
	connection->connecting = PEERFalse;
	connection->connected = PEERFalse;
	
	// Operations.
	//////////////
	piOperationsReset(peer);

	// Title.
	/////////
	piDisconnectTitle(peer);

	// Away.
	////////
	connection->away = PEERFalse;
	connection->awayReason[0] = '\0';

	// We're not trying to disconnect.
	//////////////////////////////////
	connection->disconnect = PEERFalse;
}

static void piDisconnect
(
	PEER peer
)
{
	PEER_CONNECTION;

	// Are we within a callback?
	////////////////////////////
	if(connection->callbackDepth > 0)
	{
		// Flag for disconnect later.
		/////////////////////////////
		connection->disconnect = PEERTrue;

		return;
	}

	// Can't stay in the title room if we're disconnecting.
	///////////////////////////////////////////////////////
	connection->stayInTitleRoom = PEERFalse;

	// Cleanup the connection.
	//////////////////////////
	piDisconnectCleanup(peer);

	// Think to make sure the disconnected callback gets called.
	////////////////////////////////////////////////////////////
	piThink(peer, -1);
}

void peerDisconnect
(
	PEER peer
)
{
	PEER_CONNECTION;

	// Do the disconnect.
	/////////////////////
	piDisconnect(peer);

	// Check if we got shutdown in the disconnect callback.
	///////////////////////////////////////////////////////
	PI_CHECK_SHUTDOWN
}

static void piShutdownCleanup
(
	PEER peer
)
{
	PEER_CONNECTION;

	// Operations.
	//////////////
	piOperationsCleanup(peer);

	// Callbacks.
	/////////////
	piCallbacksCleanup(peer);

	// Shut down sockets.
	/////////////////////
	SocketShutDown();

	// Keys.
	////////
	piKeysCleanup(peer);

	// gsifree the connection.
	///////////////////////
	gsifree(connection);
}

void peerShutdown
(
	PEER peer
)
{
	PEER_CONNECTION;

	// Cleanup the connection?
	//////////////////////////
	if(connection->connected || connection->connecting)
	{
		peerDisconnectedCallback callback;

		// We don't want the disconnected callback
		// called if we're being explicitly shutdown.
		/////////////////////////////////////////////
		callback = connection->callbacks.disconnected;
		connection->callbacks.disconnected = NULL;
		piDisconnect(peer);
		connection->callbacks.disconnected = callback;
	}

	// Cleanup title if needed.
	///////////////////////////
	if(connection->title[0])
		peerClearTitle(peer);

	// Are we within a callback?
	////////////////////////////
	if(connection->callbackDepth > 0)
	{
		// Flag for shutdown later.
		///////////////////////////
		connection->shutdown = PEERTrue;

		return;
	}

	// Cleanup.
	///////////
	piShutdownCleanup(peer);
}

#ifdef _DEBUG
static void piNumPlayersConsistencyCheckMap
(
	piPlayer * player,
	int count[NumRooms]
)
{
	int i;

	assert(player);
	assert(count);

	for(i = 0 ; i < NumRooms ; i++)
	{
		if(player->inRoom[i])
			count[i]++;
	}
}
#endif

static void piThink
(
	PEER peer,
	int opID
)
{
	unsigned long now;
	PEER_CONNECTION;

#ifdef _DEBUG
	if(connection->players)
	{
		// Consistency check number of players in each room.
		////////////////////////////////////////////////////
		{
			int count[NumRooms];
			int i;

			// Init the counts to 0.
			////////////////////////
			for(i = 0 ; i < NumRooms ; i++)
				count[i] = 0;

			// Map through the players checking the count.
			//////////////////////////////////////////////
			TableMap(connection->players, (TableMapFn)piNumPlayersConsistencyCheckMap, count);

			// Check the counts.
			////////////////////
			for(i = 0 ; i < NumRooms ; i++)
				assert(count[i] == connection->numPlayers[i]);
		}
	}
#endif

#if 0
	// Show info.
	/////////////
	{
		char buffer[1024];
		static int counter = -1;

		counter++;
		counter %= 20;
		if(counter == 0)
		{
			sprintf(buffer,
				"---------------------\n"
				"operationsStarted:  %d\n"
				"operationsFinished: %d\n"
				"callbacksQueued:    %d\n"
				"callbacksCalled:    %d\n"
				"callbackDepth:      %d\n"
				"titleRoomPlayers:   %d\n"
				"groupRoomPlayers:   %d\n"
				"stagingRoomPlayers: %d\n",
				connection->operationsStarted,
				connection->operationsFinished,
				connection->callbacksQueued,
				connection->callbacksCalled,
				connection->callbackDepth,
				connection->numPlayers[TitleRoom],
				connection->numPlayers[GroupRoom],
				connection->numPlayers[StagingRoom]);
			OutputDebugString(buffer);
		}
	}
#endif

	// Let chat think.
	//////////////////
	if(connection->connected || connection->connecting)
	{
		chatThink(connection->chat);

		// Only do this if we weren't disconnected.
		///////////////////////////////////////////
		if(!connection->disconnect)
		{
			// Is a title set?
			//////////////////
			if(connection->title[0])
			{
				// Do ping stuff.
				/////////////////
				piPingThink(peer);
			}

			// Are we connected?
			////////////////////
			if(connection->connected)
			{
				// Ge the current time.
				///////////////////////
				now = current_time();

				// Check if we need to ping the chat server.
				////////////////////////////////////////////
				if((now - connection->lastChatPing) > PI_CHAT_PING_TIME)
				{
					// Send the ping.
					/////////////////
					chatSendRaw(connection->chat, "PING");

					// Store the current time.
					//////////////////////////
					connection->lastChatPing = now;
				}
			}
		}
	}

	// Let SB think.
	////////////////
	piSBThink(peer);

	// Let query-reporting think.
	/////////////////////////////
	piQRThink(peer);

	// If we got disconnected from chat, do the cleanup before calling the callback.
	////////////////////////////////////////////////////////////////////////////////
	if(connection->disconnect && (connection->callbackDepth == 0))
		piDisconnect(peer);

	// Let the callbacks think.
	///////////////////////////
	piCallbacksThink(peer, opID);
}

void peerThink
(
	PEER peer
)
{
	PEER_CONNECTION;

	// Think.
	/////////
	piThink(peer, -1);

	PI_CHECK_SHUTDOWN
}

CHAT peerGetChat
(
	PEER peer
)
{
	PEER_CONNECTION;
	PEER_CONNECTED;

	// Check if connected.
	//////////////////////
	if(!connection->connected)
		return NULL;

	// Return it.
	/////////////
	return connection->chat;
}

const char * peerGetNick
(
	PEER peer
)
{
	PEER_CONNECTION;
	PEER_CONNECTED;

	// Check if connected.
	//////////////////////
	if(!connection->connected)
		return NULL;

	// Return it.
	/////////////
	return connection->nick;
}

unsigned int peerGetPublicIP
(
	PEER peer
)
{
	PEER_CONNECTION;

	// Return it.
	/////////////
	return connection->publicIP;
}

unsigned int peerGetPrivateIP
(
	PEER peer
)
{
	PEER_CONNECTION;

	// Return it.
	/////////////
	return connection->privateIP;
}

void peerChangeNick
(
	PEER peer,
	const char * newNick,
	peerChangeNickCallback callback,
	void * param,
	PEERBool blocking
)
{
	PEERBool success = PEERTrue;

	PI_OP_ID;
	PEER_CONNECTION;
	PEER_CONNECTED;
	
	assert(callback);

	// Start the operation.
	///////////////////////
	if(!piNewChangeNickOperation(peer, newNick, callback, param, opID))
		success = PEERFalse;

	// Check for failure.
	/////////////////////
	if(!success)
		piAddChangeNickCallback(peer, PEERFalse, connection->nick, newNick, callback, param, opID);

	PI_DO_BLOCKING;
}

void peerStayInRoom
(
	PEER peer,
	RoomType roomType
)
{
	PEER_CONNECTION;
	PEER_CONNECTED;

	assert(roomType == TitleRoom);
	if(roomType != TitleRoom)
		return;

	if(!connection->title[0])
		return;

	connection->stayInTitleRoom = PEERTrue;
}

void peerSetQuietMode
(
	PEER peer,
	PEERBool quiet
)
{
	PEER_CONNECTION;
	PEER_CONNECTED;

	chatSetQuietMode(connection->chat, (CHATBool)quiet);
}

void peerSetAwayMode
(
	PEER peer,
	const char * reason
)
{
	char buffer[PI_AWAY_MAX_LEN + 6];
	PEER_CONNECTION;
	PEER_CONNECTED;

	if(!reason)
		reason = "";

	// Store the setting.
	/////////////////////
	connection->away = (PEERBool)(reason[0] != '\0');
	strzcpy(connection->awayReason, reason, PI_AWAY_MAX_LEN);

	// Set the flags.
	/////////////////
	piSetLocalFlags(peer);

	// Send the chat command.
	/////////////////////////
	sprintf(buffer, "AWAY :%s", connection->awayReason);
	chatSendRaw(connection->chat, buffer);
}

void peerParseQuery
(
	PEER peer,
	char * query,
	int len,
	struct sockaddr * sender
)
{
	PEER_CONNECTION;

	assert(query);
	assert(sender);

	// Handle the query based on what type of reporting we're doing.
	////////////////////////////////////////////////////////////////
	if(connection->queryReporting)
		qr2_parse_query(connection->queryReporting, query, len, sender);
	else if(connection->autoMatchReporting)
		qr2_parse_query(connection->autoMatchReporting, query, len, sender);
}

void peerAuthenticateCDKey
(
	PEER peer,
	const char * cdkey,
	peerAuthenticateCDKeyCallback callback,
	void * param,
	PEERBool blocking
)
{
	PEERBool success = PEERTrue;

	PI_OP_ID;
	PEER_CONNECTION;
	PEER_CONNECTED;
	
	assert(callback);

	// Start the operation.
	///////////////////////
	if(!piNewAuthenticateCDKeyOperation(peer, cdkey, callback, param, opID))
		success = PEERFalse;

	// Check for failure.
	/////////////////////
	if(!success)
		piAddAuthenticateCDKeyCallback(peer, 0, "Error starting CD Key check", callback, param, opID);

	PI_DO_BLOCKING;
}

void peerSendNatNegotiateCookie
(
	PEER peer,
	unsigned int ip,
	unsigned short port,
	int cookie
)
{
	piSendNatNegotiateCookie(peer, ip, port, cookie);
}

void peerAlwaysGetPlayerInfo
(
	PEER peer,
	PEERBool always
)
{
	PEER_CONNECTION;

	connection->alwaysRequestPlayerInfo = always;
}

/**********
** ROOMS **
**********/
void peerJoinTitleRoom
(
	PEER peer,
	const char password[PEER_PASSWORD_LEN],
	peerJoinRoomCallback callback,
	void * param,
	PEERBool blocking
)
{
	PEERBool success = PEERTrue;
	PEERJoinResult result = PEERJoinFailed;
	char buffer[PI_ROOM_MAX_LEN];

	PI_OP_ID;
	PEER_CONNECTION;
	PEER_CONNECTED;
	
	assert(callback);

	// NULL password is the same as empty password.
	///////////////////////////////////////////////
	if(!password)
		password = "";

	// Check for a title.
	/////////////////////
	if(!connection->title[0])
	{
		success = PEERFalse;
		result = PEERNoTitleSet;
	}

	// Check for a connection.
	//////////////////////////
	if(success && !connection->connected)
	{
		success = PEERFalse;
		result = PEERNoConnection;
	}

	// Check if we're in the title room.
	////////////////////////////////////
	assert(!connection->enteringRoom[TitleRoom] && !connection->inRoom[TitleRoom]);
	if((success && connection->enteringRoom[TitleRoom]) || connection->inRoom[TitleRoom])
	{
		success = PEERFalse;
		result = PEERAlreadyInRoom;
	}

	// Check if we're AutoMatching.
	///////////////////////////////
	assert(!peerIsAutoMatching(peer));
	if(success && peerIsAutoMatching(peer))
	{
		success = PEERFalse;
		result = PEERAutoMatching;
	}

	// Get the room name.
	/////////////////////
	if(success)
	{
		if(connection->titleRoomChannel[0])
			strcpy(buffer, connection->titleRoomChannel);
		else
			piMangleTitleRoom(buffer, connection->title);
	}

	// Start the operation.
	///////////////////////
	if(success && !piNewJoinRoomOperation(peer, TitleRoom, buffer, password, callback, param, opID))
		success = PEERFalse;

	// Check for failure.
	/////////////////////
	if(!success)
		piAddJoinRoomCallback(peer, PEERFalse, result, TitleRoom, callback, param, opID);

	PI_DO_BLOCKING;
}

void peerJoinGroupRoom
(
	PEER peer,
	int groupID,
	peerJoinRoomCallback callback,
	void * param,
	PEERBool blocking
)
{
	PEERBool success = PEERTrue;
	PEERJoinResult result = PEERJoinFailed;
	char room[PI_ROOM_MAX_LEN];

	PI_OP_ID;
	PEER_CONNECTION;
	PEER_CONNECTED;

	assert(callback);

	// Check for a title.
	/////////////////////
	if(!connection->title[0])
	{
		success = PEERFalse;
		result = PEERNoTitleSet;
	}

	// Check for a connection.
	//////////////////////////
	if(success && !connection->connected)
	{
		success = PEERFalse;
		result = PEERNoConnection;
	}

	// Check if we're AutoMatching.
	///////////////////////////////
	assert(!peerIsAutoMatching(peer));
	if(success && peerIsAutoMatching(peer))
	{
		success = PEERFalse;
		result = PEERAutoMatching;
	}

	// Check the ID.
	////////////////
	assert(groupID);
	if(success && !groupID)
		success = PEERFalse;

	// Check if we're in a group room.
	//////////////////////////////////
	if(success && (connection->enteringRoom[GroupRoom] || connection->inRoom[GroupRoom]))
	{
		success = PEERFalse;
		result = PEERAlreadyInRoom;
	}

	// Create the name.
	///////////////////
	piMangleGroupRoom(room, groupID);

	// Save off the group id.
	/////////////////////////
	connection->groupID = groupID;

	// Start the operation.
	///////////////////////
	if(success && !piNewJoinRoomOperation(peer, GroupRoom, room, NULL, callback, param, opID))
		success = PEERFalse;

	// Check for failure.
	/////////////////////
	if(!success)
		piAddJoinRoomCallback(peer, PEERFalse, result, GroupRoom, callback, param, opID);

	PI_DO_BLOCKING;
}

void peerSetGroupID
(
	PEER peer,
	int groupID
)
{
	PEER_CONNECTION;
	PEER_CONNECTED;

	// Check for a title.
	/////////////////////
	if(!connection->title[0])
		return;

	// Check for a connection.
	//////////////////////////
	if(!connection->connected)
		return;

	// Save off the group id.
	/////////////////////////
	connection->groupID = groupID;
}

int peerGetGroupID
(
	PEER peer
)
{
	PEER_CONNECTION;
	PEER_CONNECTED;

	// Check for a title.
	/////////////////////
	if(!connection->title[0])
		return 0;

	// Check for a connection.
	//////////////////////////
	if(!connection->connected)
		return 0;

	// Get the group id.
	////////////////////
	return connection->groupID;
}

static void piJoinStagingRoom
(
	PEER peer,
	SBServer server,
	const char * channel,
	const char password[PEER_PASSWORD_LEN],
	peerJoinRoomCallback callback,
	void * param,
	PEERBool blocking
)
{
	unsigned int publicIP;
	unsigned int privateIP;
	unsigned short privatePort;
	char room[PI_ROOM_MAX_LEN];
	PEERBool success = PEERTrue;
	PEERJoinResult result = PEERJoinFailed;

	PI_OP_ID;
	PEER_CONNECTION;
	PEER_CONNECTED;

	assert(callback);

	// NULL password is the same as empty password.
	///////////////////////////////////////////////
	if(!password)
		password = "";

	// Check for a title.
	/////////////////////
	if(!connection->title[0])
	{
		success = PEERFalse;
		result = PEERNoTitleSet;
	}

	// Check for a connection.
	//////////////////////////
	if(success && !connection->connected)
	{
		success = PEERFalse;
		result = PEERNoConnection;
	}

	// Check if we're in a staging room.
	////////////////////////////////////
	if(success && (connection->enteringRoom[StagingRoom] || connection->inRoom[StagingRoom]))
	{
		success = PEERFalse;
		result = PEERAlreadyInRoom;
	}

	// Check if we're AutoMatching.
	///////////////////////////////
	assert(!peerIsAutoMatching(peer));
	if(success && peerIsAutoMatching(peer))
	{
		success = PEERFalse;
		result = PEERAutoMatching;
	}

	// If we have a server, get the public and private IPs and ports.
	/////////////////////////////////////////////////////////////////
	if(success && server)
	{
		publicIP = SBServerGetPublicInetAddress(server);
		privateIP = SBServerGetPrivateInetAddress(server);
		if(SBServerHasPrivateAddress(server))
			privatePort = SBServerGetPrivateQueryPort(server);
		else
			privatePort = SBServerGetPublicQueryPort(server);

		if(!publicIP)
			success = PEERFalse;
	}

	// If we have a channel, check it.
	//////////////////////////////////
	if(success && !server)
	{
		assert(channel);
		assert(channel[0]);
		if(!channel || !channel[0])
			success = PEERFalse;
	}

	// Stop hosting.
	////////////////
	if(success)
		piStopHosting(peer, PEERTrue);

	// If we have a server, get the staging room.
	/////////////////////////////////////////////
	if(success && server)
		piMangleStagingRoom(room, connection->title, publicIP, privateIP, privatePort);

	// Start the operation.
	///////////////////////
	if(success	&& !piNewJoinRoomOperation(peer, StagingRoom, server?room:channel, password, callback, param, opID))
		success = PEERFalse;

	// If we have a server, clone it.
	/////////////////////////////////
	if(success && server)
		connection->hostServer = piSBCloneServer(server);

	// Check for failure.
	/////////////////////
	if(!success)
		piAddJoinRoomCallback(peer, PEERFalse, result, StagingRoom, callback, param, opID);

	PI_DO_BLOCKING;
}

void peerJoinStagingRoom
(
	PEER peer,
	SBServer server,
	const char password[PEER_PASSWORD_LEN],
	peerJoinRoomCallback callback,
	void * param,
	PEERBool blocking
)
{
	piJoinStagingRoom(peer, server, NULL, password, callback, param, blocking);
}

void peerJoinStagingRoomByChannel
(
	PEER peer,
	const char * channel,
	const char password[PEER_PASSWORD_LEN],
	peerJoinRoomCallback callback,
	void * param,
	PEERBool blocking
)
{
	piJoinStagingRoom(peer, NULL, channel, password, callback, param, blocking);
}

void peerCreateStagingRoom
(
	PEER peer,
	const char * name,
	int maxPlayers,
	const char password[PEER_PASSWORD_LEN],
	peerJoinRoomCallback callback,
	void * param,
	PEERBool blocking
)
{
	peerCreateStagingRoomWithSocket(peer, name, maxPlayers, password, INVALID_SOCKET, 0, callback, param, blocking);
}

void peerCreateStagingRoomWithSocket
(
	PEER peer,
	const char * name,
	int maxPlayers,
	const char password[PEER_PASSWORD_LEN],
	SOCKET socket,
	unsigned short port,
	peerJoinRoomCallback callback,
	void * param,
	PEERBool blocking
)
{
	PEERBool success = PEERTrue;
	PEERJoinResult result = PEERJoinFailed;
	PI_OP_ID;

	PEER_CONNECTION;

	assert(name);
	assert(connection->title[0]);
	assert(callback);
	assert(maxPlayers >= 0);

	// NULL password is the same as empty password.
	///////////////////////////////////////////////
	if(!password)
		password = "";

	// Check for a title.
	/////////////////////
	if(!connection->title[0])
	{
		success = PEERFalse;
		result = PEERNoTitleSet;
	}

	// Check for a connection.
	//////////////////////////
	if(success && !connection->connected)
	{
		success = PEERFalse;
		result = PEERNoConnection;
	}

	// Check if we're in a staging room.
	////////////////////////////////////
	if(success && (connection->enteringRoom[StagingRoom] || connection->inRoom[StagingRoom]))
	{
		success = PEERFalse;
		result = PEERAlreadyInRoom;
	}

	// Check if we're AutoMatching.
	///////////////////////////////
	assert(!peerIsAutoMatching(peer));
	if(success && peerIsAutoMatching(peer))
	{
		success = PEERFalse;
		result = PEERAutoMatching;
	}

	// Stop hosting.
	////////////////
	if(success)
		piStopHosting(peer, PEERTrue);

	// Start the operation.
	///////////////////////
	if(success && !piNewCreateStagingRoomOperation(peer, name, password, maxPlayers, socket, port, callback, param, opID))
		success = PEERFalse;

	// Add callback if error.
	/////////////////////////
	if(!success)
		piAddJoinRoomCallback(peer, PEERFalse, result, StagingRoom, callback, param, opID);

	PI_DO_BLOCKING;
}

void peerLeaveRoom
(
	PEER peer,
	RoomType roomType,
	const char * reason
)
{
	PEER_CONNECTION;
	PEER_CONNECTED;

	ASSERT_ROOMTYPE(roomType);

	// Check for a title.
	/////////////////////
	if(!connection->title[0])
		return;

	// Check for a connection.
	//////////////////////////
	if(!connection->connected)
		return;

	// Check if we're in or entering.
	/////////////////////////////////
	if(!ENTERING_ROOM && !IN_ROOM)
		return;

	// Leave.
	/////////
	piLeaveRoom(peer, roomType, reason);

	// Is this an AutoMatch room?
	/////////////////////////////
	if((roomType == StagingRoom) && peerIsAutoMatching(peer))
	{
		// Go back to searching.
		////////////////////////
		piSetAutoMatchStatus(peer, PEERSearching);
	}
}

void peerListGroupRooms
(
	PEER peer,
	const char * fields,
	peerListGroupRoomsCallback callback,
	void * param,
	PEERBool blocking
)
{
	PEERBool success = PEERTrue;
	PI_OP_ID;
	PEER_CONNECTION;

	assert(callback);

	// Check for a title.
	/////////////////////
	if(!connection->title[0])
		success = PEERFalse;

	// Can't have a NULL fields.
	////////////////////////////
	if(!fields)
		fields = "";

	// Start the listing.
	/////////////////////
	if(success && !piNewListGroupRoomsOperation(peer, fields, callback, param, opID))
		success = PEERFalse;

	// Call the callback if failed.
	///////////////////////////////
	if(!success)
		piAddListGroupRoomsCallback(peer, PEERFalse, 0, NULL, NULL, 0, 0, 0, 0, callback, param, opID);

	PI_DO_BLOCKING;
}

void peerStartListingGames
(
	PEER peer,
	const unsigned char * fields,
	int numFields,
	const char * filter,
	peerListingGamesCallback callback,
	void * param
)
{
	PEERBool success;

	PEER_CONNECTION;

	assert(callback);

	// Check for a title.
	/////////////////////
	if(!connection->title[0])
		return;

	// Can't have an empty filter.
	//////////////////////////////
	if(filter && !filter[0])
		filter = NULL;

	// Check the fields.
	////////////////////
	if(!fields || (numFields <= 0))
		numFields = 0;

	// Save the callback info.
	//////////////////////////
	connection->gameListCallback = callback;
	connection->gameListParam = param;

	// Start the listing.
	/////////////////////
	success = piSBStartListingGames(peer, fields, numFields, filter);

	// Call the callback if failed.
	///////////////////////////////
	if(!success)
		piAddListingGamesCallback(peer, PEERFalse, NULL, 0);
}

void peerStopListingGames
(
	PEER peer
)
{
	PEER_CONNECTION;

	// Check for a title.
	/////////////////////
	if(!connection->title[0])
		return;

	// Stop the listing.
	////////////////////
	piSBStopListingGames(peer);
}

void peerUpdateGame
(
	PEER peer,
	SBServer server,
	PEERBool fullUpdate
)
{
	PEER_CONNECTION;

	assert(server);

	// Check for a title.
	/////////////////////
	if(!connection->title[0])
		return;

	// Update the server.
	/////////////////////
	piSBUpdateGame(peer, server, fullUpdate);
}

void peerMessageRoom
(
	PEER peer,
	RoomType roomType,
	const char * message,
	MessageType messageType
)
{
	PEER_CONNECTION;
	PEER_CONNECTED;

	ASSERT_ROOMTYPE(roomType);
	ASSERT_MESSAGETYPE(messageType);

	// Check for no message.
	////////////////////////
	if(!message || !message[0])
		return;

	// Check that we're in this room.
	/////////////////////////////////
	assert(IN_ROOM);
	if(!IN_ROOM)
		return;

	// Send the message.
	////////////////////
	chatSendChannelMessage(connection->chat, ROOM, message, (int)messageType);
}

void peerUTMRoom
(
	PEER peer,
	RoomType roomType,
	const char * command,
	const char * parameters,
	PEERBool authenticate
)
{
	PEER_CONNECTION;
	PEER_CONNECTED;

	ASSERT_ROOMTYPE(roomType);

	// Check that we're in this room.
	/////////////////////////////////
	assert(IN_ROOM);
	if(!IN_ROOM)
		return;

	// Send it.
	///////////
	piSendChannelUTM(peer, ROOM, command, parameters, authenticate);
}

void peerSetPassword
(
	PEER peer,
	RoomType roomType,
	const char password[PEER_PASSWORD_LEN]
)
{
	PEER_CONNECTION;
	PEER_CONNECTED;

	ASSERT_ROOMTYPE(roomType);

	// Check room type.
	////////////////////
	assert(roomType == StagingRoom);
	if(roomType != StagingRoom)
		return;

	// NULL password is the same as empty password.
	///////////////////////////////////////////////
	if(!password)
		password = "";

	// Check for a title.
	/////////////////////
	if(!connection->title[0])
		return;

	// Check for a connection.
	//////////////////////////
	if(!connection->connected)
		return;

	// Check if we're in or entering.
	/////////////////////////////////
	if(!ENTERING_ROOM && !IN_ROOM)
		return;

	// Make sure we're hosting.
	///////////////////////////
	assert(connection->hosting);
	if(!connection->hosting)
		return;

	// Set/clear the password.
	//////////////////////////
	if(password[0])
		chatSetChannelPassword(connection->chat, ROOM, CHATTrue, password);
	else
		chatSetChannelPassword(connection->chat, ROOM, CHATFalse, "x");

	// Set the passworded flag.
	///////////////////////////
	connection->passwordedRoom = password[0]?PEERTrue:PEERFalse;

	// Send a state-changed.
	////////////////////////
	piSendStateChanged(peer);
}

void peerSetRoomName
(
	PEER peer,
	RoomType roomType,
	const char * name
)
{
	PEER_CONNECTION;
	PEER_CONNECTED;

	ASSERT_ROOMTYPE(roomType);

	assert(roomType == StagingRoom);

	// NULL name is the same as empty name.
	///////////////////////////////////////
	if(!name)
		name = "";

	// Check for a title.
	/////////////////////
	if(!connection->title[0])
		return;

	// Check for a connection.
	//////////////////////////
	if(!connection->connected)
		return;

	// Check if we're in or entering.
	/////////////////////////////////
	if(!ENTERING_ROOM && !IN_ROOM)
		return;

	// Make sure we're hosting.
	///////////////////////////
	assert(connection->hosting);
	if(!connection->hosting)
		return;

	// Set it.
	//////////
	chatSetChannelTopic(connection->chat, ROOM, name);
}

const char * peerGetRoomName
(
	PEER peer,
	RoomType roomType
)
{
	PEER_CONNECTION;
	PEER_CONNECTED;

	ASSERT_ROOMTYPE(roomType);
	assert(IN_ROOM);
	if(!IN_ROOM)
		return NULL;

	return NAME;
}

const char * peerGetRoomChannel
(
	PEER peer,
	RoomType roomType
)
{
	PEER_CONNECTION;
	PEER_CONNECTED;

	ASSERT_ROOMTYPE(roomType);
	assert(IN_ROOM || ENTERING_ROOM);
	if(!IN_ROOM && ! ENTERING_ROOM)
		return NULL;

	return ROOM;
}

PEERBool peerInRoom
(
	PEER peer,
	RoomType roomType
)
{
	PEER_CONNECTION;
	PEER_CONNECTED;

	ASSERT_ROOMTYPE(roomType);

	return IN_ROOM;
}

void peerSetTitleRoomChannel
(
	PEER peer,
	const char * channel
)
{
	PEER_CONNECTION;
	PEER_CONNECTED;

	// Check for a title.
	/////////////////////
	if(!connection->title[0])
		return;

	// Check for a connection.
	//////////////////////////
	if(!connection->connected)
		return;

	// Check for no channel.
	////////////////////////
	if(!channel)
		channel = "";

	// Copy it.
	///////////
	strzcpy(connection->titleRoomChannel, channel, PI_ROOM_MAX_LEN);
}

SBServer peerGetHostServer(PEER peer)
{
	PEER_CONNECTION;
	PEER_CONNECTED;

	return connection->hostServer;
}

/************
** PLAYERS **
************/
typedef struct piEnumPlayersData
{
	peerEnumPlayersCallback callback;
	void * param;
} piEnumPlayersData;

static void piEnumPlayersEnumRoomPlayersCallback
(
	PEER peer,
	RoomType roomType,
	piPlayer * player,
	int index,
	void *param
)
{
	piEnumPlayersData * data = (piEnumPlayersData *)param;
	const char * nick;
	int flags;

	if(player)
	{
		nick = player->nick;
		flags = player->flags[roomType];
	}
	else
	{
		nick = NULL;
		flags = 0;
	}

	// Call the callback.
	/////////////////////
	data->callback(peer, PEERTrue, roomType, index, nick, flags, data->param);
}

void peerEnumPlayers
(
	PEER peer,
	RoomType roomType,
	peerEnumPlayersCallback callback,
	void * param
)
{
	PEERBool success = PEERTrue;
	piEnumPlayersData data;

	PEER_CONNECTION;
	PEER_CONNECTED;
	
	assert(callback);
	ASSERT_ROOMTYPE(roomType);

	// Check that we're in this room.
	/////////////////////////////////
	assert(IN_ROOM);
	if(success && !IN_ROOM)
		success = PEERFalse;

	// Check for failure.
	/////////////////////
	if(!success)
	{
		// Call the callback.
		/////////////////////
		callback(peer, PEERFalse, roomType, -1, NULL, PEERFalse, param);
		return;
	}

	// Enumerate through the players, using a local copy.
	/////////////////////////////////////////////////////
	data.callback = callback;
	data.param = param;
	piEnumRoomPlayers(peer, roomType, piEnumPlayersEnumRoomPlayersCallback, &data);
}

void peerMessagePlayer
(
	PEER peer,
	const char * nick,
	const char * message,
	MessageType messageType
)
{
	PEER_CONNECTION;
	PEER_CONNECTED;

	ASSERT_MESSAGETYPE(messageType);
	assert(nick);
	assert(nick[0]);

	// Check for connection succeeded.
	//////////////////////////////////
	if(!connection->connected)
		return;

	// Check for no message.
	////////////////////////
	if(!message || !message[0])
		return;

	// Send the message to this player.
	///////////////////////////////////
	chatSendUserMessage(connection->chat, nick, message, (int)messageType);
}

void peerUTMPlayer
(
	PEER peer,
	const char * nick,
	const char * command,
	const char * parameters,
	PEERBool authenticate
)
{
	// Send it.
	///////////
	piSendPlayerUTM(peer, nick, command, parameters, authenticate);
}

void peerKickPlayer
(
	PEER peer,
	RoomType roomType,
	const char * nick,
	const char * reason
)
{
	PEER_CONNECTION;
	PEER_CONNECTED;

	ASSERT_ROOMTYPE(roomType);
	assert(IN_ROOM || ENTERING_ROOM);
	assert(nick);
	assert(nick[0]);

	// Check for connection succeeded.
	//////////////////////////////////
	if(!connection->connected)
		return;

	// Kick the player.
	///////////////////
	chatKickUser(connection->chat, ROOM, nick, reason);
}

PEERBool peerGetPlayerPing
(
	PEER peer,
	const char * nick,
	int * ping
)
{
	piPlayer * player;

	PEER_CONNECTION;
	PEER_CONNECTED;

	assert(nick);
	assert(nick[0]);
	assert(ping);

	// Get the player.
	//////////////////
	player = piGetPlayer(peer, nick);
	if(!player)
		return PEERFalse;

	// Is it the local player?
	//////////////////////////
	if(player->local)
	{
		*ping = 0;
	}
	else
	{
		// Check if there's a ping.
		///////////////////////////
		if(!player->numPings)
			return PEERFalse;

		*ping = player->pingAverage;
	}

	return PEERTrue;
}

PEERBool peerGetPlayersCrossPing
(
	PEER peer,
	const char * nick1,
	const char * nick2,
	int * crossPing
)
{
	PEER_CONNECTION;
	PEER_CONNECTED;

	assert(nick1);
	assert(nick1[0]);
	assert(nick2);
	assert(nick2[0]);
	assert(crossPing);

	// Do it.
	/////////
	return piGetXping(peer, nick1, nick2, crossPing);
}

PEERBool peerGetPlayerInfoNoWait
(
	PEER peer,
	const char * nick,
	unsigned int * IP,
	int * profileID
)
{
	piPlayer * player;

	PEER_CONNECTION;
	PEER_CONNECTED;

	assert(nick);

	player = piGetPlayer(peer, nick);
	if(!player || !player->gotIPAndProfileID)
	{
		const char * info;
		unsigned int locIP;
		int locProfileID;

		// Can we get it from chat?
		///////////////////////////
		if(chatGetBasicUserInfoNoWait(connection->chat, nick, &info, NULL) && piDemangleUser(info, &locIP, &locProfileID))
		{
			if(player)
				piSetPlayerIPAndProfileID(peer, nick, locIP, locProfileID);

			if(IP)
				*IP = locIP;
			if(profileID)
				*profileID = locProfileID;

			return PEERTrue;
		}
		return PEERFalse;
	}

	if(IP)
		*IP = player->IP;
	if(profileID)
		*profileID = player->profileID;

	return PEERTrue;
}

void peerGetPlayerInfo
(
	PEER peer,
	const char * nick,
	peerGetPlayerInfoCallback callback,
	void * param,
	PEERBool blocking
)
{
	PEERBool success = PEERTrue;
	piPlayer * player;
	PI_OP_ID;

	PEER_CONNECTION;
	PEER_CONNECTED;

	assert(callback);
	assert(nick);
	assert(nick[0]);
	assert(callback);

	// Find the player.
	///////////////////
	player = piGetPlayer(peer, nick);

	// Check if chat has it.
	////////////////////////
	if(player && !player->gotIPAndProfileID)
	{
		const char * info;
		unsigned int IP;
		int profileID;

		if(chatGetBasicUserInfoNoWait(connection->chat, nick, &info, NULL) && piDemangleUser(info, &IP, &profileID))
		{
			piSetPlayerIPAndProfileID(peer, nick, IP, profileID);
		}
	}

	// See if we already have it.
	/////////////////////////////
	if(player && player->gotIPAndProfileID)
	{
		piAddGetPlayerInfoCallback(peer, PEERTrue, nick, player->profileID, player->IP, callback, param, opID);
	}
	else
	{
		// Start an op to get it.
		/////////////////////////
		if(!piNewGetPlayerInfoOperation(peer, nick, callback, param, opID))
			success = PEERFalse;
	}

	// If failed, add the callback.
	///////////////////////////////
	if(!success)
		piAddGetPlayerInfoCallback(peer, PEERFalse, nick, 0, 0, callback, param, opID);

	PI_DO_BLOCKING;
}

void peerGetPlayerProfileID
(
	PEER peer,
	const char * nick,
	peerGetPlayerProfileIDCallback callback,
	void * param,
	PEERBool blocking
)
{
	PEERBool success = PEERTrue;
	piPlayer * player;
	PI_OP_ID;

	PEER_CONNECTION;
	PEER_CONNECTED;

	assert(callback);
	assert(nick);
	assert(nick[0]);
	assert(callback);

	// Find the player.
	///////////////////
	player = piGetPlayer(peer, nick);

	// Check if chat has it.
	////////////////////////
	if(player && !player->gotIPAndProfileID)
	{
		const char * info;
		unsigned int IP;
		int profileID;

		if(chatGetBasicUserInfoNoWait(connection->chat, nick, &info, NULL) && piDemangleUser(info, &IP, &profileID))
		{
			piSetPlayerIPAndProfileID(peer, nick, IP, profileID);
		}
	}

	// See if we already have it.
	/////////////////////////////
	if(player && player->gotIPAndProfileID)
	{
		piAddGetPlayerProfileIDCallback(peer, PEERTrue, nick, player->profileID, callback, param, opID);
	}
	else
	{
		// Start an op to get it.
		/////////////////////////
		if(!piNewGetProfileIDOperation(peer, nick, callback, param, opID))
			success = PEERFalse;
	}

	// If failed, add the callback.
	///////////////////////////////
	if(!success)
		piAddGetPlayerProfileIDCallback(peer, PEERFalse, nick, 0, callback, param, opID);

	PI_DO_BLOCKING;
}

void peerGetPlayerIP
(
	PEER peer,
	const char * nick,
	peerGetPlayerIPCallback callback,
	void * param,
	PEERBool blocking
)
{
	PEERBool success = PEERTrue;
	piPlayer * player;
	PI_OP_ID;

	PEER_CONNECTION;
	PEER_CONNECTED;

	assert(callback);
	assert(nick);
	assert(nick[0]);
	assert(callback);

	// Find the player.
	///////////////////
	player = piGetPlayer(peer, nick);

	// Check if chat has it.
	////////////////////////
	if(player && !player->gotIPAndProfileID)
	{
		const char * info;
		unsigned int IP;
		int profileID;

		if(chatGetBasicUserInfoNoWait(connection->chat, nick, &info, NULL) && piDemangleUser(info, &IP, &profileID))
		{
			piSetPlayerIPAndProfileID(peer, nick, IP, profileID);
		}
	}

	// Check if we already have it.
	///////////////////////////////
	if(player && player->gotIPAndProfileID)
	{
		piAddGetPlayerIPCallback(peer, PEERTrue, nick, player->IP, callback, param, opID);
	}
	else
	{
		// Start an op to get it.
		/////////////////////////
		if(!piNewGetIPOperation(peer, nick, callback, param, opID))
			success = PEERFalse;
	}

	// If failed, add the callback.
	///////////////////////////////
	if(!success)
		piAddGetPlayerIPCallback(peer, PEERFalse, nick, 0, callback, param, opID);

	PI_DO_BLOCKING;
}

PEERBool peerIsPlayerHost
(
	PEER peer,
	const char * nick,
	RoomType roomType
)
{
	piPlayer * player;

	PEER_CONNECTION;
	PEER_CONNECTED;

	// Are we in this type of room?
	///////////////////////////////
	assert(IN_ROOM);
	if(!IN_ROOM)
		return PEERFalse;

	// Get the player.
	//////////////////
	player = piGetPlayer(peer, nick);
	if(!player)
		return PEERFalse;

	// If it's the local player, return the value we store.
	///////////////////////////////////////////////////////
	if(player->local)
		return connection->hosting;

	// Is he host?
	//////////////
	return piIsPlayerHost(player);
}

PEERBool peerGetPlayerFlags
(
	PEER peer,
	const char * nick,
	RoomType roomType,
	int * flags
)
{
	piPlayer * player;

	PEER_CONNECTION;
	PEER_CONNECTED;

	assert(flags);
	if(!flags)
		return PEERFalse;

	// Are we in this type of room?
	///////////////////////////////
	assert(IN_ROOM);
	if(!IN_ROOM)
		return PEERFalse;

	// Get the player.
	//////////////////
	player = piGetPlayer(peer, nick);
	if(!player)
		return PEERFalse;

	// Is he in?
	////////////
	if(!player->inRoom[roomType])
		return PEERFalse;

	// Get the flags.
	/////////////////
	*flags = player->flags[roomType];

	return PEERTrue;
}

/*********
** GAME **
*********/
void peerSetReady
(
	PEER peer,
	PEERBool ready
)
{
	PEER_CONNECTION;
	PEER_CONNECTED;

	// Check for a title.
	/////////////////////
	if(!connection->title[0])
		return;

	// Check for a connection.
	//////////////////////////
	if(!connection->connected)
		return;

	// Are we in a staging room?
	////////////////////////////
	assert(connection->inRoom[StagingRoom]);
	if(!connection->inRoom[StagingRoom])
		return;

	// Don't do anything if the state isn't changing.
	/////////////////////////////////////////////////
	if(connection->ready == ready)
		return;

	// Set it.
	//////////
	connection->ready = ready;

	// Set the flags.
	/////////////////
	piSetLocalFlags(peer);

#if 1
	// Send an old-style ready notice.
	// THIS IS ONLY NEEDED FOR BACKWARDS COMPATIBILITY AND SHOULD BE REMOVED AT SOME POINT IN THE FUTURE.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	{
	char buffer[32];
	IN_ADDR addr;
	addr.s_addr = connection->publicIP;
	strcpy(buffer, "@@@NFO \\$flags$\\");
	if(ready)
		strcat(buffer, "r");
	strcat(buffer, "X\\");  // Flag to indicate this was sent by a new client.
	peerMessageRoom(peer, StagingRoom, buffer, NormalMessage);
	}
#endif
}

PEERBool peerGetReady
(
	PEER peer,
	const char * nick,
	PEERBool * ready
)
{
	piPlayer * player;

	PEER_CONNECTION;
	PEER_CONNECTED;

	assert(nick);
	assert(nick[0]);
	assert(ready);

	// Check for a title.
	/////////////////////
	if(!connection->title[0])
		return PEERFalse;

	// Check for a connection.
	//////////////////////////
	if(!connection->connected)
		return PEERFalse;

	// Are we in a staging room?
	////////////////////////////
	assert(connection->inRoom[StagingRoom]);
	if(!connection->inRoom[StagingRoom])
		return PEERFalse;

	// Get the player.
	//////////////////
	player = piGetPlayer(peer, nick);
	if(!player || !player->inRoom[StagingRoom])
		return PEERFalse;

	*ready = (PEERBool)((player->flags[StagingRoom] & PEER_FLAG_READY) != 0);

	return PEERTrue;
}

static void piAreAllReadyEnumRoomPlayersCallback
(
	PEER peer,
	RoomType roomType,
	piPlayer * player,
	int index,
	void *param
)
{
	if(player)
	{
		PEERBool * allReadyPtr = (PEERBool *)param;

		// If this player's not ready, set the flag.
		////////////////////////////////////////////
		if(!(player->flags[StagingRoom] & PEER_FLAG_READY))
			*allReadyPtr = PEERFalse;
	}
	
	GSI_UNUSED(peer);
	GSI_UNUSED(roomType);
	GSI_UNUSED(index);
}

PEERBool peerAreAllReady
(
	PEER peer
)
{
	PEERBool allReady;

	PEER_CONNECTION;
	PEER_CONNECTED;

	// Check for a title.
	/////////////////////
	if(!connection->title[0])
		return PEERFalse;

	// Check for a connection.
	//////////////////////////
	if(!connection->connected)
		return PEERFalse;

	// Are we in a staging room?
	////////////////////////////
	assert(connection->inRoom[StagingRoom]);
	if(!connection->inRoom[StagingRoom])
		return PEERFalse;

	// Enum through all the room's players.
	///////////////////////////////////////
	allReady = PEERTrue;
	piEnumRoomPlayers(peer, StagingRoom, piAreAllReadyEnumRoomPlayersCallback, &allReady);

	return allReady;
}

void peerStartGame
(
	PEER peer,
	const char * message,
	int reportingOptions
)
{
	PEER_CONNECTION;
	PEER_CONNECTED;

	// Check for a title.
	/////////////////////
	if(!connection->title[0])
		return;

	// Check for a connection.
	//////////////////////////
	if(!connection->connected)
		return;

	// Check that we're in a staging room.
	//////////////////////////////////////
	assert(connection->inRoom[StagingRoom]);
	if(!connection->inRoom[StagingRoom])
		return;

	// Make sure we're the host.
	////////////////////////////
	assert(connection->hosting);
	if(!connection->hosting)
		return;

	// Change NULL messages to empty messages.
	//////////////////////////////////////////
	if(!message)
		message = "";

	// Send the launch UTM.
	///////////////////////
	piSendChannelUTM(peer, connection->rooms[StagingRoom], PI_UTM_LAUNCH, message, PEERFalse);

#if 1
	// Send an old-style launch command.
	// THIS IS ONLY NEEDED FOR BACKWARDS COMPATIBILITY AND SHOULD BE REMOVED AT SOME POINT IN THE FUTURE.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	{
	char buffer[32];
	IN_ADDR addr;
	addr.s_addr = connection->publicIP;
	sprintf(buffer, "@@@GML %s/OLD", inet_ntoa(addr));
	peerMessageRoom(peer, StagingRoom, buffer, NormalMessage);
	}
#endif

	// We're playing.
	/////////////////
	connection->playing = PEERTrue;

	// Set the flags.
	/////////////////
	piSetLocalFlags(peer);

	// If we're AutoMatching, we're now done.
	/////////////////////////////////////////
	if(peerIsAutoMatching(peer))
	{
		piSetAutoMatchStatus(peer, PEERComplete);
	}
	else if(connection->queryReporting)
	{
		// Check if we should stop GOA reporting.
		/////////////////////////////////////////
		if(reportingOptions & PEER_STOP_REPORTING)
		{
			// Stop.
			////////
			piStopReporting(peer);
		}
		else
		{
			// Set the options.
			///////////////////
			connection->reportingOptions = reportingOptions;

			// Send a state-changed.
			////////////////////////
			piSendStateChanged(peer);
		}
	}
}

PEERBool peerStartReporting
(
	PEER peer
)
{
	return peerStartReportingWithSocket(peer, INVALID_SOCKET, 0);
}

PEERBool peerStartReportingWithSocket
(
	PEER peer,
	SOCKET socket,
	unsigned short port
)
{
	PEER_CONNECTION;

	// Check for a title.
	/////////////////////
	if(!connection->title[0])
		return PEERFalse;

	// Start.
	/////////
	if(!piStartReporting(peer, socket, port))
		return PEERFalse;

	return PEERTrue;
}

void peerStartPlaying
(
	PEER peer
)
{
	PEER_CONNECTION;

	// Check for a title.
	/////////////////////
	if(!connection->title[0])
		return;

	// Mark us as playing.
	//////////////////////
	connection->playing = PEERTrue;

	// Set the flags.
	/////////////////
	piSetLocalFlags(peer);
}

PEERBool peerIsPlaying
(
	PEER peer
)
{
	PEER_CONNECTION;
	PEER_CONNECTED;

	// Check for a title.
	/////////////////////
	if(!connection->title[0])
		return PEERFalse;

	// Check for a connection.
	//////////////////////////
	if(!connection->connected)
		return PEERFalse;

	return connection->playing;
}

void peerStopGame
(
	PEER peer
)
{
	PEER_CONNECTION;

	// We're done playing.
	//////////////////////
	connection->playing = PEERFalse;

	// Set the flags.
	/////////////////
	piSetLocalFlags(peer);

	// Are we reporting?
	////////////////////
	if(connection->queryReporting)
	{
		// Are we still in the staging room?
		////////////////////////////////////
		if(connection->inRoom[StagingRoom])
			piSendStateChanged(peer);
		else
			piStopReporting(peer);
	}
}

void peerStateChanged
(
	PEER peer
)
{
	PEER_CONNECTION;

	// We should be reporting.
	//////////////////////////
	assert(connection->queryReporting);

	// Send a state-changed.
	////////////////////////
	piSendStateChanged(peer);
}

void piSendChannelUTM
(
	PEER peer,
	const char * channel,
	const char * command,
	const char * parameters,
	PEERBool authenticate
)
{
	char buffer[512];

	PEER_CONNECTION;
	PEER_CONNECTED;

	assert(channel && channel[0]);
	assert(command && command[0]);
	assert(parameters);

	// Check for connection succeeded.
	//////////////////////////////////
	if(!connection->connected)
		return;

	// Check for no channel.
	////////////////////////
	if(!channel || !channel[0])
		return;

	// Check for no command.
	////////////////////////
	if(!command || !command[0])
		return;

	// Check for no parameters.
	///////////////////////////
	if(!parameters)
		parameters = "";

	// Make sure this UTM isn't too long.
	/////////////////////////////////////
	if((strlen(command) + strlen(parameters) + 5) > sizeof(buffer))
		return;

	// Form the message.
	////////////////////
	sprintf(buffer, "%s %s", command, parameters);

	// Send it.
	///////////
	chatSendChannelMessage(connection->chat, channel, buffer, authenticate?CHAT_ATM:CHAT_UTM);
}

void piSendPlayerUTM
(
	PEER peer,
	const char * nick,
	const char * command,
	const char * parameters,
	PEERBool authenticate
)
{
	char buffer[512];

	PEER_CONNECTION;
	PEER_CONNECTED;

	assert(nick && nick[0]);
	assert(command && command[0]);
	assert(parameters);

	// Check for connection succeeded.
	//////////////////////////////////
	if(!connection->connected)
		return;

	// Check for no nick.
	/////////////////////
	if(!nick || !nick[0])
		return;

	// Check for no command.
	////////////////////////
	if(!command || !command[0])
		return;

	// Check for no parameters.
	///////////////////////////
	if(!parameters)
		parameters = "";

	// Make sure this UTM isn't too long.
	/////////////////////////////////////
	if((strlen(command) + strlen(parameters) + 5) > sizeof(buffer))
		return;

	// Form the message.
	////////////////////
	sprintf(buffer, "%s %s", command, parameters);

	// Send it.
	///////////
	chatSendUserMessage(connection->chat, nick, buffer, authenticate?CHAT_ATM:CHAT_UTM);
}

/*********
** KEYS **
*********/
void peerSetGlobalKeys
(
	PEER peer,
	int num,
	const char ** keys,
	const char ** values
)
{
	PEER_CONNECTION;
	PEER_CONNECTED;

	assert(keys);
	assert(values);
	assert(num > 0);

	// Check for connection succeeded.
	//////////////////////////////////
	if(!connection->connected)
		return;

	// Set the keys.
	////////////////
	chatSetGlobalKeys(connection->chat, num, keys, values);
}

void peerGetPlayerGlobalKeys
(
	PEER peer,
	const char * nick,
	int num,
	const char ** keys,
	peerGetGlobalKeysCallback callback,
	void * param,
	PEERBool blocking
)
{
	PEERBool success = PEERTrue;

	PI_OP_ID;
	PEER_CONNECTION;
	PEER_CONNECTED;

	// Check for connection succeeded.
	//////////////////////////////////
	if(!connection->connected)
		return;

	if(!nick || !nick[0])
		nick = connection->nick;

	assert(callback);

	// Start the operation.
	///////////////////////
	if(!piNewGetGlobalKeysOperation(peer, nick, num, keys, callback, param, opID))
		success = PEERFalse;

	// Check for failure.
	/////////////////////
	if(!success)
		piAddGetGlobalKeysCallback(peer, PEERFalse, nick, 0, NULL, NULL, callback, param, opID);

	PI_DO_BLOCKING;
}

void peerGetRoomGlobalKeys
(
	PEER peer,
	RoomType roomType,
	int num,
	const char ** keys,
	peerGetGlobalKeysCallback callback,
	void * param,
	PEERBool blocking
)
{
	PEERBool success = PEERTrue;

	PI_OP_ID;
	PEER_CONNECTION;
	PEER_CONNECTED;

	// Check for connection succeeded.
	//////////////////////////////////
	if(!connection->connected)
		return;

	ASSERT_ROOMTYPE(roomType);
	assert(callback);

	// Check that we're in or entering this room.
	/////////////////////////////////////////////
	if(!ENTERING_ROOM && !IN_ROOM)
		return;

	// Start the operation.
	///////////////////////
	if(!piNewGetGlobalKeysOperation(peer, ROOM, num, keys, callback, param, opID))
		success = PEERFalse;

	// Check for failure.
	/////////////////////
	if(!success)
		piAddGetGlobalKeysCallback(peer, PEERFalse, "", 0, NULL, NULL, callback, param, opID);

	PI_DO_BLOCKING;
}

void peerSetRoomKeys
(
	PEER peer,
	RoomType roomType,
	const char * nick,
	int num,
	const char ** keys,
	const char ** values
)
{
	PEER_CONNECTION;
	PEER_CONNECTED;

	assert(keys);
	assert(values);
	assert(num > 0);
	ASSERT_ROOMTYPE(roomType);

	// Check for connection succeeded.
	//////////////////////////////////
	if(!connection->connected)
		return;

	// Check that we're in or entering this room.
	/////////////////////////////////////////////
	if(!ENTERING_ROOM && !IN_ROOM)
		return;

	// Set the keys.
	////////////////
	chatSetChannelKeys(connection->chat, ROOM, nick, num, keys, values);
}

void peerGetRoomKeys
(
	PEER peer,
	RoomType roomType,
	const char * nick,
	int num,
	const char ** keys,
	peerGetRoomKeysCallback callback,
	void * param,
	PEERBool blocking
)
{
	PEERBool success = PEERTrue;

	PI_OP_ID;
	PEER_CONNECTION;
	PEER_CONNECTED;

	// Check for connection succeeded.
	//////////////////////////////////
	if(!connection->connected)
		return;

	ASSERT_ROOMTYPE(roomType);
	assert(callback);

	// Check that we're in or entering this room.
	/////////////////////////////////////////////
	if(!ENTERING_ROOM && !IN_ROOM)
		return;

	// Start the operation.
	///////////////////////
	if(!piNewGetRoomKeysOperation(peer, roomType, nick, num, keys, callback, param, opID))
		success = PEERFalse;

	// Check for failure.
	/////////////////////
	if(!success)
		piAddGetRoomKeysCallback(peer, PEERFalse, roomType, nick, 0, NULL, NULL, callback, param, opID);

	PI_DO_BLOCKING;
}

void peerSetGlobalWatchKeys
(
	PEER peer,
	RoomType roomType,
	int num,
	const char ** keys,
	PEERBool addKeys
)
{
	piSetGlobalWatchKeys(peer, roomType, num, keys, addKeys);
}

void peerSetRoomWatchKeys
(
	PEER peer,
	RoomType roomType,
	int num,
	const char ** keys,
	PEERBool addKeys
)
{
	piSetRoomWatchKeys(peer, roomType, num, keys, addKeys);
}

const char * peerGetGlobalWatchKey
(
	PEER peer,
	const char * nick,
	const char * key
)
{
	return piGetGlobalWatchKey(peer, nick, key);
}

const char * peerGetRoomWatchKey
(
	PEER peer,
	RoomType roomType,
	const char * nick,
	const char * key
)
{
	return piGetRoomWatchKey(peer, roomType, nick, key);
}

void peerStartAutoMatch
(
	PEER peer,
	int maxPlayers,
	const char * filter,
	peerAutoMatchStatusCallback statusCallback,
	peerAutoMatchRateCallback rateCallback,
	void * param,
	PEERBool blocking
)
{
	peerStartAutoMatchWithSocket(peer, maxPlayers, filter, INVALID_SOCKET, 0, statusCallback, rateCallback, param, blocking);
}

void peerStartAutoMatchWithSocket
(
	PEER peer,
	int maxPlayers,
	const char * filter,
	SOCKET socket,
	unsigned short port,
	peerAutoMatchStatusCallback statusCallback,
	peerAutoMatchRateCallback rateCallback,
	void * param,
	PEERBool blocking
)
{
	PI_OP_ID;
	PEER_CONNECTION;
	PEER_CONNECTED;

	assert(maxPlayers >= 2);

	// Check the params.
	////////////////////
	if(!filter)
		filter = "";

	// Check for a title.
	/////////////////////
	if(!connection->title[0])
		goto failed;

	// Check for a connection.
	//////////////////////////
	if(!connection->connected)
		goto failed;

	// Check for an AutoMatch in progress.
	//////////////////////////////////////
	assert(!peerIsAutoMatching(peer));
	if(peerIsAutoMatching(peer))
		goto failed;

	// If entering a staging room, leave.
	/////////////////////////////////////
	if(connection->enteringRoom[StagingRoom])
		piLeaveRoom(peer, StagingRoom, "");

	// Stop any reporting.
	//////////////////////
	piStopReporting(peer);

	// Stop any game listing.
	/////////////////////////
	piSBStopListingGames(peer);

	// Store some parameters.
	/////////////////////////
	connection->maxPlayers = maxPlayers;
	connection->autoMatchFilter = goastrdup(filter);
	if(!connection->autoMatchFilter)
		goto failed;

	// Initialize the AutoMatch status.
	///////////////////////////////////
	connection->autoMatchStatus = PEERFailed;

	// Clear the SB and QR failed flags.
	////////////////////////////////////
	connection->autoMatchSBFailed = PEERFalse;
	connection->autoMatchQRFailed = PEERFalse;

	// Start the AutoMatch.
	///////////////////////
	if(!piNewAutoMatchOperation(peer, socket, port, statusCallback, rateCallback, param, opID))
	{
		gsifree(connection->autoMatchFilter);
		goto failed;
	}
	
	PI_DO_BLOCKING;

	return;

failed:
	// Failed to start the attempt.
	///////////////////////////////
	connection->autoMatchStatus = PEERFailed;
	piAddAutoMatchStatusCallback(peer);
}

void peerStopAutoMatch(PEER peer)
{
	PEER_CONNECTION;
	PEER_CONNECTED;

	// Check for a title.
	/////////////////////
	if(!connection->title[0])
		return;

	// Check for a connection.
	//////////////////////////
	if(!connection->connected)
		return;

	// Stop the AutoMatch.
	//////////////////////
	piStopAutoMatch(peer);
}

PEERBool peerIsAutoMatching(PEER peer)
{
	PEER_CONNECTION;

	// If the status is Failed or Done, then we're not matching.
	////////////////////////////////////////////////////////////
	if(connection->autoMatchStatus == PEERFailed)
		return PEERFalse;
	if(connection->autoMatchStatus == PEERComplete)
		return PEERFalse;

	return PEERTrue;
}

PEERAutoMatchStatus peerGetAutoMatchStatus(PEER peer)
{
	PEER_CONNECTION;

	return connection->autoMatchStatus;
}
