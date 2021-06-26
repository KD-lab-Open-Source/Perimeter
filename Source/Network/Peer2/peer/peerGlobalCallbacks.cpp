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
#include "peer.h"
#include "peerMain.h"
#include "peerGlobalCallbacks.h"
#include "peerRooms.h"
#include "peerPlayers.h"
#include "peerCallbacks.h"
#include "peerOperations.h"
#include "peerPing.h"
#include "peerMangle.h"
#include "peerKeys.h"
#include "peerAutoMatch.h"

#include <iostream>

/************
** DEFINES **
************/
#define PI_UTM_MATCH(utm)       (strncmp(piUTMCommand, utm, strlen(utm) - 1) == 0)

#define PI_UTM_COMMAND_LEN      8
#define PI_UTM_PARAMATERS_LEN   512

/************
** GLOBALS **
************/
static char piUTMCommand[PI_UTM_COMMAND_LEN];
static char piUTMParameters[PI_UTM_PARAMATERS_LEN];

/**************
** FUNCTIONS **
**************/

/* Chat.
*******/
static PEERBool piIsOldUTM
(
	const char * message
)
{
	// Check for no message.
	////////////////////////
	assert(message);
	if(!message)
		return PEERFalse;

	// Check for too short for prefix + 1 char.
	///////////////////////////////////////////
	if(strlen(message) < 4)
		return PEERFalse;

	// Check for no prefix.
	///////////////////////
	if((message[0] != '@') ||
		(message[1] != '@') ||
		(message[2] != '@') ||
		(message[3] == ' '))
	{
		return PEERFalse;
	}

	return PEERTrue;
}

// Returns PEERTrue if a UTM.
/////////////////////////////
static PEERBool piParseUTM
(
	const char * message
)
{
	int len;

	// Check for no message.
	////////////////////////
	assert(message);
	if(!message)
		return PEERFalse;

	// Find the end of the command.
	///////////////////////////////
	len = strcspn(message, "/ ");
	if(len >= PI_UTM_COMMAND_LEN)
		return PEERFalse;
	memcpy(piUTMCommand, message, len);
	piUTMCommand[len] = '\0';

	// Copy off the parameters.
	///////////////////////////
	message += len;
	if(message[0])
	{
		message++;
		if(strlen(message) >= PI_UTM_PARAMATERS_LEN)
			return PEERFalse;
		strcpy(piUTMParameters, message);
	}
	else
	{
		piUTMParameters[0] = '\0';
	}

	return PEERTrue;
}

static void piProcessUTM
(
	PEER peer,
	piPlayer * player,
	PEERBool inRoom,
	RoomType roomType
)
{
	char * params = piUTMParameters;

	PEER_CONNECTION;

	assert(piUTMCommand[0]);
	assert(player);

	if(PI_UTM_MATCH(PI_UTM_LAUNCH))
	{
#ifdef _DEBUG
		assert(connection->inRoom[StagingRoom]);
		if(inRoom)
			assert(roomType == StagingRoom);
		else
			assert(player->inRoom[StagingRoom]);
#endif
		if(!connection->inRoom[StagingRoom])
			return;
		if(inRoom && (roomType != StagingRoom))
			return;
		if(!inRoom && !player->inRoom[StagingRoom])
			return;

		// Ignore if we're hosting.
		///////////////////////////
		if(connection->hosting)
			return;

		// Only accept launch from ops.
		///////////////////////////////
		if(!piIsPlayerOp(player))
			return;

		// We're playing.
		/////////////////
		connection->playing = PEERTrue;

		// Set the flags.
		/////////////////
		piSetLocalFlags(peer);

		// Add the callback.
		////////////////////
		piAddGameStartedCallback(peer, connection->hostServer, params);

		// If we're AutoMatching, we're now done.
		/////////////////////////////////////////
		if(peerIsAutoMatching(peer))
			piSetAutoMatchStatus(peer, PEERComplete);
	}
	else if(PI_UTM_MATCH(PI_UTM_XPING))
	{
		piPlayer * other;
		int ping;
		unsigned int IP;

#ifdef _DEBUG
//		if(inRoom)
//			assert(connection->xpingRoom[roomType]);
#endif
		if(inRoom && !connection->xpingRoom[roomType])
			return;

		// Check for no params.
		///////////////////////
		if(!params[0])
			return;

		// Get the IP.
		//////////////
		IP = piDemangleIP(params);

		// Get the ping.
		////////////////
		params += 11;
		ping = atoi(params);

		// Figure out who this ping is to.
		//////////////////////////////////
		other = piFindPlayerByIP(peer, IP);
		if(!other)
			return;
		if(strcasecmp(player->nick, other->nick) == 0)
			return;
		if(inRoom && !player->inRoom[roomType])
			return;
		if(!inRoom)
		{
			int i;
			PEERBool success = PEERFalse;

			// Check that the three of us are in a room with xping enabled.
			///////////////////////////////////////////////////////////////
			for(i = 0 ; i < NumRooms ; i++)
			{
				if(connection->xpingRoom[i] && connection->inRoom[i] && player->inRoom[i] && other->inRoom[i])
					success = PEERTrue;
			}
			if(!success)
				return;
		}

		// Update.
		//////////
		piUpdateXping(peer, player->nick, other->nick, ping);

		// Add a xping callback.
		////////////////////////
		piAddCrossPingCallback(peer, player->nick, other->nick, ping);
	}
}

