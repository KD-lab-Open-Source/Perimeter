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
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include "chatSocket.h"

#include <iostream>

/************
** DEFINES **
************/
#define BUFFER_INC      8192
#define RECV_LEN        4096

/***********
** MACROS **
***********/
#define ASSERT_SOCK(sock)   {\
								assert((sock) != NULL);\
								assert(((sock)->connectState == ciNotConnected) ||\
									((sock)->connectState == ciConnected) ||\
									((sock)->connectState == ciDisconnected));\
								ASSERT_BUFFER(&(sock)->inputQueue);\
								ASSERT_BUFFER(&(sock)->outputQueue);\
							}

#define ASSERT_CONNECTED(sock)  assert((sock)->connectState == ciConnected)

#define ASSERT_BUFFER(buffer)   {\
									assert((buffer) != NULL);\
									assert((buffer)->size >= 0);\
									assert(((buffer)->size % BUFFER_INC) == 0);\
									assert((buffer)->length >= 0);\
									assert((buffer)->length <= (buffer)->size);\
								}
#define RESET(ptr)  {if (ptr) {gsifree(ptr); ptr = NULL;} }

/*********
** TIME **
*********/
#ifdef __MWERKS__
const char * ciGetTime(void); // Codewarrior warning if not prototyped
#endif

const char * ciGetTime(void)
{
#if defined(UNDER_CE) || defined(_PS2)
	return "";
#else
	static char buffer[256];
	time_t timer;
	struct tm * now;

	timer = time(NULL);
	now = localtime(&timer);
	if(now) // fixes the date > 2060 crash (23apr03/bgw)
	{
		if(now->tm_year > 99)
			now->tm_year -= 100;
		sprintf(buffer, "%02d.%02d.%02d %02d:%02d.%02d", now->tm_mon + 1, now->tm_mday, now->tm_year, now->tm_hour, now->tm_min, now->tm_sec);
	}
	else
		strcpy(buffer, "00.00.00 00:00.00");

	return buffer;
#endif
}

/***********
** BUFFER **
***********/
static CHATBool ciBufferInit(ciBuffer * buffer)
{
	assert(buffer != NULL);

	buffer->length = 0;
	buffer->size = BUFFER_INC;
	buffer->buffer = (char *)gsimalloc(buffer->size + 1);
	if(buffer->buffer == NULL)
		return CHATFalse;

	// Just for fun.
	////////////////
	buffer->buffer[0] = '\0';

	return CHATTrue;
}

static void ciBufferFree(ciBuffer * buffer)
{
	gsifree(buffer->buffer);
}

static CHATBool ciBufferPreAppend(ciBuffer * buffer, int len)
{
	int total;
	char * tempPtr;

	ASSERT_BUFFER(buffer);
	assert(len >= 0);
	assert(len <= SHRT_MAX); // sanity check

	// Check if the buffer is big enough.
	/////////////////////////////////////
	total = (buffer->length + len);
	if(total <= buffer->size)
		return CHATTrue;

	// Figure out the new size.
	///////////////////////////
	total += BUFFER_INC;
	total -= (total % BUFFER_INC);

	// Allocate the memory.
	///////////////////////
	tempPtr = (char *)gsirealloc(buffer->buffer, total + 1);
	if(tempPtr == NULL)
		return CHATFalse;

	// Update the buffer.
	/////////////////////
	buffer->buffer = tempPtr;
	buffer->size = total;

	return CHATTrue;
}

static void ciBufferClipFront(ciBuffer * buffer, int len)
{
	ASSERT_BUFFER(buffer);
	assert(len >= 0);
	assert(len <= buffer->length);

	buffer->length -= len;
	memmove(buffer->buffer, &buffer->buffer[len], buffer->length);
	buffer->buffer[buffer->length] = '\0';
}


#ifdef __MWERKS__ // CodeWarrior will warn if not prototyped
/***************
** PROTOTYPES **
****************/
CHATBool ciParseParam(const char *pText, ciServerMessage * message);
#endif

