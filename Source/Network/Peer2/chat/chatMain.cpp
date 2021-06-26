/*
GameSpy Chat SDK 
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
#include <stdio.h>
#include <stdlib.h>
#include "chat.h"
#include "chatMain.h"
#include "chatSocket.h"
#include "chatHandlers.h"
#include "chatChannel.h"
#include "chatCallbacks.h"
#if defined(applec) || defined(THINK_C) || defined(__MWERKS__) && !defined(__mips64) && !defined(_WIN32)
	#include "::nonport.h"
#else
	#include "../nonport.h"
#endif

#include <iostream>

/************
** DEFINES **
************/
#define CI_DO_BLOCKING      if(blocking)\
							{\
								do{\
									ciThink(chat, ID);\
									msleep(10);\
								}while(ciCheckForID(chat, ID));\
							}

#define ASSERT_CHANNEL()    assert(channel != NULL); assert(channel[0] != '\0');
#define ASSERT_NICK()       assert(nick != NULL); assert(nick[0] != '\0'); assert(strlen(nick) < MAX_NICK);
#define ASSERT_USER(user)   assert(user != NULL); assert(user[0] != '\0'); assert(strlen(user) < MAX_USER);
#define ASSERT_MESSAGE()    assert(message != NULL); assert(message[0] != '\0');
#define ASSERT_TYPE(type)   assert((type == CHAT_MESSAGE) || (type == CHAT_ACTION) || (type == CHAT_NOTICE) || (type == CHAT_UTM) || (type == CHAT_ATM));
#define ASSERT_PASSWORD()   assert(password != NULL); assert(password[0] != '\0');
#define ASSERT_BAN()        assert(ban != NULL); assert(ban [0] != '\0');

/**********
** TYPES **
**********/

typedef struct ciEnumUsersData
{
	chatEnumUsersCallback callback;
	void * param;
} ciEnumUsersData;

/**************
** FUNCTIONS **
**************/
static CHATBool ciProcessServerMessage(CHAT chat, ciServerMessage * message)
{
	int i;

	assert(message != NULL);

	// Figure out what type of message this is.
	///////////////////////////////////////////
	for(i = 0 ; i < numServerMessageTypes ; i++)
	{
		// Does the type match?
		///////////////////////
		if(strcasecmp(message->command, serverMessageTypes[i].command) == 0)
		{
			// Is there a handler?
			//////////////////////
			if(serverMessageTypes[i].handler != NULL)
			{
				// Call the handler.
				////////////////////
				serverMessageTypes[i].handler(chat, message);
			}

			return CHATTrue;
		}
	}

	// Didn't find a match.
	///////////////////////
	return CHATFalse;  //ERRCON
}

static CHATBool ciCheckForID(CHAT chat, int ID)
{
	return (CHATBool)(ciCheckFiltersForID(chat, ID) || ciCheckCallbacksForID(chat, ID));
}

void ciHandleDisconnect(CHAT chat, const char * reason)
{
	CHATBool connecting;
	CONNECTION;

	// Check if we've already handled this.
	///////////////////////////////////////
	if(connection->disconnected)
		return;

	// Keep track of if we are trying to connect.
	/////////////////////////////////////////////
	connecting = connection->connecting;

	// Not connected anymore.
	/////////////////////////
	connection->connected = CHATFalse;
	connection->connecting = CHATFalse;
	connection->disconnected = CHATTrue;

	// If we're still connecting, let the app know the attempt failed.
	//////////////////////////////////////////////////////////////////
	if(connection->connecting)
	{
		// Call the callback.
		/////////////////////
		if(connection->connectCallback != NULL)
			connection->connectCallback(chat, CHATFalse, connection->connectParam);
	}
	// Otherwise call the global callback.
	//////////////////////////////////////
	else if(connection->globalCallbacks.disconnected != NULL)
	{
		ciCallbackDisconnectedParams params;
		params.reason = (char *)reason;
		ciAddCallback_wrapper(chat, CALLBACK_DISCONNECTED, connection->globalCallbacks.disconnected, &params, connection->globalCallbacks.param, 0, NULL);
	}
}

static void ciThink(CHAT chat, int ID)
{
	ciServerMessage * message;
	CONNECTION;

	// Is the socket connected?
	///////////////////////////
	if(connection->chatSocket.connectState == ciConnected)
	{
		// Do processing.
		/////////////////
		ciSocketThink(&connection->chatSocket);

		// Check received messages.
		///////////////////////////
		while((message = ciSocketRecv(&connection->chatSocket)) != NULL)
		{
			// Call the raw callback.
			/////////////////////////
			if(connection->globalCallbacks.raw != NULL)
			{
				ciCallbackRawParams params;
				params.raw = message->message;
				ciAddCallback_wrapper(chat, CALLBACK_RAW, connection->globalCallbacks.raw, &params, connection->globalCallbacks.param, 0, NULL);
			}

			// Process the message.
			///////////////////////
			ciProcessServerMessage(chat, message);
		}

		// Have we lost connection?
		///////////////////////////
		if(connection->chatSocket.connectState == ciDisconnected)
		{
			ciHandleDisconnect(chat, "Disconnected");
		}
	}

	// Let the filters think.
	/////////////////////////
	ciFilterThink(chat);

	// Call callbacks.
	//////////////////
	ciCallCallbacks(chat, ID);
}

/************
** GENERAL **
************/
void ciSendNickAndUser(CHAT chat)
{
	CONNECTION;

	// Send the user.
	/////////////////
	ciSocketSendf(&connection->chatSocket, "USER %s %s %s :%s",
		connection->user,
		"127.0.0.1",
		connection->server,
		connection->name);

	// Check for an invalid nick.
	/////////////////////////////
	if (CHATFalse == ciNickIsValid(connection->nick))
	{
		ciNickError(chat, CHAT_INVALID);
		return;
	}

	// Send the nick.
	/////////////////
	ciSocketSendf(&connection->chatSocket, "NICK %s", connection->nick);
}