void piChatDisconnected
(
	CHAT chat,
	const char * reason,
	PEER peer
)
{
	PEER_CONNECTION;

	// We're disconnected.
	//////////////////////
	connection->disconnect = PEERTrue;

	// Add the disconnected callback.
	/////////////////////////////////
	piAddDisconnectedCallback(peer, reason);
	
	GSI_UNUSED(chat);
}

static void piHandleOldNFO
(
	PEER peer,
	piPlayer * player,
	const char * message
)
{
	// Ignore old NFOs from new clients.
	////////////////////////////////////
	if(strncmp(message + strlen(message) - 2, "X\\", 2) != 0)
	{
		const char * str;

		if(!player->inRoom[StagingRoom])
			return;

		str = strstr(message, "\\$flags$\\");
		if(str)
		{
			PEERBool ready = PEERFalse;
			str += 9;
			while(*str && (*str != '\\'))
			{
				if(*str++ == 'r')
				{
					ready = PEERTrue;
					break;
				}
			}

			if(ready)
				player->flags[StagingRoom] |= PEER_FLAG_READY;
			else
				player->flags[StagingRoom] &= ~PEER_FLAG_READY;
		}
	}
	
	GSI_UNUSED(peer);
}

void piChatPrivateMessage
(
	CHAT chat,
	const char * user,
	const char * message,
	int type,
	PEER peer
)
{
	assert(message);

	if(!user || !user[0])
		return;

	// Check for old-style UTMs.
	////////////////////////////
	if(piIsOldUTM(message))
	{
		// Check for ready.
		///////////////////
		if(strncasecmp(message, "@@@NFO", 6) == 0)
		{
			piPlayer * player;

			player = piGetPlayer(peer, user);
			if(player)
				piHandleOldNFO(peer, player, message);
		}

		return;
	}

	// Check if it's a UTM.
	///////////////////////
	if((type == CHAT_UTM) || (type == CHAT_ATM))
	{
		if(piParseUTM(message))
		{
			piPlayer * player;

			// Get the player it's from.
			////////////////////////////
			player = piGetPlayer(peer, user);
			if(player)
			{
				// Process it.
				//////////////
				piProcessUTM(peer, player, PEERFalse, (RoomType)0);
			}

			// Pass it along.
			/////////////////
			piAddPlayerUTMCallback(peer, user, piUTMCommand, piUTMParameters, (PEERBool)(type == CHAT_ATM));
		}

		return;
	}

	// It's a regular message, deliver it.
	//////////////////////////////////////
	piAddPlayerMessageCallback(peer, user, message, (MessageType)type);
	
	GSI_UNUSED(chat);
}

