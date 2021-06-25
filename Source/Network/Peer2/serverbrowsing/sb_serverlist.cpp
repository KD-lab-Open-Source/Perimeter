#include "sb_serverbrowsing.h"
#include "sb_internal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

#define SERVER_GROWBY 100

//for the master server info
#define INCOMING_BUFFER_SIZE 4096

#define MAX_OUTGOING_REQUEST_SIZE (MAX_FIELD_LIST_LEN + MAX_FILTER_LEN + 255)




//global pointer to alternate master server name/IP
char *SBOverrideMasterServer = NULL;


int NTSLengthSB(char *buf, int len)
{
	int i;
	for (i = 0 ; i < len ; i++)
	{
		if (buf[i] == '\0') //found a full NTS
			return i + 1; //return the length including the null
	}
	return -1;
}



void SBServerListInit(SBServerList *slist, const char *queryForGamename, const char *queryFromGamename, const char *queryFromKey, int queryFromVersion, SBListCallBackFn callback, void *instance)
{
	assert(slist != NULL);
	slist->state = sl_disconnected;
	SBAllocateServerList(slist);
	SBRefStrHash(slist); //make sure it's initialized
	strcpy(slist->queryforgamename, queryForGamename);
	strcpy(slist->queryfromgamename, queryFromGamename);
	strcpy(slist->queryfromkey, queryFromKey);
	slist->ListCallback = callback;
	assert(callback != NULL);
	slist->instance = instance;
	slist->sortkey = "";
	slist->mypublicip = 0;
	slist->slsocket = INVALID_SOCKET;
	slist->inbuffer = NULL;
	slist->inbufferlen = 0;
	slist->keylist = NULL;
	slist->expectedelements = -1;
	slist->numpopularvalues = 0;
	slist->lasterror = "";
	slist->srcip = 0;
	slist->fromgamever = queryFromVersion;

	srand((unsigned int)current_time());
	SocketStartUp();
	
}


static void ErrorDisconnect(SBServerList *slist)
{
	//call the callback..
	slist->ListCallback(slist, slc_disconnected, SBNullServer, slist->instance);
	SBServerListDisconnect(slist);	
}

#define MULTIPLIER -1664117991
static int StringHash(const char *s, int numbuckets)
{
	goa_uint32 hashcode = 0;
	while (*s != 0)
	{
		hashcode = hashcode * MULTIPLIER + tolower(*s);
		s++;
	}
    return (hashcode % numbuckets);

}



static SBError ServerListConnect(SBServerList *slist)
{
	struct   sockaddr_in saddr;
	struct hostent *hent;
	char masterHostname[128];
	int masterIndex;
	

	masterIndex = StringHash(slist->queryforgamename, NUM_MASTER_SERVERS);
	if (SBOverrideMasterServer != NULL)
		strcpy(masterHostname, SBOverrideMasterServer);
	else //use the default format...
		sprintf(masterHostname,"%s.ms%d.gamespy.com", slist->queryforgamename, masterIndex);
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(MSPORT2);
	saddr.sin_addr.s_addr = inet_addr(masterHostname);
	if (saddr.sin_addr.s_addr == INADDR_NONE)
	{
		hent = gethostbyname(masterHostname);
		if (!hent)
			return sbe_dnserror; 
		memcpy(&saddr.sin_addr.s_addr,hent->h_addr_list[0], sizeof(saddr.sin_addr.s_addr));
	}

	if (slist->slsocket == INVALID_SOCKET)
	{
		slist->slsocket = socket ( AF_INET, SOCK_STREAM, IPPROTO_TCP );
		if (slist->slsocket == INVALID_SOCKET)
			return sbe_socketerror;
	}
	if (connect ( slist->slsocket, (struct sockaddr *) &saddr, sizeof saddr ) != 0)
	{
		closesocket(slist->slsocket);
		slist->slsocket = INVALID_SOCKET;
		return sbe_connecterror; 
	}
	
	//else we are connected
	return sbe_noerror;

}

static void BufferAddNTS(char **buffer, const char *str, int *len)
{
	int slen;
	if (str == NULL)
		str = "";
	slen = strlen(str) + 1;
	memcpy(*buffer, str, slen);
	(*len) += slen;
	(*buffer) += slen;
}

static void BufferAddByte(char **buffer, unsigned char bval, int *len)
{
	*(*buffer) = (char)bval;
	(*len) += 1;
	(*buffer) += 1;
}

static void BufferAddInt(char **buffer, int ival, int *len)
{
	memcpy(*buffer, &ival, 4);
	(*len) += 4;
	(*buffer) += 4;
}

static void BufferAddData(char **buffer, char *data, int dlen, int *len)
{
	memcpy(*buffer, data, dlen);
	(*len) += dlen;
	(*buffer) += dlen;
}



#define CALCULATEODDMODE(buffer, i, oddmode) ((buffer[i-1] & 1) ^ (i & 1) ^ oddmode ^ (buffer[0] & 1) ^ ((buffer[0] < 79) ? 1 : 0) ^ ((buffer[i-1] < buffer[0]) ? 1 : 0));
static void SetupListChallenge(SBServerList *slist)
{
	int i;
	int oddmode;

	slist->mychallenge[0] = 33 + rand() % 93; //use chars in the range 33 - 125
	oddmode = 0;
	for (i = 1; i < LIST_CHALLENGE_LEN ; i++)
	{
		oddmode = CALCULATEODDMODE(slist->mychallenge,i, oddmode);
		slist->mychallenge[i] = 33 + rand() % 93; //use chars in the range 33 - 125
		//if oddmode make sure the char is odd, otherwise make sure it's even
		if ((oddmode && (slist->mychallenge[i] & 1) == 0) || (!oddmode && ((slist->mychallenge[i] & 1) == 1)))
			slist->mychallenge[i]++;

	}
}