static CHAT chatConnectDoit(const char * serverAddress,
				 int port,
                 const char * nick,
				 const char * user,
				 const char * name,
				 const char * gamename,
				 const char * secretKey,
				 chatGlobalCallbacks * callbacks,
				 chatNickErrorCallback nickErrorCallback,
				 chatFillInUserCallback fillInUserCallback,
                 chatConnectCallback connectCallback,
                 void * param,
                 CHATBool blocking)
{
	ciConnection * connection;

	assert(serverAddress != NULL);
	ASSERT_NICK();
	assert(callbacks != NULL);
	assert(connectCallback != NULL);

	// Init sockets.
	////////////////
	SocketStartUp();

	// Create a connection object.
	//////////////////////////////
	connection = (ciConnection *)gsimalloc(sizeof(ciConnection));
	if(connection == NULL)
		return NULL;  //ERRCON

	// Initialize the connection.
	/////////////////////////////
	memset(connection, 0, sizeof(ciConnection));
	strcpy(connection->nick, nick);
	if(user)
		strcpy(connection->user, user);
	strcpy(connection->name, name);
	strcpy(connection->server, serverAddress);
	connection->port = port;
	connection->globalCallbacks = *callbacks;
/*	if(gethostname(connection->host, MAX_HOST) == SOCKET_ERROR)
		strcpy(connection->host, "<unknown>"); //ERRCON*/
	connection->nextID = 1;
	connection->connecting = CHATTrue;
	connection->quiet = CHATFalse;

	// Initialize the channel table.
	////////////////////////////////
	if(!ciInitChannels(connection))
	{
		gsifree(connection);
		SocketShutDown();
		return NULL; //ERRCON
	}

	// Initialize the callbacks list.
	/////////////////////////////////
	if(!ciInitCallbacks(connection))
	{
		ciCleanupChannels((CHAT)connection);
		gsifree(connection);
		SocketShutDown();
		return NULL; //ERRCON
	}

	// Initialize the socket.
	/////////////////////////
	if(!ciSocketInit(&connection->chatSocket, nick))
	{
		ciCleanupCallbacks((CHAT)connection);
		ciCleanupChannels((CHAT)connection);
		gsifree(connection);
		SocketShutDown();
		return NULL; //ERRCON
	}

	// Connect the socket.
	//////////////////////
	if(!ciSocketConnect(&connection->chatSocket, serverAddress, port))
	{
		ciSocketDisconnect(&connection->chatSocket);
		ciCleanupCallbacks((CHAT)connection);
		ciCleanupChannels((CHAT)connection);
		gsifree(connection);
		SocketShutDown();
		return NULL; //ERRCON
	}

	// Special stuff for MS Chat server.
	////////////////////////////////////
	//ciSocketSend(&connection->chatSocket, "MODE ISIRCX");
	//ciSocketSend(&connection->chatSocket, "IRCX");

	// Set the callback info.
	/////////////////////////
	connection->nickErrorCallback = nickErrorCallback;
	connection->fillInUserCallback = fillInUserCallback;
	connection->connectCallback = connectCallback;
	connection->connectParam = param;

	// Check for a secure connection.
	/////////////////////////////////
	if(gamename && gamename[0] && secretKey && secretKey[0])
	{
		// Save the game secret key.
		////////////////////////////
		strncpy(connection->secretKey, secretKey, MAX_SECRETKEY);

		// Get the random keys.
		///////////////////////
		ciSocketSendf(&connection->chatSocket, "CRYPT des 1 %s", gamename);
	}
	else if(connection->fillInUserCallback)
	{
		// Get the IP.
		//////////////
		ciSocketSend(&connection->chatSocket, "USRIP");
	}
	else
	{
		ciSendNickAndUser((CHAT)connection);
	}

	// Do blocking.
	///////////////
	if(blocking)
	{
		// While we're connecting.
		//////////////////////////
		do
		{
			ciThink((CHAT)connection, 0);
			msleep(10);
		} while(connection->connecting);

		// Check if the connect failed.
		///////////////////////////////
		if(!connection->connected)
		{
			// Disconnect the connection.
			/////////////////////////////
			chatDisconnect((CHAT)connection);
			connection = NULL;
		}
	}

	return (CHAT)connection;
}

CHAT chatConnect(const char * serverAddress,
				 int port,
                 const char * nick,
				 const char * user,
				 const char * name,
				 chatGlobalCallbacks * callbacks,
				 chatNickErrorCallback nickErrorCallback,
                 chatConnectCallback connectCallback,
                 void * param,
                 CHATBool blocking)
{
	return chatConnectDoit(serverAddress,
		port,
		nick,
		user,
		name,
		NULL,
		NULL,
		callbacks,
		nickErrorCallback,
		NULL,
		connectCallback,
		param,
		blocking);
}

CHAT chatConnectSpecial(const char * serverAddress,
				 int port,
                 const char * nick,
				 const char * name,
				 chatGlobalCallbacks * callbacks,
				 chatNickErrorCallback nickErrorCallback,
				 chatFillInUserCallback fillInUserCallback,
                 chatConnectCallback connectCallback,
                 void * param,
                 CHATBool blocking)
{
	return chatConnectDoit(serverAddress,
		port,
		nick,
		NULL,
		name,
		NULL,
		NULL,
		callbacks,
		nickErrorCallback,
		fillInUserCallback,
		connectCallback,
		param,
		blocking);
}

CHAT chatConnectSecure(const char * serverAddress,
				 int port,
                 const char * nick,
				 const char * name,
				 const char * gamename,
				 const char * secretKey,
				 chatGlobalCallbacks * callbacks,
				 chatNickErrorCallback nickErrorCallback,
				 chatFillInUserCallback fillInUserCallback,
                 chatConnectCallback connectCallback,
                 void * param,
                 CHATBool blocking)
{
	return chatConnectDoit(serverAddress,
		port,
		nick,
		NULL,
		name,
		gamename,
		secretKey,
		callbacks,
		nickErrorCallback,
		fillInUserCallback,
		connectCallback,
		param,
		blocking);
}