static void piChannelMessage
(
	CHAT chat,
	const char * channel,
	const char * user,
	const char * message,
	int type,
	PEER peer
)
{
	piPlayer * player;
	RoomType roomType;

	PEER_CONNECTION;

	assert(message);

	// Check the room type.
	///////////////////////
	if(!piRoomToType(peer, channel, &roomType))
		return;

	// Get the player.
	//////////////////
	player = piGetPlayer(peer, user);

	// Check for old-style UTMs.
	////////////////////////////
	if(player && piIsOldUTM(message))
	{
		// Only take stuff in staging rooms.
		////////////////////////////////////
		if(roomType != StagingRoom)
			return;

		// Check for a launch.
		//////////////////////
		if(strncasecmp(message, "@@@GML", 6) == 0)
		{
			// Ignore old launches from new clients.
			////////////////////////////////////////
			if(strncmp(message + strlen(message) - 4, "/OLD", 4) == 0)
				return;

			// Convert this into its modern equivalent.
			///////////////////////////////////////////
			type = CHAT_UTM;
			message = "GML";
		}
		// Check for ready.
		///////////////////
		else if(strncasecmp(message, "@@@NFO", 6) == 0)
		{
			piHandleOldNFO(peer, player, message);

			return;
		}
		else
		{
			return;
		}
	}

	// Check if it's a UTM.
	///////////////////////
	if((type == CHAT_UTM) || (type == CHAT_ATM))
	{
		if(piParseUTM(message))
		{
			// Process it.
			//////////////
			if(player)
				piProcessUTM(peer, player, PEERTrue, roomType);

			// Pass it along.
			/////////////////
			piAddRoomUTMCallback(peer, roomType, user, piUTMCommand, piUTMParameters, (PEERBool)(type == CHAT_ATM));
		}

		return;
	}

	// Add the callback.
	////////////////////
	piAddRoomMessageCallback(peer, roomType, user, message, (MessageType)type);
	
	GSI_UNUSED(chat);
}

static void piChannelKicked
(
	CHAT chat,
	const char * channel,
	const char * user,
	const char * reason,
	PEER peer
)
{
	RoomType roomType;

	PEER_CONNECTION;

	// Figure out the room type.
	////////////////////////////
	if(!piRoomToType(peer, channel, &roomType))
		return;

	// Leave the room.
	//////////////////
	piLeaveRoom(peer, roomType, NULL);

	// Add the callback.
	////////////////////
	piAddKickedCallback(peer, roomType, user, reason);
	
	// If we were kicked from an AutoMatch room, start searching.
	/////////////////////////////////////////////////////////////
	if((roomType == StagingRoom) && peerIsAutoMatching(peer))
		piSetAutoMatchStatus(peer, PEERSearching);
		
	GSI_UNUSED(chat);
}

static void piChannelUserJoined
(
	CHAT chat,
	const char * channel,
	const char * user,
	int mode,
	PEER peer
)
{
	RoomType roomType;
	piPlayer * player;

	PEER_CONNECTION;

	// Figure out the room type.
	////////////////////////////
	if(!piRoomToType(peer, channel, &roomType))
		return;

	// Add this player to the room.
	///////////////////////////////
	player = piPlayerJoinedRoom(peer, user, roomType, mode);
	if(!player)
		return;

	// Get IP and profile ID if we don't already have it.
	/////////////////////////////////////////////////////
	if(!player->gotIPAndProfileID)
	{
		const char * info;
		unsigned int IP;
		int profileID;

		if(chatGetBasicUserInfoNoWait(connection->chat, user, &info, NULL) && piDemangleUser(info, &IP, &profileID))
		{
			piSetPlayerIPAndProfileID(peer, user, IP, profileID);
		}
	}

	// Refresh this player's watch keys.
	////////////////////////////////////
	piKeyCacheRefreshPlayer(peer, roomType, user);

	// Add the callback.
	////////////////////
	piAddPlayerJoinedCallback(peer, roomType, user);

#if 1
	// If this is the staging room, send our ready state.
	/////////////////////////////////////////////////////
	if((roomType == StagingRoom) && connection->ready)
		peerMessagePlayer(peer, user, "@@@NFO \\$flags$\\rX\\", NormalMessage);
#endif

	// Check if this is an AutoMatch room.
	//////////////////////////////////////
	if((roomType == StagingRoom) && peerIsAutoMatching(peer))
	{
		// If we are Waiting, we're now Staging.
		////////////////////////////////////////
		if(connection->autoMatchStatus == PEERWaiting)
			piSetAutoMatchStatus(peer, PEERStaging);

		// If we've got maxplayers, we're now Ready.
		////////////////////////////////////////////
		if((connection->autoMatchStatus == PEERStaging) && (connection->numPlayers[StagingRoom] >= connection->maxPlayers))
			piSetAutoMatchStatus(peer, PEERReady);
	}
	
	GSI_UNUSED(chat);
}