/**************
** FUNCTIONS **
**************/
CHATBool ciSocketInit(ciSocket * sock, const char * nick)
{
#ifdef IRC_LOG
	FILE * log;
#endif

	assert(sock != NULL);

	memset(sock, 0, sizeof(ciSocket));

	sock->sock = INVALID_SOCKET;
	if(!ciBufferInit(&sock->inputQueue))
		return CHATFalse;
	if(!ciBufferInit(&sock->outputQueue))
	{
		ciBufferFree(&sock->inputQueue);
		return CHATFalse;
	}
#ifdef IRC_LOG
	sprintf(sock->filename, "%s_irc.log", nick);
	log = fopen(sock->filename, "at");
	if(log != NULL)
	{
		fprintf(log, "\n\n\n\n\nCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC\n");
		fclose(log);
	}
#endif
	return CHATTrue;
	
	GSI_UNUSED(nick);
}

CHATBool ciSocketConnect(ciSocket * sock,
					 const char * serverAddress,
					 int port)
{
	unsigned int ip;
	HOSTENT * host;
	SOCKADDR_IN address;
	int rcode;
	int keepalive;

	ASSERT_SOCK(sock);
	assert(serverAddress != NULL);
	assert(port >= 0);
	assert(port <= USHRT_MAX);
	assert(sock->connectState == ciNotConnected);

	// Copy off the address.
	////////////////////////
	strncpy(sock->serverAddress, serverAddress, 255);

	// Try resolving the string as an IP a.b.c.d number.
	////////////////////////////////////////////////////
	ip = inet_addr(serverAddress);
	if(ip == INADDR_NONE)
	{
		// Try resolving with DNS.
		//////////////////////////
		host = gethostbyname((char *)serverAddress);
		if(host == NULL)
			return CHATFalse;

		// Get the ip.
		//////////////
		ip = *(unsigned int *)host->h_addr_list[0];
	}

	// Setup the address.
	/////////////////////
	memset(&address, 0, sizeof(SOCKADDR_IN));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = ip;
	address.sin_port = htons((unsigned short)port);

	// Create the socket.
	/////////////////////
	sock->sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock->sock == INVALID_SOCKET)
		return CHATFalse;

	// Enable keep-alive.
	/////////////////////
	keepalive = 1;
	rcode = setsockopt(sock->sock, SOL_SOCKET, SO_KEEPALIVE, (char *)&keepalive, sizeof(int));
	//assert(rcode != SOCKET_ERROR);

	// Try and connect.
	///////////////////
	rcode = connect(sock->sock, (SOCKADDR *)&address, sizeof(SOCKADDR_IN));
	if(rcode == SOCKET_ERROR)
	{
		closesocket(sock->sock);
		return CHATFalse;
	}

	// We're connected.
	///////////////////
	sock->connectState = ciConnected;

	return CHATTrue;
}

void ciSocketDisconnect(ciSocket * sock)
{
	int i;

	ASSERT_SOCK(sock);

	// Shutdown the socket.
	///////////////////////
	if(sock->sock != INVALID_SOCKET)
	{
		shutdown(sock->sock, 2);
		closesocket(sock->sock);
	}

	// We're disconnected.
	//////////////////////
	sock->connectState = ciDisconnected;

	// gsifree the buffers.
	////////////////////
	ciBufferFree(&sock->inputQueue);
	ciBufferFree(&sock->outputQueue);

	// gsifree the last-message pointers.
	//////////////////////////////////
	gsifree(sock->lastMessage.message);
	gsifree(sock->lastMessage.server);
	gsifree(sock->lastMessage.nick);
	gsifree(sock->lastMessage.user);
	gsifree(sock->lastMessage.host);
	gsifree(sock->lastMessage.command);
	gsifree(sock->lastMessage.middle);
	gsifree(sock->lastMessage.param);
	for(i = 0 ; i < sock->lastMessage.numParams ; i++)
		gsifree(sock->lastMessage.params[i]);
	gsifree(sock->lastMessage.params);
}