void chatRetryWithNick(CHAT chat,
					   const char * nick)
{
	CONNECTION;

	// Are we already connected?
	////////////////////////////
	if(connection->connected)
		return;

	// Copy the new nick.
	/////////////////////
	strcpy(connection->nick, nick);

	// Check for a bad nick.
	////////////////////////
	if (CHATFalse == ciNickIsValid(nick))
	{
		ciNickError(chat, CHAT_INVALID);
		return;
	}

	// Send the new nick.
	/////////////////////
	ciSocketSendf(&connection->chatSocket, "NICK :%s", nick);

}

void chatDisconnect(CHAT chat)
{
	CONNECTION;

	// Cleanup all the filters first.
	/////////////////////////////////
	ciCleanupFilters(chat);

	// Call the disconnected callback if we haven't already.
	////////////////////////////////////////////////////////
	if(!connection->disconnected && connection->globalCallbacks.disconnected)
		connection->globalCallbacks.disconnected(chat, "", connection->globalCallbacks.param);

	// Are we connected.
	////////////////////
	if(connection->connected)
	{
		ciSocketSend(&connection->chatSocket, "QUIT :Later!");
		ciSocketThink(&connection->chatSocket);
	}

	// gsifree the channel table.
	//////////////////////////
	ciCleanupChannels(chat);

	// Cleanup the callbacks list.
	//////////////////////////////
	ciCleanupCallbacks(chat);

	// Shutdown the chat socket.
	////////////////////////////
	ciSocketDisconnect(&connection->chatSocket);

	// gsifree the memory.
	///////////////////
	gsifree(chat);

	// Shutdown sockets.
	////////////////////
	SocketShutDown();
}

void chatThink(CHAT chat)
{
	ciThink(chat, 0);
}

void chatSendRaw(CHAT chat,
				 const char * command)
{
	CONNECTION;
	CONNECTED;

	ciSocketSend(&connection->chatSocket, command);
}

void chatChangeNick(CHAT chat,
					const char * newNick,
					chatChangeNickCallback callback,
					void * param,
					CHATBool blocking)
{
	int ID;
	CHATBool success = CHATTrue;

	CONNECTION;
	CONNECTED;

	assert(newNick);
	assert(newNick[0]);
	assert(strlen(newNick) < MAX_NICK);
	assert(callback);
	assert(connection->connected);

	// chatRetryWithNick should be called while connecting.
	///////////////////////////////////////////////////////
	if(!connection->connected)
		return;

	// No nick.
	///////////
	if(!newNick || !newNick[0] || (strlen(newNick) >= MAX_NICK))
		success = CHATFalse;

	// Check for same nick.
	///////////////////////
	if(success && (strcasecmp(newNick, connection->nick) == 0))
		success = CHATFalse;

	// Call the callback?
	/////////////////////
	if(!success)
	{
		if(callback)
		{
			ciCallbackChangeNickParams params;
			params.success = success;
			params.oldNick = connection->nick;
			params.newNick = (char *)newNick;
			ID = ciGetNextID(chat);
			ciAddCallback_wrapper(chat, CALLBACK_CHANGE_NICK, callback, &params, param, ID, NULL);

			CI_DO_BLOCKING;
		}

		return;
	}

	// Send the request.
	////////////////////
	ciSocketSendf(&connection->chatSocket, "NICK :%s", newNick);

	ID = ciAddNICKFilter(chat, connection->nick, newNick, callback, param);

	CI_DO_BLOCKING;
}

char * chatGetNick(CHAT chat)
{
	CONNECTION;

	if(!connection->connected) 
		return "";

	return connection->nick;
}

void chatFixNick(char * newNick,
				 const char * oldNick)
{
	int c;

	assert(oldNick);
	assert(newNick);

	if(isdigit(*oldNick) || (*oldNick == '-'))
		*newNick++ = '_';

	while((c = *oldNick++) != '\0')
	{
		if(!strchr(VALID_NICK_CHARS, c))
			c = '_';

		*newNick++ = c;
	}
	*newNick = '\0';
}

static void ciSetQuietModeEnumJoinedChannels(CHAT chat,
											 int index,
											 const char * channel,
											 void * param)
{
	// Setup a filter.
	//////////////////
	ciAddUNQUIETFilter(chat, channel);
	
	GSI_UNUSED(index);
	GSI_UNUSED(param);
}

void chatSetQuietMode(CHAT chat,
					  CHATBool quiet)
{
	CONNECTION;
	CONNECTED;

	// Check if its the current mode.
	/////////////////////////////////
	if(connection->quiet == quiet)
		return;

	// Send the message.
	////////////////////
	if(quiet)
		ciSocketSendf(&connection->chatSocket, "MODE %s +q", connection->nick);
	else
		ciSocketSendf(&connection->chatSocket, "MODE %s -q", connection->nick);

	// Set the mode.
	////////////////
	connection->quiet = quiet;

	// Are we disabling it?
	///////////////////////
	if(!quiet)
	{
		// Clear all the player lists.
		//////////////////////////////
		ciClearAllUsers(chat);

		// Setup a filter for each joined channel.
		//////////////////////////////////////////
		ciEnumJoinedChannels(chat, ciSetQuietModeEnumJoinedChannels, NULL);
	}
}

