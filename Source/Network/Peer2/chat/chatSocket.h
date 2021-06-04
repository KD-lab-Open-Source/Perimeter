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

#ifndef _CHATSOCKET_H_
#define _CHATSOCKET_H_

/*************
** INCLUDES **
*************/
#include <stdio.h>
#include "chat.h"
#include "chatCrypt.h"
#if defined(applec) || defined(THINK_C) || defined(__MWERKS__) && !defined(__mips64) && !defined(_WIN32)
	#include "::nonport.h"
#else
	#include "../nonport.h"
#endif

/**********
** ENUMS **
**********/
typedef enum ciConnectState
{
	ciNotConnected,
	ciConnected,
	ciDisconnected
} ciConnectState;

/**********
** TYPES **
**********/
typedef struct ciBuffer
{
	char * buffer;
	int length;
	int size;
} ciBuffer;

typedef struct ciServerMessage
{
	char * message;
	char * server;
	char * nick;
	char * user;
	char * host;
	char * command;
	char * middle;
	char * param;
	char ** params;
	int numParams;
} ciServerMessage;

typedef struct ciSocket
{
	SOCKET sock;
	ciConnectState connectState;
	char serverAddress[256];

	ciBuffer inputQueue;
	ciBuffer outputQueue;

	CHATBool secure;
	gs_crypt_key inKey;
	gs_crypt_key outKey;

	ciServerMessage lastMessage;

#ifdef IRC_LOG
	char filename[FILENAME_MAX];
#endif
} ciSocket;

/**************
** FUNCTIONS **
**************/
CHATBool ciSocketInit(ciSocket * sock, const char * nick);

CHATBool ciSocketConnect(ciSocket * sock,
					 const char * serverAddress,
					 int port);

void ciSocketDisconnect(ciSocket * sock);

void ciSocketThink(ciSocket * sock);

CHATBool ciSocketSend(ciSocket * sock,
				  const char * buffer);

CHATBool ciSocketSendf(ciSocket * sock,
				   const char * format,
				   ...);

ciServerMessage * ciSocketRecv(ciSocket * sock);

#endif