static void ciSocketSelect(SOCKET sock, CHATBool * readFlag, CHATBool * writeFlag, CHATBool * exceptFlag)
{
	fd_set writeSet;
	fd_set readSet;
	fd_set exceptSet;
	fd_set * writefds;
	fd_set * readfds;
	fd_set * exceptfds;
	int rcode;
	struct timeval timeout;

	assert(sock != INVALID_SOCKET);

	// Setup the parameters.
	////////////////////////
	if(readFlag != NULL)
	{
		FD_ZERO(&readSet);
		FD_SET(sock, &readSet);
		readfds = &readSet;
	}
	else
	{
		readfds = NULL;
	}
	if(writeFlag != NULL)
	{
		FD_ZERO(&writeSet);
		FD_SET(sock, &writeSet);
		writefds = &writeSet;
	}
	else
	{
		writefds = NULL;
	}
	if(exceptFlag != NULL)
	{
		FD_ZERO(&exceptSet);
		FD_SET(sock, &exceptSet);
		exceptfds = &exceptSet;
	}
	else
	{
		exceptfds = NULL;
	}

	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	// Get the write info.
	//////////////////////
	rcode = select(FD_SETSIZE, readfds, writefds, exceptfds, &timeout);

	// Check results.
	/////////////////
	if(readFlag != NULL)
	{
		if(rcode > 0 && FD_ISSET(sock, readfds))
			*readFlag = CHATTrue;
		else
			*readFlag = CHATFalse;
	}
	if(writeFlag != NULL)
	{
		if(rcode > 0 && FD_ISSET(sock, writefds))
			*writeFlag = CHATTrue;
		else
			*writeFlag = CHATFalse;
	}
	if(exceptFlag != NULL)
	{
		if(rcode > 0 && FD_ISSET(sock, exceptfds))
			*exceptFlag = CHATTrue;
		else
			*exceptFlag = CHATFalse;
	}
}

static void ciSocketThinkSend(ciSocket * sock)
{
	int rcode;
	int len;

	ASSERT_SOCK(sock);
	ASSERT_CONNECTED(sock);

	// While there's data to send...
	////////////////////////////////
	while(sock->outputQueue.length > 0)
	{
		CHATBool writeFlag;
		
		// Can we send?
		///////////////
		ciSocketSelect(sock->sock, NULL, &writeFlag, NULL);
		if(!writeFlag)
			return;

		// Try and send some.
		/////////////////////
		len = (std::min)(sock->outputQueue.length, 1024);
		rcode = send(sock->sock, sock->outputQueue.buffer, len, 0);
		if(rcode == 0)
			return;
		if(rcode == SOCKET_ERROR)
			return; //ERRCON

		// Update the output queue.
		///////////////////////////
		ciBufferClipFront(&sock->outputQueue, rcode);
	}
}

static void ciSocketThinkRecv(ciSocket * sock)
{
	CHATBool readFlag;
	int len;
	int rcode;
	char * pos;
	
	ASSERT_SOCK(sock);
	ASSERT_CONNECTED(sock);

	while(CHATTrue)
	{
		// Check the read flag.
		///////////////////////
		ciSocketSelect(sock->sock, &readFlag, NULL, NULL);

		// Nothing to read?
		///////////////////
		if(!readFlag)
			return;

		// Make sure the buffer is ready.
		/////////////////////////////////
		if(!ciBufferPreAppend(&sock->inputQueue, RECV_LEN))
			return; //ERRCON

		// Recv into the buffer.
		////////////////////////
		pos = &sock->inputQueue.buffer[sock->inputQueue.length];
		rcode = recv(sock->sock, pos, RECV_LEN, 0);

		// Connection closed?
		/////////////////////
		if(rcode <= 0) //crt -- handle remote disconnections
		{
			rcode = GOAGetLastError(sock->sock);
			sock->connectState = ciDisconnected;
			return;
		}

		// Set the number of bytes read.
		////////////////////////////////
		len = rcode;

#if 0
{
		char * buffer;
		int i;
		buffer = (char *)gsimalloc(len + 2);
		assert(buffer != NULL);
		memcpy(buffer, &sock->inputQueue.buffer[sock->inputQueue.length], len);
		buffer[len] = '\0';
		OutputDebugString("--->>>XXXRECV\n");
		for(i = 0 ; i < len ; i += 1023)
			OutputDebugString(&buffer[i]);
		OutputDebugString("<<<---XXXRECV\n");
}
#endif

		// Decrypt data if the socket is secure.
		////////////////////////////////////////
		if(sock->secure)
			gs_crypt((unsigned char *)pos, len, &sock->inKey);

		// Update the buffer length.
		////////////////////////////
		sock->inputQueue.length += len;

		// NUL terminate the buffer.
		////////////////////////////
		sock->inputQueue.buffer[sock->inputQueue.length] = '\0';
	}
}