void chatAuthenticateCDKey(CHAT chat,
						   const char * cdkey,
						   chatAuthenticateCDKeyCallback callback,
						   void * param,
						   CHATBool blocking)
{
	int ID;
	CHATBool success = CHATTrue;

	CONNECTION;
	CONNECTED;

	assert(cdkey);
	assert(cdkey[0]);
	assert(callback);
	assert(connection->connected);

	// Check we're connected.
	/////////////////////////
	if(!connection->connected)
		return;

	// No key.
	//////////
	if(!cdkey || !cdkey[0])
		success = CHATFalse;

	// Call the callback?
	/////////////////////
	if(!success)
	{
		if(callback)
		{
			ciCallbackAuthenticateCDKeyParams params;
			params.result = 0;
			params.message = "";
			ID = ciGetNextID(chat);
			ciAddCallback_wrapper(chat, CALLBACK_AUTHENTICATE_CDKEY, callback, &params, param, ID, NULL);

			CI_DO_BLOCKING;
		}

		return;
	}

	// Send the request.
	////////////////////
	ciSocketSendf(&connection->chatSocket, "CDKEY %s", cdkey);

	ID = ciAddCDKEYFilter(chat, callback, param);

	CI_DO_BLOCKING;
}

/*************
** CHANNELS **
*************/
void chatEnumChannels(CHAT chat,
					  const char * filter,
					  chatEnumChannelsCallbackEach callbackEach,
					  chatEnumChannelsCallbackAll callbackAll,
					  void * param,
					  CHATBool blocking)
{
	int ID;
	CONNECTION;
	CONNECTED;

	assert((callbackAll != NULL) || (callbackEach != NULL));

	if(!filter)
		filter = "";

	ciSocketSendf(&connection->chatSocket, "LIST %s", filter);

	ID = ciAddLISTFilter(chat, callbackEach, callbackAll, param);

	CI_DO_BLOCKING;
}

void chatEnterChannel(CHAT chat,
					  const char * channel,
					  const char * password,
					  chatChannelCallbacks * callbacks,
					  chatEnterChannelCallback callback,
					  void * param,
					  CHATBool blocking)
{
	int ID;
	CONNECTION;
	CONNECTED;

	ASSERT_CHANNEL();
	assert(callbacks != NULL);

	if(password == NULL)
		password = "";

	ciSocketSendf(&connection->chatSocket, "JOIN %s %s", channel, password);

	ID = ciAddJOINFilter(chat, channel, callback, param, callbacks, password);

	// Entering.
	////////////
	ciChannelEntering(chat, channel);

	CI_DO_BLOCKING;
}

void chatLeaveChannel(CHAT chat,
					  const char * channel,
					  const char * reason)
{
	CONNECTION;
	CONNECTED;

	ASSERT_CHANNEL();

	if(!reason)
		reason = "";

	ciSocketSendf(&connection->chatSocket, "PART %s :%s", channel, reason);

	// Left the channel.
	////////////////////
	ciChannelLeft(chat, channel);
}

void chatSendChannelMessage(CHAT chat,
							const char * channel,
							const char * message,
							int type)
{
	chatChannelCallbacks * callbacks;
	CONNECTION;
	CONNECTED;

	ASSERT_CHANNEL();
	ASSERT_TYPE(type);

	if (!message || !message[0])
		return;
	if(type == CHAT_MESSAGE)
		ciSocketSendf(&connection->chatSocket, "PRIVMSG %s :%s", channel, message);
	else if(type == CHAT_ACTION)
		ciSocketSendf(&connection->chatSocket, "PRIVMSG %s :\001ACTION %s\001", channel, message);
	else if(type == CHAT_NOTICE)
		ciSocketSendf(&connection->chatSocket, "NOTICE %s :%s", channel, message);
	else if(type == CHAT_UTM)
		ciSocketSendf(&connection->chatSocket, "UTM %s :%s", channel, message);
	else if(type == CHAT_ATM)
		ciSocketSendf(&connection->chatSocket, "ATM %s :%s", channel, message);
	else
		return;

	// We don't get these back, so call the callbacks.
	//////////////////////////////////////////////////
	callbacks = ciGetChannelCallbacks(chat, channel);
	if(callbacks != NULL)
	{
		ciCallbackChannelMessageParams params;
		params.channel = (char *)channel;
		params.user = connection->nick;
		params.message = (char *)message;
		params.type = type;
		ciAddCallback_wrapper(chat, CALLBACK_CHANNEL_MESSAGE, callbacks->channelMessage, &params, callbacks->param, 0, channel);
	}
}

void chatSetChannelTopic(CHAT chat,
						 const char * channel,
						 const char * topic)
{
	CONNECTION;
	CONNECTED;

	ASSERT_CHANNEL();

	if(topic == NULL)
		topic = "";

	ciSocketSendf(&connection->chatSocket, "TOPIC %s :%s", channel, topic);
}

void chatGetChannelTopic(CHAT chat,
						 const char * channel,
						 chatGetChannelTopicCallback callback,
						 void * param,
						 CHATBool blocking)
{
	int ID;
	const char * topic;
	CONNECTION;
	CONNECTED;

	ASSERT_CHANNEL();
	assert(callback != NULL);

	// Check if we already have the topic.
	//////////////////////////////////////
	topic = ciGetChannelTopic(chat, channel);
	if(topic)
	{
		ciCallbackGetChannelTopicParams params;

		ID = ciGetNextID(chat);

		params.success = CHATTrue;
		params.channel = (char *)channel;
		params.topic = (char *)topic;
		ciAddCallback_wrapper(chat, CALLBACK_GET_CHANNEL_TOPIC, callback, &params, param, ID, channel);
	}
	else
	{
		ciSocketSendf(&connection->chatSocket, "TOPIC %s", channel);

		ID = ciAddTOPICFilter(chat, channel, callback, param);
	}

	CI_DO_BLOCKING;
}