static void piChannelUserParted
(
	CHAT chat,
	const char * channel,
	const char * user,
	int why,
	const char * reason,
	const char * kicker,
	PEER peer
)
{
	RoomType roomType;
	PEERAutoMatchStatus status;
	PEERBool newStatus = PEERFalse;
	PEERBool autoMatchRoom;

	PEER_CONNECTION;

	// Figure out the room type.
	////////////////////////////
	if(!piRoomToType(peer, channel, &roomType))
		return;

	// Check if this is an AutoMatch room.
	//////////////////////////////////////
	autoMatchRoom = ((roomType == StagingRoom) && peerIsAutoMatching(peer))?PEERTrue:PEERFalse;
	if(autoMatchRoom)
	{
		piPlayer * player;

		// Get the players.
		///////////////////
		player = piGetPlayer(peer, user);

		// Check if he was the last op.
		///////////////////////////////
		if(!connection->hosting && !piCountRoomOps(peer, StagingRoom, connection->nick))
		{
			status = PEERSearching;
			newStatus = PEERTrue;
		}
		// Check for host and everyone left.
		////////////////////////////////////
		else if(connection->hosting && connection->numPlayers[StagingRoom] == 2)
		{
			status = PEERWaiting;
			newStatus = PEERTrue;
		}
		// Check for no longer at maxplayers.
		/////////////////////////////////////
		else if(connection->numPlayers[StagingRoom] == connection->maxPlayers)
		{
			status = PEERStaging;
			newStatus = PEERTrue;
		}
	}

	// Remove this player from the room.
	////////////////////////////////////
	piPlayerLeftRoom(peer, user, roomType);

	// Figure out the reason.
	/////////////////////////
	if((why == CHAT_KICKED) || (why == CHAT_KILLED))
		reason = "Kicked";
	else if(!reason)
		reason = "";

	// Add the callback.
	////////////////////
	piAddPlayerLeftCallback(peer, roomType, user, reason);

	// Set status if needed.
	////////////////////////
	if(newStatus)
		piSetAutoMatchStatus(peer, status);

	GSI_UNUSED(chat);
	GSI_UNUSED(kicker);
}

static void piChannelUserChangedNick
(
	CHAT chat,
	const char * channel,
	const char * oldNick,
	const char * newNick,
	PEER peer
)
{
	RoomType roomType;

	// Figure out the room type.
	////////////////////////////
	if(!piRoomToType(peer, channel, &roomType))
		return;

	// Change the nick locally.
	///////////////////////////
	piPlayerChangedNick(peer, oldNick, newNick);

	// Add the callback.
	////////////////////
	piAddPlayerChangedNickCallback(peer, roomType, oldNick, newNick);
	
	GSI_UNUSED(chat);
}

