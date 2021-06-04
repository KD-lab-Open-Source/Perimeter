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

#ifndef _CHATMAIN_H_
#define _CHATMAIN_H_

/*************
** INCLUDES **
*************/
#include "chat.h"
#include "chatSocket.h"
#include "chatHandlers.h"
#if defined(applec) || defined(THINK_C) || defined(__MWERKS__) && !defined(__mips64) && !defined(_WIN32)
	#include "::hashtable.h"
	#include "::darray.h"
	#include "::nonport.h"
#else
	#include "../hashtable.h"
	#include "../darray.h"
	#include "../nonport.h"
#endif

/************
** DEFINES **
************/
#define MAX_NICK         64
#define MAX_NAME        128
#define MAX_USER        128
#define MAX_SERVER      128
#define MAX_PARAM       512
#define MAX_SECRETKEY   128

#define CONNECTION      ciConnection * connection;\
						assert(chat != NULL);\
						connection = (ciConnection *)chat;
#define CONNECTED       if(!connection->connected) return; //ERRCON
#if 0
ciConnection * connection;  // for visual assist
#endif

#define VALID_NICK_CHARS    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789[]\\`_^{|}-"

/**********
** TYPES **
**********/
typedef struct ciConnection
{
	CHATBool connected;
	CHATBool connecting;
	CHATBool disconnected;
	chatNickErrorCallback nickErrorCallback;
	chatFillInUserCallback fillInUserCallback;
	chatConnectCallback connectCallback;
	void * connectParam;

	ciSocket chatSocket;

	char nick[MAX_NICK];
	char name[MAX_NAME];
	char user[MAX_USER];
	unsigned int IP;

	char server[MAX_SERVER];
	int port;

	chatGlobalCallbacks globalCallbacks;

	HashTable channelTable;
	DArray enteringChannelList;

	ciServerMessageFilter * filterList;
	ciServerMessageFilter * lastFilter;

	int nextID;

	DArray callbackList;

	CHATBool quiet;

	char secretKey[MAX_SECRETKEY];
} ciConnection;

void ciSendNickAndUser(CHAT chat);
void ciHandleDisconnect(CHAT chat, const char * reason);
CHATBool ciNickIsValid(const char* nick);
void ciNickError(CHAT chat, int type);


#endif