void chatSetChannelMode(CHAT chat,
						const char * channel,
						CHATChannelMode * mode)
{
	char buffer[64];

	CONNECTION;
	CONNECTED;

	ASSERT_CHANNEL();
	assert(mode != NULL);

	// Build the mode string.
	/////////////////////////
	strcpy(buffer, "XiXpXsXmXnXtXl");
	if(mode->InviteOnly)
		buffer[0] = '+';
	else
		buffer[0] = '-';
	if(mode->Private)
		buffer[2] = '+';
	else
		buffer[2] = '-';
	if(mode->Secret)
		buffer[4] = '+';
	else
		buffer[4] = '-';
	if(mode->Moderated)
		buffer[6] = '+';
	else
		buffer[6] = '-';
	if(mode->NoExternalMessages)
		buffer[8] = '+';
	else
		buffer[8] = '-';
	if(mode->OnlyOpsChangeTopic)
		buffer[10] = '+';
	else
		buffer[10] = '-';
	if(mode->Limit > 0)
		buffer[12] = '+';
	else
		buffer[12] = '-';

	// Add limit if needed.
	///////////////////////
	if(mode->Limit > 0)
		sprintf(&buffer[strlen(buffer)], " %d", mode->Limit);

	ciSocketSendf(&connection->chatSocket, "MODE %s %s", channel, buffer);
}

void chatGetChannelMode(CHAT chat,
						const char * channel,
						chatGetChannelModeCallback callback,
						void * param,
						CHATBool blocking)
{
	int ID;
	CONNECTION;
	CONNECTED;

	ASSERT_CHANNEL();
	assert(callback != NULL);

	// Are we in this channel?
	//////////////////////////
	if(ciInChannel(chat, channel))
	{
		CHATChannelMode mode;

		// Get the mode locally.
		////////////////////////
		if(ciGetChannelMode(chat, channel, &mode))
		{
			ciCallbackGetChannelModeParams params;

			// Get an ID.
			/////////////
			ID = ciGetNextID(chat);

			// Add the callback.
			////////////////////
			params.success = CHATTrue;
			params.channel = (char *)channel;
			params.mode = &mode;
			ciAddCallback_wrapper(chat, CALLBACK_GET_CHANNEL_MODE, callback, &params, param, ID, NULL);

			CI_DO_BLOCKING;

			return;
		}
	}

	ciSocketSendf(&connection->chatSocket, "MODE %s", channel);

	ID = ciAddCMODEFilter(chat, channel, callback, param);

	CI_DO_BLOCKING;
}

void chatSetChannelPassword(CHAT chat,
							const char * channel,
							CHATBool enable,
							const char * password)
{
	CONNECTION;
	CONNECTED;

	ASSERT_CHANNEL();
	ASSERT_PASSWORD();

	if(enable)
		ciSocketSendf(&connection->chatSocket, "MODE %s +k %s", channel, password);
	else
		ciSocketSendf(&connection->chatSocket, "MODE %s -k %s", channel, password);
}

void chatGetChannelPassword(CHAT chat,
							const char * channel,
							chatGetChannelPasswordCallback callback,
							void * param,
							CHATBool blocking)
{
	ciCallbackGetChannelPasswordParams params;
	const char * password;
	int ID;
	CONNECTION;
	CONNECTED;

	ASSERT_CHANNEL();
	assert(callback != NULL);

	// Check that we're in the channel.
	///////////////////////////////////
	if(!ciInChannel(chat, channel))
		return; //ERRCON

	// Get the password.
	////////////////////
	password = ciGetChannelPassword(chat, channel);
	assert(password != NULL);

	// Get an ID.
	/////////////
	ID = ciGetNextID(chat);

	// Add the callback.
	////////////////////
	params.success = CHATTrue;
	params.channel = (char *)channel;
	params.enabled = CHATTrue;
	params.password = (char *)password;
	ciAddCallback_wrapper(chat, CALLBACK_GET_CHANNEL_PASSWORD, callback, &params, param, ID, NULL);

	CI_DO_BLOCKING;
}

void chatSetChannelLimit(CHAT chat,
						 const char * channel,
						 int limit)
{
	CONNECTION;
	CONNECTED;

	ASSERT_CHANNEL();
	assert(limit >= 0);

	if(limit)
		ciSocketSendf(&connection->chatSocket, "MODE %s +l %d", channel, limit);
	else
		ciSocketSendf(&connection->chatSocket, "MODE %s -l", channel);
}

void chatEnumChannelBans(CHAT chat,
						 const char * channel,
						 chatEnumChannelBansCallback callback,
						 void * param,
						 CHATBool blocking)
{
	int ID;
	CONNECTION;
	CONNECTED;

	ASSERT_CHANNEL();
	assert(callback != NULL);

	ciSocketSendf(&connection->chatSocket, "MODE %s +b", channel);

	ID = ciAddGETBANFilter(chat, channel, callback, param);

	CI_DO_BLOCKING;
}

void chatAddChannelBan(CHAT chat,
					   const char * channel,
					   const char * ban)
{
	CONNECTION;
	CONNECTED;

	ASSERT_CHANNEL();
	ASSERT_BAN();

	ciSocketSendf(&connection->chatSocket, "MODE %s +b %s", channel, ban);
}

void chatRemoveChannelBan(CHAT chat,
						  const char * channel,
						  const char * ban)
{
	CONNECTION;
	CONNECTED;

	ASSERT_CHANNEL();
	ASSERT_BAN();

	ciSocketSendf(&connection->chatSocket, "MODE %s -b %s", channel, ban);
}

void chatSetChannelGroup(CHAT chat,
						 const char * channel,
						 const char * group)
{
	CONNECTION;
	CONNECTED;

	ASSERT_CHANNEL();

	// No way to clear the group.
	/////////////////////////////
	if(!group || !group[0])
		return;

	ciSocketSendf(&connection->chatSocket, "SETGROUP %s %s", channel, group);
}

int chatGetChannelNumUsers(CHAT chat,
						   const char * channel)
{
	CONNECTION;
	if(!connection->connected)
		return -1;

	ASSERT_CHANNEL();

	if(!channel || !channel[0])
		return -1;

	if(!ciInChannel(chat, channel))
		return -1;

	return ciGetChannelNumUsers(chat, channel);
}