static SBError SendWithRetry(SBServerList *slist, char *data, int len)
{
	SBError err;
	int ret;
	
	int retryCount = 1;
	
	while (retryCount >= 0)
	{
		retryCount--;
		ret = send(slist->slsocket, data, len, 0);
		if (ret <= 0 && retryCount >= 0) //error! try to reconnect
		{
			SBServerListDisconnect(slist);
			err = SBServerListConnectAndQuery(slist, NULL, NULL, NO_SERVER_LIST, 0);
			if (err != sbe_noerror)
			{
				ErrorDisconnect(slist);
				return err; //couldn't reconnect
			}
		} else
			break; //send ok
	}
	if (ret <= 0)
		return sbe_connecterror;
	else
		return sbe_noerror;
}



/*
1 bytes - message type (0)
2 bytes - message length
Protocol Version - Byte
Requested Encoding - Byte
Query For Game - NTS
Query From Game -  NTS
Server Challenge - 8 bytes
Filter - NTS
Field List - NTS
Options - 4 bytes
Send Fields for non-NAT servers too (TurboQuery!)
No-server-slist-requested (just initiate connection and/or receive push updates)
Push Live-Updates
Log Alternate Source IP
*/
#define ALTERNATE_SOURCE_IP 8

SBError SBServerListConnectAndQuery(SBServerList *slist, const char *fieldList, const char *serverFilter, int options, int maxServers)
{
	SBError err;
	int ret;
	int requestLen;
	unsigned short netLen;
	char *requestBuf;
	char outgoingRequest[MAX_OUTGOING_REQUEST_SIZE + 1];
	assert(slist->state == sl_disconnected);

	
	if (fieldList == NULL)
		fieldList = "";
	if (serverFilter == NULL)
		serverFilter = "";
	
	if (strlen(fieldList) > MAX_FIELD_LIST_LEN)
		return sbe_paramerror;
	
	if (strlen(serverFilter) > MAX_FILTER_LEN)
		return sbe_paramerror;
	

	err = ServerListConnect(slist);
	if (err != sbe_noerror)
		return err;

	slist->queryoptions = options;

	//setup our challenge value
	SetupListChallenge(slist);

	//skip the length field for now
	requestLen = 2;
	requestBuf = outgoingRequest + 2;

	BufferAddByte(&requestBuf, SERVER_LIST_REQUEST, &requestLen);
	BufferAddByte(&requestBuf, LIST_PROTOCOL_VERSION, &requestLen);
	BufferAddByte(&requestBuf, LIST_ENCODING_VERSION, &requestLen);
	BufferAddInt(&requestBuf, slist->fromgamever, &requestLen);
	BufferAddNTS(&requestBuf, slist->queryforgamename, &requestLen);
	BufferAddNTS(&requestBuf, slist->queryfromgamename, &requestLen);
	BufferAddData(&requestBuf, slist->mychallenge, LIST_CHALLENGE_LEN, &requestLen);
	BufferAddNTS(&requestBuf, serverFilter, &requestLen);
	BufferAddNTS(&requestBuf, fieldList, &requestLen);
	options = htonl(options);
	BufferAddInt(&requestBuf, options, &requestLen); 
	if (slist->queryoptions & ALTERNATE_SOURCE_IP)
	{
		BufferAddInt(&requestBuf, slist->srcip, &requestLen); 		
	}
	if (slist->queryoptions & LIMIT_RESULT_COUNT)
	{
		BufferAddInt(&requestBuf, maxServers, &requestLen); 		
	}
	netLen = htons((unsigned short)requestLen);
	memcpy(outgoingRequest, &netLen, 2); //set the length...
	
	//now send!
	ret = send(slist->slsocket, outgoingRequest, requestLen, 0);	
	if (ret <= 0)
	{
		SBServerListDisconnect(slist);
		return sbe_connecterror;
	}
	slist->state = sl_mainlist;
	slist->pstate = pi_cryptheader;
	//allocate an incoming buffer
	if (slist->inbuffer == NULL)
	{
		slist->inbuffer = (char *)gsimalloc(INCOMING_BUFFER_SIZE);
		if (slist->inbuffer == NULL)
			return sbe_allocerror;
		slist->inbufferlen = 0;
	}
	

	return sbe_noerror;
}