void ciSocketThink(ciSocket * sock)
{
	ASSERT_SOCK(sock);
	ASSERT_CONNECTED(sock);

	// Disconnected?
	////////////////
	if(sock->connectState == ciDisconnected)
		return;

	// Send any waiting data.
	/////////////////////////
	ciSocketThinkSend(sock);

	// Recv any waiting data.
	/////////////////////////
	ciSocketThinkRecv(sock);
}

CHATBool ciSocketSend(ciSocket * sock,
				  const char * buffer)
{
#ifdef IRC_LOG
	FILE * log;
#endif
	int len;
	char * pos;

	ASSERT_SOCK(sock);
	ASSERT_CONNECTED(sock);
	assert(buffer != NULL);

	// Disconnected?
	////////////////
	if(sock->connectState == ciDisconnected)
		return CHATTrue;

	// Get the buffer length.
	/////////////////////////
	len = strlen(buffer);

	// Make sure the buffer is big enough.
	//////////////////////////////////////
	if(!ciBufferPreAppend(&sock->outputQueue, len + 2))
		return CHATFalse;

	// Append to the output buffer.
	///////////////////////////////
	pos = &sock->outputQueue.buffer[sock->outputQueue.length];
	memcpy(pos, buffer, len);

	// Update the buffer length.
	////////////////////////////
	sock->outputQueue.length += len;

	// Add the CRLF.
	////////////////
	sock->outputQueue.buffer[sock->outputQueue.length++] = 0x0D;
	sock->outputQueue.buffer[sock->outputQueue.length++] = 0x0A;

	// Encrypt data if the socket is secure.
	////////////////////////////////////////
	if(sock->secure)
		gs_crypt((unsigned char *)pos, len + 2, &sock->outKey);

#ifdef IRC_LOG
	// Write it to the log.
	///////////////////////
	log = fopen(sock->filename, "at");
	if(log != NULL)
	{
		fprintf(log, "%s | OUT | %s\n", ciGetTime(), buffer);
		fclose(log);
	}
#endif

	return CHATTrue;
}

CHATBool ciSocketSendf(ciSocket * sock,
				   const char * format,
				   ...)
{
	static char buffer[4096];
	int num;
	va_list args;

	ASSERT_SOCK(sock);
	ASSERT_CONNECTED(sock);
	assert(buffer != NULL);

	// Disconnected?
	////////////////
	if(sock->connectState == ciDisconnected)
		return CHATTrue;

	// Do the formatting.
	/////////////////////
	va_start(args, format);
	num = vsprintf(buffer, format, args);
	if(num != -1)
		buffer[num] = '\0';
	else
		buffer[sizeof(buffer) - 1] = '\0'; //ERRCON

	// Send.
	////////
	return ciSocketSend(sock, buffer);
}

static CHATBool ciParseUser(const char *pText, ciServerMessage * message)
{
	char *pTmpNick = NULL, *pTmpUsername = NULL, *pTmpHost = NULL;
	int    nNick = 0, nUsername = 0, nHost = 0;
	char *p;
	
	if(pText == NULL || pText[0] == '\0')
	{
		assert(0);
		return CHATFalse; //ERRCON
	}
	
	p = pTmpNick = (char *)pText;
	
	while(*p != '\0')
	{
		if(*p != '!')
		{
			++p;
			++nNick;
		}
		else
		{
			pTmpUsername = ++p;
			
			while(*p != '\0')
			{
				if(*p != '@')
				{
					++p;
					++nUsername;
				}
				else
				{
					pTmpHost = ++p;
					
					while(*p != '\0')
					{
						++p;
						++nHost;
					}
				}
			}
		}
	}
	
	if(nNick)
	{
		message->nick = (char *)gsimalloc(nNick + 1);
		
		if(message->nick)
		{
			memcpy(message->nick, pTmpNick, nNick);
			message->nick[nNick] = '\0';
		}
	}
	else
		message->nick = NULL;
	
	if(nUsername)
	{
		message->user = (char *)gsimalloc(nUsername + 1);
		
		if(message->user)
		{
			memcpy(message->user, pTmpUsername, nUsername);
			message->user[nUsername] = '\0';
		}
	}
	else
		message->user = NULL;
	
	if(nHost)
	{
		message->host = (char *)gsimalloc(nHost + 1);
		
		if(message->host)
		{
			memcpy(message->host, pTmpHost, nHost);
			message->host[nHost] = '\0';
		}
	}
	else
		message->host = NULL;

	return CHATTrue;
}