CHATBool chatInChannel(CHAT chat,
						   const char * channel)

{
	CONNECTION;
	if(!connection->connected)
		return CHATFalse;

	ASSERT_CHANNEL();

	if(!channel || !channel[0])
		return CHATFalse;

	return ciInChannel(chat, channel);
}

/**********
** USERS **
**********/
static void ciEnumUsersCallback(CHAT chat, const char * channel, int numUsers, const char ** users, int * modes, void * param)
{
	ciEnumUsersData * data;
	CONNECTION;

	// Check the args.
	//////////////////
	ASSERT_CHANNEL();
	assert(numUsers >= 0);
#ifdef _DEBUG
	{
	int i;
	if(numUsers > 0)
	{
		assert(users != NULL);
		assert(modes != NULL);
	}
	for(i = 0 ; i < numUsers ; i++)
	{
		ASSERT_USER(users[i]);
		ASSERT_TYPE(modes[i]);
	}
	}
#endif
	assert(param != NULL);

	// Get the data.
	////////////////
	data = (ciEnumUsersData *)param;
	assert(data->callback != NULL);

	// Call the callback directly.
	//////////////////////////////
	data->callback(chat, CHATTrue, channel, numUsers, users, modes, data->param);
}

void chatEnumUsers(CHAT chat,
				   const char * channel,
				   chatEnumUsersCallback callback,
				   void * param,
				   CHATBool blocking)
{
	int ID;
	ciEnumUsersData data;
	CONNECTION;
	CONNECTED;

	//ASSERT_CHANNEL();
	assert(callback != NULL);

	if(channel == NULL)
		channel = "";

	// Is there a channel specified?
	////////////////////////////////
	if(channel[0] != '\0')
	{
		// Check if we have this one locally.
		/////////////////////////////////////
		if(ciInChannel(chat, channel))
		{
			// Get the users in the channel.
			////////////////////////////////
			data.callback = callback;
			data.param = param;
			ciChannelListUsers(chat, channel, ciEnumUsersCallback, &data);

			return;
		}
	}

	ciSocketSendf(&connection->chatSocket, "NAMES %s", channel);

	// Channel needs to be empty, not NULL, for the filter.
	///////////////////////////////////////////////////////
	if(!channel[0])
		channel = NULL;

	ID = ciAddNAMESFilter(chat, channel, callback, param);
	
	CI_DO_BLOCKING;
}


// Enumerates the channels that we are joined to
//////////////////////////////////////////////////////
void chatEnumJoinedChannels(CHAT chat,
					  chatEnumJoinedChannelsCallback callback,
					  void * param)
{
	ciEnumJoinedChannels(chat, callback, param);
}

void chatSendUserMessage(CHAT chat,
						 const char * user,
						 const char * message,
						 int type)
{
	CONNECTION;
	CONNECTED;

	ASSERT_USER(user);
	ASSERT_TYPE(type);

	if (!message || message[0] == 0)
		return;	
	
	if(type == CHAT_MESSAGE)
		ciSocketSendf(&connection->chatSocket, "PRIVMSG %s :%s", user, message);
	else if(type == CHAT_ACTION)
		ciSocketSendf(&connection->chatSocket, "PRIVMSG %s :\001ACTION %s\001", user, message);
	else if(type == CHAT_NOTICE)
		ciSocketSendf(&connection->chatSocket, "NOTICE %s :%s", user, message);
	else if(type == CHAT_UTM)
		ciSocketSendf(&connection->chatSocket, "UTM %s :%s", user, message);
	else if(type == CHAT_ATM)
		ciSocketSendf(&connection->chatSocket, "ATM %s :%s", user, message);
}

void chatGetUserInfo(CHAT chat,
					 const char * user,
					 chatGetUserInfoCallback callback,
					 void * param,
					 CHATBool blocking)
{
	int ID;
	CONNECTION;
	CONNECTED;

	ASSERT_USER(user);
	assert(callback != NULL);

	ciSocketSendf(&connection->chatSocket, "WHOIS %s", user);

	ID = ciAddWHOISFilter(chat, user, callback, param);

	CI_DO_BLOCKING;
}

void chatGetBasicUserInfo(CHAT chat,
						  const char * nick,
						  chatGetBasicUserInfoCallback callback,
						  void * param,
						  CHATBool blocking)
{
	int ID;
	const char * user;
	const char * address;
	CONNECTION;
	CONNECTED;

	ASSERT_USER(nick);
	assert(callback != NULL);

	// Check if we already have it.
	///////////////////////////////
	if(ciGetUserBasicInfo(chat, nick, &user, &address))
	{
		ciCallbackGetBasicUserInfoParams params;

		params.success = CHATTrue;
		params.nick = (char *)nick;
		params.user = (char *)user;
		params.address = (char *)address;

		ID = ciGetNextID(chat);

		ciAddCallback_wrapper(chat, CALLBACK_GET_BASIC_USER_INFO, callback, &params, param, ID, NULL);
	}
	else
	{
		ciSocketSendf(&connection->chatSocket, "WHO %s", nick);

		ID = ciAddWHOFilter(chat, nick, callback, param);
	}

	CI_DO_BLOCKING;
}

CHATBool chatGetBasicUserInfoNoWait(CHAT chat,
									const char * nick,
									const char ** user,
									const char ** address)
{
	CONNECTION;
	// 2002.Feb.28.JED - added additional check, was blowing up in GSA
	if(!connection)
		return CHATFalse;
	if(!connection->connected)
		return CHATFalse;

	ASSERT_USER(nick);

	return ciGetUserBasicInfo(chat, nick, user, address);
}

void chatGetChannelBasicUserInfo(CHAT chat,
								 const char * channel,
								 chatGetChannelBasicUserInfoCallback callback,
								 void * param,
								 CHATBool blocking)
{
	int ID;
	CONNECTION;
	CONNECTED;

	ASSERT_CHANNEL();
	assert(callback != NULL);

	ciSocketSendf(&connection->chatSocket, "WHO %s", channel);

	ID = ciAddCWHOFilter(chat, channel, callback, param);

	CI_DO_BLOCKING;
}