SBError SBServerListGetLANList(SBServerList *slist, unsigned short startport, unsigned short endport, int queryversion)
{
	int optval = 1;
	struct   sockaddr_in saddr;
	unsigned short i;
	unsigned char qr2_echo_request[] = {QR2_MAGIC_1, QR2_MAGIC_2, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00};
	int qr2requestlen = sizeof(qr2_echo_request) / sizeof(qr2_echo_request[0]);
	if (slist->state != sl_disconnected)
		SBServerListDisconnect(slist);

	slist->slsocket = socket ( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if (slist->slsocket == INVALID_SOCKET)
		return sbe_socketerror;
	if (setsockopt(slist->slsocket, SOL_SOCKET, SO_BROADCAST, (char *)&optval, sizeof(optval)) != 0)
		return sbe_socketerror;


	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = 0xFFFFFFFF; //broadcast
	if (endport - startport > 500) //the max we will search
		endport = startport + 500;
	for (i = startport ; i <= endport ; i += 1)
	{
		saddr.sin_port = htons(i);
		if (queryversion == QVERSION_QR2) //send a QR2 echo request
			sendto(slist->slsocket, reinterpret_cast<const char*>(qr2_echo_request),qr2requestlen,0,(struct sockaddr *)&saddr,sizeof(saddr));
		else //send a GOA echo request
			sendto(slist->slsocket, "\\echo\\test",10,0,(struct sockaddr *)&saddr,sizeof(saddr));
	}
	slist->state = sl_lanbrowse;
	slist->lanstarttime = current_time();
	return sbe_noerror;

	
	
}

static void FreePopularValues(SBServerList *slist)
{
	int i;
	for (i = 0 ; i < slist->numpopularvalues ; i++)
		SBReleaseStr(slist, slist->popularvalues[i]);
	slist->numpopularvalues = 0;
}

static void FreeKeyList(SBServerList *slist)
{
	int i;
	if (slist->keylist == NULL)
		return;
	for (i = 0 ; i < ArrayLength(slist->keylist) ; i++)
		SBReleaseStr(slist, ((KeyInfo *)ArrayNth(slist->keylist,i))->keyName);

	ArrayFree(slist->keylist);
	slist->keylist = NULL;

}

void SBServerListDisconnect(SBServerList *slist)
{
	if (slist->inbuffer != NULL)
		gsifree(slist->inbuffer);
	slist->inbuffer = NULL;
	slist->inbufferlen = 0;
	if (slist->slsocket != INVALID_SOCKET)
		closesocket(slist->slsocket);
	slist->slsocket = INVALID_SOCKET;
	slist->state = sl_disconnected;
	
	FreeKeyList(slist);
	slist->expectedelements = -1;
	FreePopularValues(slist);	
}

void SBServerListCleanup(SBServerList *slist)
{
	SBServerListDisconnect(slist);
	SBServerListClear(slist);
	SBRefStrHashCleanup(slist);
	if(slist->servers)
		ArrayFree(slist->servers);
	slist->servers = NULL;
}



static void InitCryptKey(SBServerList *slist, char *key, int keylen)
{
	//combine our secret key, our challenge, and the server's challenge into a crypt key
	int i;
	int seckeylen = strlen(slist->queryfromkey);
	char *seckey = slist->queryfromkey;
	for (i = 0 ; i < keylen ; i++)
	{
		slist->mychallenge[(i *  seckey[i % seckeylen]) % LIST_CHALLENGE_LEN] ^= ((slist->mychallenge[i % LIST_CHALLENGE_LEN] ^ key[i]) & 0xFF);
	}
	GOACryptInit(&(slist->cryptkey), (unsigned char *)(slist->mychallenge), LIST_CHALLENGE_LEN);

}


static int ServerSizeForFlags(int flags)
{
	int size = 5; //all servers are at least 5 ..
	if (flags & PRIVATE_IP_FLAG)
		size += 4;
	if (flags & ICMP_IP_FLAG)
		size += 4;
	if (flags & NONSTANDARD_PORT_FLAG)
		size += 2;
	if (flags & NONSTANDARD_PRIVATE_PORT_FLAG)
		size += 2;
	return size;

}


static int FullRulesPresent(char *buf, int len)
{
	int i;
	while (len > 0 && *buf)
	{
		i = NTSLengthSB(buf, len);
		if (i < 0)
			return 0;  //no full key
		buf += i;
		len -= i;
		i = NTSLengthSB(buf, len);
		if (i < 0)
			return 0; //no full value
		buf += i;
		len -= i;		
	}
	if (len == 0)
		return 0; //not even enough space for the null term
	if (*buf == 0)
		return 1; //all there
	return 0;
}

//checks to see if all the keys for the given servers are there
static int AllKeysPresent(SBServerList *slist, char *buf, int len)
{
	int numkeys;
	int i;
	int strindex;
	numkeys = ArrayLength(slist->keylist);
	for (i = 0 ; i < numkeys ; i++)
	{
		switch (((KeyInfo *)ArrayNth(slist->keylist, i))->keyType)
		{
		case KEYTYPE_BYTE:
			buf++;
			len--;
			break;
		case KEYTYPE_SHORT:
			buf += 2;
			len -= 2;
			break;
		case KEYTYPE_STRING:
			if (len < 1)
				return 0; //not enough
			strindex = (unsigned char)(buf[0]);
			buf++;
			len--;
			if (strindex == 0xFF) //a NTS string
			{
				strindex = NTSLengthSB(buf, len);
				if (strindex == -1) //not all there..
					return 0;
				buf += strindex;
				len -= strindex;
			} //else it's a popular string - just the index is present
			break;
		default:
			assert(0);
			return 0; //error - unknown key type
		}
		if (len < 0)
			return 0; //not enough..
	}
	return 1;
}

#define LAST_SERVER_MARKER "\xFF\xFF\xFF\xFF"
#define SERVER_MARKER_LEN 4

//parse only the IP/port from the server buffer
static void ParseServerIPPort(SBServerList *slist, char *buf, int len, goa_uint32 *ip, unsigned short *port)
{
	unsigned char flags;
	if (len < 5)
		return; //invalid buffer
	flags = (unsigned char)buf[0];
	buf++;
	len--;
	memcpy(ip, buf, 4);
	
	buf += 4;
	len -= 4;
		
	if (flags & NONSTANDARD_PORT_FLAG)
	{
		if (len < 2)
			return; //invalid buffer
		memcpy(port, buf, 2);
	} else
		*port = slist->defaultport;
}




//parse a server buffer
static int ParseServer(SBServerList *slist, SBServer server, char *buf, int len, int usepopularlist)
{
	int numkeys;
	int i;
	short sval;
	int strindex;
	int holdlen = len;
	goa_uint32 ip;
	unsigned short port;
	unsigned char flags;


	flags = (unsigned char)buf[0];
	SBServerSetFlags(server, flags);
	buf++;
	len--;

	//skip the IP, it's already set
	buf += 4;
	len -= 4;
		
	//skip the port, if needed
	if (flags & NONSTANDARD_PORT_FLAG)
	{
		buf += 2;
		len -= 2;
	}

	if (flags & PRIVATE_IP_FLAG)
	{
		memcpy(&ip, buf, 4);
		buf += 4;
		len -= 4;
	} else
		ip = 0;
	if (flags & NONSTANDARD_PRIVATE_PORT_FLAG)
	{
		memcpy(&port, buf, 2);
		buf += 2;
		len -= 2;
	} else
		port = slist->defaultport;
	SBServerSetPrivateAddr(server, ip, port);
	if (flags & ICMP_IP_FLAG)
	{
		memcpy(&ip, buf, 4);
		buf += 4;
		len -= 4;
		SBServerSetICMPIP(server, ip);
	}
	
	if (flags & HAS_KEYS_FLAG) //parse the keys
	{	
		numkeys = ArrayLength(slist->keylist);
		for (i = 0 ; i < numkeys ; i++)
		{
			KeyInfo *ki = (KeyInfo *)ArrayNth(slist->keylist, i);
			switch (ki->keyType)
			{
				case KEYTYPE_BYTE:
					SBServerAddIntKeyValue(server, ki->keyName, (unsigned char)buf[0]);
					buf++;
					len--;
					break;
				case KEYTYPE_SHORT:
					memcpy(&sval, buf, 2);
					SBServerAddIntKeyValue(server, ki->keyName, ntohs(sval));
					buf += 2;
					len -= 2;
					break;
				case KEYTYPE_STRING:
					if (usepopularlist)
					{
						strindex = (unsigned char)(buf[0]);
						buf++;
						len--;
					} else
						strindex = 0xFF;
					if (strindex == 0xFF) //a NTS string
					{
						SBServerAddKeyValue(server, ki->keyName, buf);
						strindex = strlen(buf) + 1;
						buf += strindex;
						len -= strindex;
					} else //else it's a popular string - just the index is present
					{
						SBServerAddKeyValue(server, ki->keyName, slist->popularvalues[strindex]);
					}
					break;
			}
		
		}
		SBServerSetState(server, (unsigned char)(SBServerGetState(server) | STATE_BASICKEYS));
	}
	if (flags & HAS_FULL_RULES_FLAG) //got the full rules in there
	{
		while (*buf && len > 0)
		{
			char *k = buf;
			i = strlen(k) + 1;
			buf += i;
			len -= i;
			SBServerAddKeyValue(server, k, buf);
			i = strlen(buf) + 1;
			buf += i;
			len -= i;
		}
		buf++;
		len--; //get the last null out
		SBServerSetState(server, (unsigned char)(SBServerGetState(server) | STATE_FULLKEYS));
	}
	return holdlen - len;
}


static int IncomingListParseServer(SBServerList *slist, char *buf, int len)
{
	int i;
	goa_uint32 ip;
	unsigned short port;
	SBServer server;
	unsigned char flags;
	//fields depends on the flags..
	if (len < 1)
		return 0;
	flags = (unsigned char)(buf[0]);
	i = ServerSizeForFlags(flags);
	if (len < i)
		return 0;
	if (flags & HAS_KEYS_FLAG)
	{
		if (!AllKeysPresent(slist, buf + i, len - i))
			return 0;
	}
	if (flags & HAS_FULL_RULES_FLAG)
	{
		if (!FullRulesPresent(buf + i, len - i))
			return 0;
	}
	//else we have a whole server!
	//see if it's the "last" server (0xffffffff)
	if (memcmp(buf + 1, LAST_SERVER_MARKER, SERVER_MARKER_LEN) == 0)
		return -1;
	ParseServerIPPort(slist, buf, len, &ip, &port);
	server = SBAllocServer(slist, ip, port);
	if (SBIsNullServer(server))
		return -2;
	i = ParseServer(slist, server, buf, len, 1);
	SBServerListAppendServer(slist, server);
	return i;	
}

const char *SBLastListError(SBServerList *slist)
{
	return slist->lasterror;
}


#define FIXED_HEADER_LEN 6

/*
--challenge header
1 byte - number of random bytes to follow xor 0xEC
random bytes
1 byte - length xor 0xEA
keylen bytes - server challenge
--fixed header
YOUR public IP - 4 bytes
Standard query port - 2 bytes - or 0xFFFF if the master does not know about this gamename yet
[rest only follows if they requested a server slist]
--key slist
Number of Keys - 1 byte
List of Keys - with key type
Key Type - 1 Byte (0 = string, 1 = byte, 2 = short)	
KeyName - NTS
--unique value slist
number of unique string values - 1 byte
Table of Unique String Values (255 most popular string keys)
Key	NTS 
--servers
Server Table (for each server - terminated with IP of 0xFFFFFFFF, flags 0)
1 byte flags
-Connect negotiate support?
-Has private IP
-Unsolicited UDP support?
-Has ICMP IP
-Nonstandard Public Port
-Nonstandard Private Port
-Has Keys
4 bytes public ip
[optional]
4 bytes port
[optional]
4 bytes private IP
[optional]
4 bytes private port
[optional]
4 bytes ICMP IP
[optional - keys]
String values
1-bytes - value ID
or
FF + valuestring + 00
Byte Values
	1 byte - value
	Short values?
	2 bytes - value
*/		  

static SBError ProcessMainListData(SBServerList *slist)
{
	int reqlen;
	int keyoffset;
	int keylen;
	char *inbuf = slist->inbuffer;
	int inlen = slist->inbufferlen;
	switch (slist->pstate)
	{
	case pi_cryptheader:
		if (inlen < 1)
			break;
		reqlen = (((unsigned char)(inbuf[0])) ^ 0xEC) + 2;
		if (inlen < reqlen)
			break; //not there yet
		keyoffset = reqlen;
		keylen = ((unsigned char)(inbuf[reqlen - 1])) ^ 0xEA;
		reqlen += keylen;
		if (inlen < reqlen)
			break; //not there yet
		//otherwise we have the whole crypt header and can init our crypt key
		InitCryptKey(slist, inbuf + keyoffset, keylen);
		slist->pstate = pi_fixedheader;
		inbuf += reqlen;
		inlen -= reqlen;
		//decrypt any remaining data!
		GOADecrypt(&(slist->cryptkey), (unsigned char *)inbuf, inlen);
		//and fall through
	case pi_fixedheader:
		if (inlen < FIXED_HEADER_LEN)
			break;
		memcpy(&slist->mypublicip, inbuf, 4);
		slist->ListCallback(slist, slc_publicipdetermined, SBNullServer, slist->instance);
		memcpy(&slist->defaultport, inbuf + 4, 2);
		if (slist->defaultport == 0xFFFF) //there was an error-  grab the error string if present.. 
		{
			if (NTSLengthSB(inbuf + FIXED_HEADER_LEN, inlen - FIXED_HEADER_LEN) == -1) //not all there
				break;
			slist->lasterror = inbuf + FIXED_HEADER_LEN;
			//make the error callback
			slist->ListCallback(slist, slc_queryerror, SBNullServer, slist->instance);
			if (slist->inbuffer == NULL)
				break;
		}
		inbuf += FIXED_HEADER_LEN;
		inlen -= FIXED_HEADER_LEN;
		if ((slist->queryoptions & NO_SERVER_LIST) || slist->defaultport == 0xFFFF)
		{
			slist->pstate = pi_finished;
			slist->state = sl_connected;
			break; //no more data expected (if we didn't request any more, or the server doesn't know about this game)
		}
		slist->pstate = pi_keylist;
		slist->expectedelements = -1;
		
		//and fall through
	case pi_keylist:
		if (slist->expectedelements == -1) //we haven't read the initial count of keys yet..
		{
			if (inlen < 1)
				break;
			slist->expectedelements = (unsigned char)(inbuf[0]);
			slist->keylist = ArrayNew(sizeof(KeyInfo), slist->expectedelements, NULL);
			if (slist->keylist == NULL) //error
				return sbe_allocerror;
			inbuf++;
			inlen--;
		}
		//try to read elements, up to the expected amount...
		while (slist->expectedelements > ArrayLength(slist->keylist))
		{
			KeyInfo ki;
			int keylen;
			if (inlen < 2)
				break; //can't possibly be a full key (keytype + string)
			keylen = NTSLengthSB(inbuf + 1, inlen - 1);
			if (keylen == -1)
				break; //no full NTS string there
			ki.keyType = (unsigned char)(inbuf[0]);
			ki.keyName = SBRefStr(slist, inbuf + 1);
			ArrayAppend(slist->keylist, &ki);
			inbuf += keylen + 1;
			inlen -= keylen + 1;
		}
		if (slist->expectedelements > ArrayLength(slist->keylist))
			break; //didn't read them all...
		//else we have read all the keys, fall through..
		slist->pstate = pi_uniquevaluelist;
		slist->expectedelements = -1;
	case pi_uniquevaluelist:
		if (slist->expectedelements == -1) //we haven't read the # of unique values yet..
		{
			if (inlen < 1)
				break;
			slist->expectedelements = (unsigned char)(inbuf[0]);
			slist->numpopularvalues = 0;
			inbuf++;
			inlen--;
		}
		while (slist->expectedelements > slist->numpopularvalues)
		{
			int keylen = NTSLengthSB(inbuf, inlen);
			if (keylen == -1)
				break; //no full NTS string
			slist->popularvalues[slist->numpopularvalues++] = SBRefStr(slist, inbuf);
			inbuf += keylen;
			inlen -= keylen;
		}
		if (slist->expectedelements > slist->numpopularvalues)
			break; //didn't read all the popular values
		//else we've got them all - move on to servers!
		slist->pstate = pi_servers;		
	case pi_servers :
		if (inlen < 5) //not enough for a full servers
			break;
		do
		{
			reqlen =  IncomingListParseServer(slist, inbuf, inlen);
			if (reqlen == -2)
				return sbe_allocerror;			
			else if (reqlen == -1) //that was the last server!
			{
				inlen -= 5;
				inbuf += 5;
				slist->pstate = pi_finished;
				slist->state = sl_connected;
				slist->ListCallback(slist, slc_initiallistcomplete, SBNullServer, slist->instance);
				break;
			}
			inbuf += reqlen;
			inlen -= reqlen;
			if (slist->inbuffer == NULL)
				reqlen = 0; //break out - they disconnected
		} while (reqlen != 0);
		break;
	default:
		break;
	}
	assert(inlen >= 0);
	if (slist->inbuffer == NULL)
		return sbe_noerror; //don't keep processing - they disconnected
	if (inlen != 0) //need to shift it over..
	{
		memmove(slist->inbuffer, inbuf, inlen);
	}
	slist->inbufferlen = inlen;
	//we could clear the key list here if we wanted

	return sbe_noerror;
}

/*
  Number of Keys - Byte
  List of Keys - with key type
  Key Type - 1 Byte (0 = string, 1 = byte, 2 = short)	
  KeyName - NTS
*/  
static SBError ProcessPushKeyList(SBServerList *slist, char *buf, int len)
{
	int i;
	int numkeys;
	//first byte is the # of keys
	numkeys = (unsigned char)buf[0];
	buf++;
	len--;
	//set up our key list...
	if (slist->keylist != NULL) //free the existing key list
	{
		FreeKeyList(slist);
	}
	slist->keylist = ArrayNew(sizeof(KeyInfo), numkeys, NULL);
	if (slist->keylist == NULL)
		return sbe_allocerror;
	for (i = 0 ; i < numkeys ; i++)
	{
		int keylen;
		KeyInfo ki;
		if (len < 2)
			return sbe_dataerror; //can't possibly be a full key (keytype + string)
		keylen = NTSLengthSB(buf + 1, len - 1);
		if (keylen == -1)
			return sbe_dataerror; //no full NTS string there
		ki.keyType = (unsigned char)(buf[0]);
		ki.keyName = SBRefStr(slist, buf + 1);
		ArrayAppend(slist->keylist, &ki);
		buf += keylen + 1;
		len -= keylen + 1;
	}
	return sbe_noerror;	
}


static SBError ProcessDeleteServer(SBServerList *slist, char *buf, int len)
{
	goa_uint32 ip;
	unsigned short port;
	int i;

	if (len < 6)
		return sbe_dataerror;
	//need to grab out the ip & port to find it in our list
	memcpy(&ip, buf, 4);
	memcpy(&port, buf + 4, 2);
	//see if we can find the server in the list - if not, then add it
	i = SBServerListFindServerByIP(slist, ip, port);
	if (i == -1)
		return sbe_noerror;
	SBServerListRemoveAt(slist, i);
	return sbe_noerror;
}

/*
same as server list server format, except without using popular value lists
*/
static SBError ProcessPushServer(SBServerList *slist, char *buf, int len)
{

	SBServer server;
	goa_uint32 ip;
	unsigned short port;
	int foundexisting;
	int i;

	if (len < 5)
		return sbe_dataerror;
	//need to grab out the ip & port to find it in our list
	ParseServerIPPort(slist, buf, len, &ip, &port);
	
	foundexisting = SBServerListFindServerByIP(slist, ip, port);
	if (foundexisting == -1) //need to add it
	{
		server = SBAllocServer(slist, ip, port);
		if (SBIsNullServer(server))
			return sbe_allocerror;
	} else
		server = SBServerListNth(slist, foundexisting);
	i = ParseServer(slist, server, buf, len, 0);
	if (i < 0)
		return sbe_dataerror; //whole thing wasn't there
	if (foundexisting == -1)
		SBServerListAppendServer(slist, server);
	slist->ListCallback(slist, slc_serverupdated, server, slist->instance);
	return sbe_noerror;
	
}



static SBError ProcessAdHocData(SBServerList *slist)
{
	unsigned short msglen;
	int i;
	SBError ret = sbe_noerror;
	while (slist->inbufferlen >= 3) //while there is at least 1 message in there.. (2 bytes for the size + 1 for the type)
	{
		//first two bytes are the length - see if we have the whole message..
		memcpy(&msglen, slist->inbuffer, 2);
		msglen = ntohs(msglen);
		if (msglen > INCOMING_BUFFER_SIZE) //ack! won't fit..
		{
			ret = sbe_dataerror;
			break;
		}
		if (slist->inbufferlen < msglen)
			return sbe_noerror; //whole message not yet here
		//else process the message
		switch (slist->inbuffer[2])
		{
			case PUSH_KEYS_MESSAGE:
				ret = ProcessPushKeyList(slist, slist->inbuffer + 3, msglen - 3);
				break;
			case PUSH_SERVER_MESSAGE:
				ret = ProcessPushServer(slist, slist->inbuffer + 3, msglen - 3);
				break;
			case KEEPALIVE_MESSAGE: //just need to send it back..
				i = send(slist->slsocket, slist->inbuffer, msglen, 0);
				if (i <= 0)
					return sbe_connecterror;
				break;
			case DELETE_SERVER_MESSAGE:
				ret = ProcessDeleteServer(slist, slist->inbuffer + 3, msglen - 3);
				break;
		}
		
		slist->inbufferlen -= msglen;
		assert(slist->inbufferlen >= 0);
		if (slist->inbufferlen != 0 && slist->inbuffer != NULL) //if anything is left, shift it over..
		{
			memmove(slist->inbuffer, slist->inbuffer + msglen, slist->inbufferlen);
		}
		if (ret != sbe_noerror)
			break;
	}
	if (ret != sbe_noerror)
		ErrorDisconnect(slist);
	return ret;
}


static SBError ProcessIncomingData(SBServerList *slist)
{
	SBError err;
	int len;
	int oldlen;

	if(!CanReceiveOnSocket(slist->slsocket))
		return sbe_noerror;
	
	//append to data
	oldlen = slist->inbufferlen;
	len = recv(slist->slsocket, slist->inbuffer + slist->inbufferlen, INCOMING_BUFFER_SIZE - slist->inbufferlen, 0);
	if (len == SOCKET_ERROR || len == 0)
	{
		ErrorDisconnect(slist);
		return sbe_connecterror;		
	}
	slist->inbufferlen += len;
	err = sbe_noerror;
	if (slist->state == sl_connected || slist->pstate > pi_cryptheader) //decrypt any new data..
	{
		GOADecrypt(&(slist->cryptkey), (unsigned char *)(slist->inbuffer + oldlen), slist->inbufferlen - oldlen);
	}
	
	if (slist->state == sl_mainlist)
		err = ProcessMainListData(slist);
	if (err != sbe_noerror)
		return err;
	//always need to check this after mainlistdata, in case some extra data has some in (e.g. key list for push)
	if (slist->state == sl_connected && slist->inbufferlen > 0)	
		return ProcessAdHocData(slist);
	return sbe_noerror;

}

SBError SBGetServerRulesFromMaster(SBServerList *slist, goa_uint32 ip, unsigned short port)
{
	char requestBuffer[16];
	unsigned short tmp;
	unsigned short msgLen = 9;
	
	if (slist->state == sl_disconnected) //try to connect
		SBServerListConnectAndQuery(slist, NULL, NULL, NO_SERVER_LIST, 0); 

	if (slist->state == sl_disconnected)
		return sbe_connecterror;

	//the length
	tmp = htons(msgLen);
	memcpy(requestBuffer, &tmp, 2);
	//the message type
	requestBuffer[2] = SERVER_INFO_REQUEST;
	memcpy(requestBuffer + 3, &ip, sizeof(ip));
	memcpy(requestBuffer + 7, &port, sizeof(port));	
	return SendWithRetry(slist, requestBuffer, msgLen);	
}

SBError SBSendMessageToServer(SBServerList *slist, goa_uint32 ip, unsigned short port, char *data, int len)
{
	char requestBuffer[16];
	unsigned short tmp;
	unsigned short msgLen = 9;
	SBError ret;
	int i;
	
	if (slist->state == sl_disconnected) //try to connect
		SBServerListConnectAndQuery(slist, NULL, NULL, NO_SERVER_LIST, 0); 

	if (slist->state == sl_disconnected)
		return sbe_connecterror;

	//the length
	tmp = htons((unsigned short)(msgLen + len));
	memcpy(requestBuffer, &tmp, 2);
	//the message type
	requestBuffer[2] = SEND_MESSAGE_REQUEST;
	memcpy(requestBuffer + 3, &ip, sizeof(ip));
	memcpy(requestBuffer + 7, &port, sizeof(port));	
	ret = SendWithRetry(slist, requestBuffer, msgLen);
	if (ret != sbe_noerror)
		return ret;
	i = send(slist->slsocket, data, len, 0); //send the actual push data
	if (i < 0)
		return sbe_connecterror;
	return sbe_noerror;
	
}

SBError SBSendNatNegotiateCookieToServer(SBServerList *slist, goa_uint32 ip, unsigned short port, int cookie)
{
	unsigned char negotiateBuffer[NATNEG_MAGIC_LEN + 4];
	negotiateBuffer[0] = NN_MAGIC_0;
	negotiateBuffer[1] = NN_MAGIC_1;
	negotiateBuffer[2] = NN_MAGIC_2;
	negotiateBuffer[3] = NN_MAGIC_3;
	negotiateBuffer[4] = NN_MAGIC_4;
	negotiateBuffer[5] = NN_MAGIC_5;
	cookie = (int)htonl(cookie);
	memcpy(negotiateBuffer + NATNEG_MAGIC_LEN, &cookie, 4);
	return SBSendMessageToServer(slist, ip, port, reinterpret_cast<char*>(negotiateBuffer), NATNEG_MAGIC_LEN + 4);
}

static SBError ProcessLanData(SBServerList *slist)
{
	char indata[500];
	struct   sockaddr_in saddr;
	int saddrlen = sizeof(saddr);
	int error;
	int foundexisting;
	SBServer server;

	while (1) //we break if the select fails
	{
		if(!CanReceiveOnSocket(slist->slsocket))
			break;
		error = recvfrom(slist->slsocket, indata, sizeof(indata) - 1, 0, (struct sockaddr *)&saddr, &saddrlen );
		if (SOCKET_ERROR == error)
			continue; 
		//if we got data, then add it to the list...
		foundexisting = SBServerListFindServerByIP(slist, saddr.sin_addr.s_addr, saddr.sin_port);
		if (foundexisting != -1) //already exists
			continue;
		server = SBAllocServer(slist, saddr.sin_addr.s_addr, saddr.sin_port);
		if (SBIsNullServer(server))
			return sbe_allocerror;
		SBServerSetFlags(server, UNSOLICITED_UDP_FLAG|NONSTANDARD_PORT_FLAG);
		SBServerListAppendServer(slist, server);
	}
	if (current_time() - slist->lanstarttime > SL_LAN_SEARCH_TIME) //done waiting for replies
	{
		closesocket(slist->slsocket);
		slist->slsocket = INVALID_SOCKET;
		slist->state = sl_disconnected;
	}
	return sbe_noerror;
}

SBError SBListThink(SBServerList *slist)
{
	SBFreeDeadList(slist); //free any pending deleted servers
	switch (slist->state)
	{
	case sl_disconnected:
		break;
	case sl_connected:
	case sl_mainlist:
		return ProcessIncomingData(slist);
		break;
	case sl_lanbrowse:
		return ProcessLanData(slist);
		break;
	}

	return sbe_noerror;
	//see if any data is available...
	
}







#ifndef SB_SERVERLIST_OVERRIDE

static SBServerList *g_sortserverlist; //global serverlist for sorting info!!

/****
Comparision Functions
***/
static int IntKeyCompare(const void *entry1, const void *entry2)
{
	SBServer server1 = *(SBServer *)entry1, server2 = *(SBServer *)entry2;
	int diff;
	diff = SBServerGetIntValue(server1, g_sortserverlist->sortkey, 0) - 
			SBServerGetIntValue(server2, g_sortserverlist->sortkey, 0);
	if (!g_sortserverlist->sortascending) 
		diff = -diff;
	return diff;
	
}

static int FloatKeyCompare(const void *entry1, const void *entry2)
{
    SBServer server1 = *(SBServer *)entry1, server2 = *(SBServer *)entry2;
	double f = SBServerGetFloatValue(server1, g_sortserverlist->sortkey, 0) - 
			SBServerGetFloatValue(server2, g_sortserverlist->sortkey, 0);
	if (!g_sortserverlist->sortascending) 
		f = -f;
	if ((float)f > (float)0.0) 
		return 1;
	else if ((float)f < (float)0.0) 
		return -1; 
	else 
		return 0;
}

static int StrCaseKeyCompare(const void *entry1, const void *entry2)
{
	
    SBServer server1 = *(SBServer *)entry1, server2 = *(SBServer *)entry2;
    int diff = strcmp(SBServerGetStringValue(server1, g_sortserverlist->sortkey, ""),
				SBServerGetStringValue(server2, g_sortserverlist->sortkey, ""));
	if (!g_sortserverlist->sortascending) 
		diff = -diff;
	return diff;
}

static int StrNoCaseKeyCompare(const void *entry1, const void *entry2)
{
	SBServer server1 = *(SBServer *)entry1, server2 = *(SBServer *)entry2;
	int diff = strcasecmp(SBServerGetStringValue(server1, g_sortserverlist->sortkey, ""),
				SBServerGetStringValue(server2, g_sortserverlist->sortkey, ""));
	if (!g_sortserverlist->sortascending) 
		diff = -diff;
	return diff;
}


/* ServerListSort
-----------------
Sort the server list in either ascending or descending order using the 
specified comparemode.
sortkey can be a normal server key, or "ping" or "hostaddr" */
void SBServerListSort(SBServerList *slist, SBBool ascending, char *sortkey, SBCompareMode comparemode)
{
	ArrayCompareFn comparator;
	switch (comparemode)
	{
	case sbcm_int: comparator = IntKeyCompare;
		break;
	case sbcm_float: comparator = FloatKeyCompare;
		break;
	case sbcm_strcase: comparator = StrCaseKeyCompare;
		break;
	case sbcm_stricase: comparator = StrNoCaseKeyCompare;
		break;
	default: 
		comparator = StrNoCaseKeyCompare;
	}
	slist->sortkey = sortkey;
	slist->sortascending = ascending;
	g_sortserverlist = slist;
	ArraySort(slist->servers,comparator);
}



 
void SBServerListAppendServer(SBServerList *slist, SBServer server)
{
	ArrayAppend(slist->servers, &server);
	slist->ListCallback(slist, slc_serveradded, server, slist->instance);
	
}



int SBServerListFindServer(SBServerList *slist, SBServer findserver)
{
	int numservers;
	int i;
	numservers = ArrayLength(slist->servers);
	for (i = 0 ; i < numservers ; i++)
	{
		if (findserver == *(SBServer *)ArrayNth(slist->servers, i))
		{
			return i;
		}
	}
	return -1;
}

int SBServerListFindServerByIP(SBServerList *slist, goa_uint32 ip, unsigned short port)
{
	int numservers;
	SBServer server;
	int i;
	numservers = ArrayLength(slist->servers);
	for (i = 0 ; i < numservers ; i++)
	{
		server = *(SBServer *)ArrayNth(slist->servers, i);
		if (SBServerGetPublicInetAddress(server) == ip && SBServerGetPublicQueryPortNBO(server) == port)
		{
			return i;
		}
	}
	return -1;
}

//add to the singly linked list of dead servers
static void AddServerToDeadlist(SBServerList *slist, SBServer server)
{
	if (slist->deadlist == NULL)
	{
		SBServerSetNext(server, NULL);
	} else
		SBServerSetNext(server, slist->deadlist);
	slist->deadlist = server;	
}


void SBServerListRemoveAt(SBServerList *slist, int index)
{
	SBServer server = *(SBServer *)ArrayNth(slist->servers, index);
	slist->ListCallback(slist, slc_serverdeleted, server, slist->instance);
	//need to remove it...
	ArrayDeleteAt(slist->servers, index);	
	//now add it to the dead list
	AddServerToDeadlist(slist, server);
}

int SBServerListCount(SBServerList *slist)
{
	return ArrayLength(slist->servers);
}

SBServer SBServerListNth(SBServerList *slist, int i)
{
	return *(SBServer *)ArrayNth(slist->servers, i);
}


void SBFreeDeadList(SBServerList *slist)
{
	SBServer server, next;
	if (slist->deadlist == NULL)
		return;
	server = slist->deadlist;
	while (server != NULL)
	{
		next = SBServerGetNext(server);
		SBServerFree(&server);
		server = next;
	}
	slist->deadlist = NULL;
}


void SBServerListClear(SBServerList *slist)
{
	//we need to add each server to the dead list so it can be freed after the clear
	int i;
	int nservers = ArrayLength(slist->servers);
	for (i = 0 ; i < nservers ; i++)
	{
		AddServerToDeadlist(slist, *(SBServer *)ArrayNth(slist->servers, i));
	}
	ArrayClear(slist->servers);
	//now free the dead list
	SBFreeDeadList(slist);
}

void SBAllocateServerList(SBServerList *slist)
{
	slist->servers = ArrayNew(sizeof(SBServer ), SERVER_GROWBY, NULL); //don't free the server automatically - it goes into the dead list
	slist->deadlist = NULL;
}



const char *SBRefStr(SBServerList *slist, const char *str)
{
	SBRefString ref, *val;
	ref.str = str;
	val = (SBRefString *)TableLookup(SBRefStrHash(slist), &ref);
	if (val != NULL)
	{
		val->refcount++;
		return val->str;
	}
	//else we need to add.
	ref.str = goastrdup(str);
	ref.refcount = 1;
	TableEnter(SBRefStrHash(slist), &ref);
	return ref.str;
	
	
}

void SBReleaseStr(SBServerList *slist, const char *str)
{
	SBRefString ref, *val;
	ref.str = str;
	val = (SBRefString *)TableLookup(SBRefStrHash(slist), &ref);
	assert(val != NULL);
	if (val == NULL)
		return; //not found!
	val->refcount--;
	if (val->refcount == 0)
		TableRemove(SBRefStrHash(slist), &ref);
}


#endif