static CHATBool ciAddParam(const char *param, ciServerMessage * message)
{
	void * tempPtr;
	
	// Reallocate the parameter array.
	//////////////////////////////////
	tempPtr = gsirealloc(message->params, sizeof(char *) * (message->numParams + 1));
	if(tempPtr == NULL)
		return CHATFalse; //ERRCON
	message->params = (char **)tempPtr;

	// Allocate mem for the param.
	//////////////////////////////
	tempPtr = gsimalloc(strlen(param) + 1);
	if(tempPtr == NULL)
		return CHATFalse; //ERRCON

	// Copy the param.
	//////////////////
	strcpy((char *)tempPtr, param);
	message->params[message->numParams++] = (char *)tempPtr;

	return CHATTrue;
}

CHATBool ciParseParam(const char *pText, ciServerMessage * message)
{
	char * colon;
	char * str;
	char * p;

	assert(pText != NULL);
	assert(message != NULL);

	// Copy off the text.
	/////////////////////
	p = (char *)gsimalloc(strlen(pText) + 1);
	if(p == NULL)
		return CHATFalse; //ERRCON
	strcpy(p, pText);

	// Find the colon.
	//////////////////
	if(p[0] == ':')
	{
		p[0] = '\0';
		colon = &p[1];
	}
	else
	{
		colon = strstr(p, " :");
		if(colon != NULL)
		{
			*colon = '\0';
			colon += 2;
		}
	}

	str = strtok(p, " ");
	while(str != NULL)
	{
		// Add the param.
		/////////////////
		if(!ciAddParam(str, message))
		{
			gsifree(p);
			return CHATFalse; //ERRCON
		}

		// Get the next one.
		////////////////////
		str = strtok(NULL, " ");
	}

	if(colon != NULL)
	{
		// Add the last param.
		//////////////////////
		if(!ciAddParam(colon, message))
		{
			gsifree(p);
			return CHATFalse; //ERRCON
		}
	}

	gsifree(p);
	return CHATTrue;
}

static CHATBool ciParseMessage(ciSocket * sock, const char *sText)
{	
	//	TRACE("ServM: Parse Data=%s\n", sText);
	int    nMessage = 0, nServer = 0, nCommand = 0, nMiddle = 0, nParam = 0;
	char *p, *temp;
	ciServerMessage * message = &sock->lastMessage;
	
	if(sText == NULL || sText[0] == '\0')
	{
		assert(0);
		return CHATFalse; //ERRCON
	}

	nMessage = strlen(sText);
	message->message = (char *)gsimalloc(nMessage + 1);
	if(message->message == NULL)
		return CHATFalse; //ERRCON
	memcpy(message->message, sText, nMessage);
	message->message[nMessage] = '\0';

	p = (char *)sText;
	
	while(*p == '\n' || *p == '\r')
		++p;
	
	if(*p == ':')
	{  // server
		message->server = ++p;  // ?? BUGBUG
		
		if(*p != '\0')
		{
			while(*p != ' ' && *p != '\0')
			{
				++nServer;
				++p;
			}
		}
	}
	
	while(*p == ' ')
		// skip spaces
		++p;
	
	if(*p != '\0')
	{  // command
		message->command = p;
		
		while(*p != ' ' && *p != '\0')
		{
			++nCommand;
			++p;
		}
	}
	
	while(*p == ' ')
		// skip spaces
		++p;
	
	if(*p != ':' && *p != '\0')
	{  // middle
		message->middle = p;
		
		while(*p != ' ' && *p != '\0')
		{
			++nMiddle;
			++p;
		}
	}
	
	while(*p == ' ')
		// skip spaces
		++p;
	
	//if(*p == ':')
		// params delimiter
	//	++p;
	
	if(*p != '\0')
	{  // params
		message->param = p;
		
		while(*p != '\0')
		{
			++nParam;
			++p;
		}
	}
	
	if(nServer)
	{
		temp = message->server;
		message->server = (char *)gsimalloc(nServer + 1);
		
		if(message->server)
		{
			memcpy(message->server, temp, nServer);
			message->server[nServer] = '\0';
		}

		if(!ciParseUser(message->server, message))
		{
			RESET(message->message);
			RESET(message->server);
			return CHATFalse; //ERRCON
		}
	}
	else
	{
		message->server = NULL;
		message->nick = NULL;
		message->user = NULL;
		message->host = NULL;
	}

	if(nMiddle)
	{
		if(!ciParseParam(message->middle, message))
		{
			RESET(message->message);
			RESET(message->server);
			RESET(message->nick);
			RESET(message->user);
			RESET(message->host);
			return CHATFalse; //ERRCON
		}
	}
	else if(nParam)
	{
		if(!ciParseParam(message->param, message))
		{
			RESET(message->message);
			RESET(message->server);
			RESET(message->nick);
			RESET(message->user);
			RESET(message->host);
			return CHATFalse; //ERRCON
		}
	}
	else
	{
		message->params = NULL;
		message->numParams = 0;
	}
	
	if(nParam)
	{
		temp = message->param;
		message->param = (char *)gsimalloc(nParam + 1);

		if(message->param)
		{
			memcpy(message->param, temp, nParam);
			message->param[nParam] = '\0';
		}
	}
	else
	{
		message->param = NULL;
	}
	
	if(nCommand)
	{
		temp = message->command;
		message->command = (char *)gsimalloc(nCommand + 1);
		
		if(message->command)
		{
			memcpy(message->command, temp, nCommand);
			message->command[nCommand] = '\0';
		}
	}
	else
		message->command = NULL;
	
	if(nMiddle)
	{
		temp = message->middle;
		message->middle = (char *)gsimalloc(nMiddle + 1);
		
		if(message->middle)
		{
			memcpy(message->middle, temp, nMiddle);
			message->middle[nMiddle] = '\0';
		}
	}
	else
		message->middle = NULL;

	return CHATTrue;
}