void chatInviteUser(CHAT chat,
					const char * channel,
					const char * user)
{
	CONNECTION;
	CONNECTED;

	ASSERT_CHANNEL();
	ASSERT_USER(user);

	ciSocketSendf(&connection->chatSocket, "INVITE %s %s", user, channel);
}

void chatKickUser(CHAT chat,
				  const char * channel,
				  const char * user,
				  const char * reason)
{
	CONNECTION;
	CONNECTED;

	ASSERT_CHANNEL();
	ASSERT_USER(user);

	if(reason == NULL)
		reason = "";

	ciSocketSendf(&connection->chatSocket, "KICK %s %s :%s", channel, user, reason);
}

void chatBanUser(CHAT chat,
				 const char * channel,
				 const char * user)
{
	CONNECTION;
	CONNECTED;

	ASSERT_CHANNEL();
	ASSERT_USER(user);

	ciSocketSendf(&connection->chatSocket, "WHOIS %s", user);

	ciAddBANFilter(chat, user, channel);
}

void chatSetUserMode(CHAT chat,
					 const char * channel,
					 const char * user,
					 int mode)
{
	int sign;

	CONNECTION;
	CONNECTED;

	ASSERT_CHANNEL();
	ASSERT_USER(user);
	ASSERT_TYPE(mode);

	sign = (mode & CHAT_OP)?'+':'-';
	ciSocketSendf(&connection->chatSocket, "MODE %s %co %s", channel, sign, user);

	sign = (mode & CHAT_VOICE)?'+':'-';
	ciSocketSendf(&connection->chatSocket, "MODE %s %cv %s", channel, sign, user);
}

void chatGetUserMode(CHAT chat,
					 const char * channel,
					 const char * user,
					 chatGetUserModeCallback callback,
					 void * param,
					 CHATBool blocking)
{
	int ID;
	int mode;
	CONNECTION;
	CONNECTED;

	ASSERT_CHANNEL();
	ASSERT_USER(user);
	assert(callback != NULL);

	// Get the mode.
	////////////////
	mode = ciGetUserMode(chat, channel, user);
	if(mode != -1)
	{
		ciCallbackGetUserModeParams params;
		params.success = CHATTrue;
		params.channel = (char *)channel;
		params.user = (char *)user;
		params.mode = mode;

		ID = ciGetNextID(chat);
		ciAddCallback_wrapper(chat, CALLBACK_GET_USER_MODE, callback, &params, param, ID, NULL);

		CI_DO_BLOCKING;
	}

	ciSocketSendf(&connection->chatSocket, "WHO %s", user);

	ID = ciAddUMODEFilter(chat, user, channel, callback, param);
	
	CI_DO_BLOCKING;
}

CHATBool chatGetUserModeNoWait(CHAT chat,
							   const char * channel,
							   const char * user,
							   int * mode)
{
	CONNECTION;
	if(!connection->connected)
		return CHATFalse;

	ASSERT_CHANNEL();
	ASSERT_USER(user);
	assert(mode);

	// Get the mode.
	////////////////
	*mode = ciGetUserMode(chat, channel, user);

	return (CHATBool)(*mode != -1);
}

/*********
** KEYS **
*********/
void chatSetGlobalKeys(CHAT chat,
					   int num,
					   const char ** keys,
					   const char ** values)
{
	char buffer[512];
	const char * key;
	const char * value;
	int i;
	CONNECTION;
	CONNECTED;

	if(!keys || !values)
		return;

	strcpy(buffer, "SETKEY :");
	for(i = 0 ; i < num ; i++)
	{
		key = keys[i];
		if(!key || !key[0])
			return;
		value = values[i];
		if(!value)
			value = "";
		sprintf(buffer + strlen(buffer), "\\%s\\%s", key, value);
	}

	ciSocketSend(&connection->chatSocket, buffer);
}

static char * ciRandomCookie()
{
	static char cookie[4];
	static int nextCookie = 0;

	sprintf(cookie, "%03d", nextCookie++);
	nextCookie %= 1000;

	return cookie;
}

static void ciSendGetKey(CHAT chat,
						 const char * target,
						 const char * cookie,
						 int num,
						 const char ** keys)
{
	char buffer[512];
	int len;
	int i;
	int j;
	int keyLen;

	CONNECTION;

	assert(target && target[0]);
	assert(cookie && cookie[0]);
	assert(num >= 1);
	assert(keys);

	// Start off the buffer.
	////////////////////////
	sprintf(buffer, "GETKEY %s %s 0 :", target, cookie);
	len = strlen(buffer);

	// Add the keys.
	////////////////
	for(i = 0 ; i < num ; i++)
	{
		// Check for a blank.
		/////////////////////
		if(!keys[i] || !keys[i][0])
			continue;

		// Check lengths.
		/////////////////
		keyLen = strlen(keys[i]);
		if((len + keyLen + 1) >= (int)sizeof(buffer))
			return;

		// Add the key.
		///////////////
		buffer[len++] = '\\';
		memcpy(buffer + len, keys[i], keyLen);
		for(j = len ; j < (len + keyLen) ; j++)
			if(buffer[j] == '\\')
				buffer[j] = '/';
		len += keyLen;
		buffer[len] = '\0';
	}

	// Send it.
	///////////
	ciSocketSend(&connection->chatSocket, buffer);
}