static void piChannelTopicChanged
(
	CHAT chat,
	const char * channel,
	const char * topic,
	PEER peer
)
{
	RoomType roomType;

	PEER_CONNECTION;

	// Figure out the room type.
	////////////////////////////
	if(!piRoomToType(peer, channel, &roomType))
		return;
	
	// Don't allow blank names.
	///////////////////////////
	if(!topic[0])
		return;

	// Is it the same as the old name?
	//////////////////////////////////
	//if(strcmp(NAME, topic) == 0)
	//	return;

	// Copy the new name.
	/////////////////////
	strzcpy(NAME, topic, PI_NAME_MAX_LEN);

	// Add a callback.
	//////////////////
	if(IN_ROOM)
		piAddRoomNameChangedCallback(peer, roomType);
		
	GSI_UNUSED(chat);
}

static void piChannelNewUserList
(
	CHAT chat,
	const char * channel,
	int num,
	const char ** users,
	int * modes,
	PEER peer
)
{
	int i;
	RoomType roomType;

	PEER_CONNECTION;

	// Figure out the room type.
	////////////////////////////
	if(!piRoomToType(peer, channel, &roomType))
		return;

	// Clear all the players out of this room.
	//////////////////////////////////////////
	piClearRoomPlayers(peer, roomType);

	// Add all the new ones.
	////////////////////////
	for(i = 0 ; i < num ; i++)
		piPlayerJoinedRoom(peer, users[i], roomType, modes[i]);

	// Refresh keys.
	////////////////
	piKeyCacheRefreshRoom(peer, roomType);

	// Call the callback.
	/////////////////////
	piAddNewPlayerListCallback(peer, roomType);	
	
	GSI_UNUSED(chat);
}

static void piBroadcastKeyChanged
(
	CHAT chat,
	const char * channel,
	const char * user,
	const char * key,
	const char * value,
	PEER peer
)
{
	RoomType roomType;

	PEER_CONNECTION;

	// Figure out the room type.
	////////////////////////////
	if(!piRoomToType(peer, channel, &roomType))
		return;

	// Update the watch keys.
	/////////////////////////
	piRoomKeyChanged(peer, roomType, user, key, value);

	// Add the callback.
	////////////////////
	piAddRoomKeyChangedCallback(peer, roomType, user, key, value);
	
	GSI_UNUSED(chat);
}

static void piUserModeChanged
(
	CHAT chat,
	const char * channel,
	const char * user,
	int mode,
	PEER peer
)
{
	RoomType roomType;

	PEER_CONNECTION;

	// Figure out the room type.
	////////////////////////////
	if(!piRoomToType(peer, channel, &roomType))
		return;

	// Set the user mode.
	/////////////////////
	piSetPlayerModeFlags(peer, user, roomType, mode);
	
	GSI_UNUSED(chat);
}

static void piChannelModeChanged
(
	CHAT chat,
	const char * channel,
	CHATChannelMode * mode,
	PEER peer
)
{
	RoomType roomType;

	PEER_CONNECTION;

	// Figure out the room type.
	////////////////////////////
	if(!piRoomToType(peer, channel, &roomType))
		return;

	// Add the callback.
	////////////////////
	piAddRoomModeChangedCallback(peer, roomType, mode);
	
	GSI_UNUSED(chat);
}

void piSetChannelCallbacks
(
	PEER peer,
	chatChannelCallbacks * channelCallbacks
)
{
	memset(channelCallbacks, 0, sizeof(chatChannelCallbacks));
	channelCallbacks->param = peer;
	channelCallbacks->channelMessage = piChannelMessage;
	channelCallbacks->kicked = piChannelKicked;
	channelCallbacks->userJoined = piChannelUserJoined;
	channelCallbacks->userParted = piChannelUserParted;
	channelCallbacks->userChangedNick = piChannelUserChangedNick;
	channelCallbacks->topicChanged = piChannelTopicChanged;
	channelCallbacks->newUserList = piChannelNewUserList;
	channelCallbacks->broadcastKeyChanged = piBroadcastKeyChanged;
	channelCallbacks->userModeChanged = piUserModeChanged;
	channelCallbacks->channelModeChanged = piChannelModeChanged;
}