static CHATBool ciParseInput(ciSocket * sock)
{
	char *p, *q, *r;
	char   temp;
	int i;
	
	p = sock->inputQueue.buffer;  // start
	
	if(*p != '\0')
	{
		// eat all CRs & LFs
		while(*p == 13 || *p == 10)
			++p;
		
		// end of string ?
		if(*p != '\0')
		{
			r = q = p;
			
			// everything between last-nonspace char and CRLF should be discarded
			while(*q != 10 && *q != 13 && *q != '\0')
			{
				if(*q != ' ')
					r = q;
				++q;
			}
			
			if(*q != '\0')
			{
				++r;
				temp = *r;
				*r = '\0';

				// gsifree the old message stuff.
				//////////////////////////////
				RESET(sock->lastMessage.message);
				RESET(sock->lastMessage.server);
				RESET(sock->lastMessage.nick);
				RESET(sock->lastMessage.user);
				RESET(sock->lastMessage.host);
				RESET(sock->lastMessage.command);
				RESET(sock->lastMessage.middle);
				RESET(sock->lastMessage.param);
				for(i = 0 ; i < sock->lastMessage.numParams ; i++)
					RESET(sock->lastMessage.params[i]);
				RESET(sock->lastMessage.params);
				sock->lastMessage.numParams = 0;

				// Parse the message.
				/////////////////////
				memset(&sock->lastMessage,0,sizeof(sock->lastMessage));
				if(!ciParseMessage(sock, p))
				{
					memset(&sock->lastMessage,0,sizeof(sock->lastMessage));
					return CHATFalse; //ERRCON
				}

				// Restore the temp character.
				//////////////////////////////
				*r = temp;

				// Take the message out of the buffer.
				//////////////////////////////////////
				ciBufferClipFront(&sock->inputQueue, (q - sock->inputQueue.buffer));

				return CHATTrue;
			}
		}
	}

	return CHATFalse;
}

ciServerMessage * ciSocketRecv(ciSocket * sock)
{
#ifdef IRC_LOG
	FILE * log;
#endif
	ASSERT_SOCK(sock);

	if(sock->connectState == ciDisconnected)
		return NULL;

	// Check for an empty buffer.
	/////////////////////////////
	if(sock->inputQueue.length == 0)
		return NULL;

	// Check for a message.
	///////////////////////
	if(!ciParseInput(sock))
	{
		// No message.
		//////////////
		return NULL;
	}

#ifdef IRC_LOG
	// Write it to the log.
	///////////////////////
	log = fopen(sock->filename, "at");
	if(log != NULL)
	{
		fprintf(log, "%s | IN  | %s\n", ciGetTime(), sock->lastMessage.message);
		fclose(log);
	}
#endif

	// Got a message.
	/////////////////
	return &sock->lastMessage;
}