void chatGetGlobalKeys(CHAT chat,
					   const char * target,
					   int num,
					   const char ** keys,
					   chatGetGlobalKeysCallback callback,
					   void * param,
					   CHATBool blocking)
{
	char * cookie;
	const char * channel;
	int ID;
	CONNECTION;
	CONNECTED;

	assert(num >= 0);
	assert(keys);

	if(!target || !target[0])
		target = connection->nick;

	// Get a cookie.
	////////////////
	cookie = ciRandomCookie();

	// Send the request.
	////////////////////
	ciSendGetKey(chat, target, cookie, num, keys);

	// Check if this is a channel or a user.
	////////////////////////////////////////
	if(target[0] == '#')
		channel = target;
	else
		channel = NULL;

	ID = ciAddGETKEYFilter(chat, cookie, num, keys, channel, callback, param);
	
	CI_DO_BLOCKING;
}

void chatSetChannelKeys(CHAT chat,
						const char * channel,
						const char * user,
						int num,
						const char ** keys,
						const char ** values)
{
	char buffer[512];
	const char * value;
	int i;
	CONNECTION;
	CONNECTED;

	if(!user || !user[0])
		sprintf(buffer, "SETCHANKEY %s :", channel);
	else
		sprintf(buffer, "SETCKEY %s %s :", channel, user);
	for(i = 0 ; i < num ; i++)
	{
		value = values[i];
		if(!value)
			value = "";
		sprintf(buffer + strlen(buffer), "\\%s\\%s", keys[i], value);
	}

	ciSocketSend(&connection->chatSocket, buffer);
}

static CHATBool ciSendGetChannelKey(CHAT chat,
									const char * channel,
									const char * nick,
									const char * cookie,
									int num,
									const char ** keys)
{
	char buffer[512];
	int len;
	int i;
	int j;
	int keyLen;
	CHATBool getBrocastKeys = CHATFalse;

	CONNECTION;

	assert(channel && channel[0]);
	assert(cookie && cookie[0]);
	assert(!num || keys);

	// Start off the buffer.
	////////////////////////
	if(!nick || !nick[0])
		sprintf(buffer, "GETCHANKEY %s %s 0 :", channel, cookie);
	else
		sprintf(buffer, "GETCKEY %s %s %s 0 :", channel, nick, cookie);
	len = strlen(buffer);

	// Add the keys.
	////////////////
	for(i = 0 ; i < num ; i++)
	{
		// Check for a blank.
		/////////////////////
		if(!keys[i] || !keys[i][0])
			continue;

		// Check for b_*.
		/////////////////
		if(strcmp(keys[i], "b_*") == 0)
		{
			getBrocastKeys = CHATTrue;
			continue;
		}

		// Check lengths.
		/////////////////
		keyLen = strlen(keys[i]);
		if((len + keyLen + 1) >= (int)sizeof(buffer))
			continue;

		// Add the key.
		///////////////
		buffer[len++] = '\\';
		memcpy(buffer + len, keys[i], keyLen);
		for(j = len ; j < (len + keyLen) ; j++)
			if(buffer[j] == '\\')
				buffer[j] = '/';
		len += keyLen;
		buffer[len] = '\0';
	}

	// Check for broadcast keys.
	////////////////////////////
	if(getBrocastKeys)
	{
		if((len + 4) < (int)sizeof(buffer))
		{
			strcpy(buffer + len, "\\b_*");
			len += 4;
		}
	}
	
	// Check for requesting all keys on a room.
	///////////////////////////////////////////
	if(!num && (!nick || !nick[0]))
	{
		strcpy(buffer + len, "*");
		len++;
	}

	// Send it.
	///////////
	ciSocketSend(&connection->chatSocket, buffer);

	return getBrocastKeys;
}

void chatGetChannelKeys(CHAT chat,
						const char * channel,
						const char * user,
						int num,
						const char ** keys,
						chatGetChannelKeysCallback callback,
						void * param,
						CHATBool blocking)
{
	char * cookie;
	int ID;
	CHATBool getBroadcastKeys;
	CONNECTION;
	CONNECTED;

	assert(num >= 0);
	assert(!num || keys);

	// Get a cookie.
	////////////////
	cookie = ciRandomCookie();

	// Send the request.
	////////////////////
	getBroadcastKeys = ciSendGetChannelKey(chat, channel, user, cookie, num, keys);

	if(!user || !user[0])
		ID = ciAddGETCHANKEYFilter(chat, cookie, num, keys, getBroadcastKeys, callback, param);
	else
		ID = ciAddGETCKEYFilter(chat, cookie, num, keys, (CHATBool)(strcmp(user, "*") == 0), getBroadcastKeys, callback, param);
	
	CI_DO_BLOCKING;
}


// Check if a given nickname is valid.  Looks for illegal IRC characters.
// [in] nick		-  The nickname to validate
CHATBool ciNickIsValid(const char* nick)
{
	// Empty nick is invalid
	if ((NULL == nick) || ('\0' == *nick))
		return CHATFalse;

	// Nick can't start with a number or '-'
	if(isdigit(*nick) || (*nick == '-'))
		return CHATFalse;

	// Make sure each character is valid
	while(*nick != '\0') 
	{
		// If the character isn't in the valid set, the nick is not valid
		if (NULL == strchr(VALID_NICK_CHARS,*nick++))
			return CHATFalse;
	}
	
	return CHATTrue;
}

/****************
** NICK ERRORS **
****************/
void ciNickError(CHAT chat, int type)
{
	CONNECTION;

	// Check if there's a nick-in-use callback.
	///////////////////////////////////////////
	if(connection->nickErrorCallback)
	{
		// Add the callback.
		////////////////////
		ciCallbackNickErrorParams params;
		memset(&params, 0, sizeof(ciCallbackNickErrorParams));
		params.type = type;
		params.nick = connection->nick;
		ciAddCallback_wrapper(chat, CALLBACK_NICK_ERROR, connection->nickErrorCallback, &params, connection->connectParam, 0, NULL);
	}
	else
	{
		// There's no callback, disconnect.
		///////////////////////////////////
		connection->connecting = CHATFalse;

		// Call the callback.
		/////////////////////
		if(connection->connectCallback != NULL)
			connection->connectCallback(chat, CHATFalse, connection->connectParam);
	}
}

