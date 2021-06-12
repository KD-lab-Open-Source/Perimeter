#include "sb_internal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>



//for the unique value list
#define LIST_NUMKEYBUCKETS 500
#define LIST_NUMKEYCHAINS 4

//global, shared unique value list
#if defined(_WIN32) && !defined(_DLL) && !defined(_MANAGED)
THREAD_LOCAL
#endif
HashTable g_SBRefStrList = NULL;

/***********
 * REF COUNTING STRING HASHTABLE FUNCTIONS
 **********/
static int StringHash(const char *s, int numbuckets);
static int RefStringHash(const void *elem, int numbuckets)
{
	return StringHash(((SBRefString *)elem)->str, numbuckets);
}



/* keyval
* Compares two gkeyvaluepair 
*/

static int RefStringCompare(const void *entry1, const void *entry2)
{
   	return strcasecmp(((SBRefString *)entry1)->str,((SBRefString *)entry2)->str);
}


static void RefStringFree(void *elem)
{
	gsifree((char *)((SBRefString *)elem)->str);
}


HashTable SBRefStrHash(SBServerList *slist)
{
	if (g_SBRefStrList == NULL)
		g_SBRefStrList = TableNew2(sizeof(SBRefString),LIST_NUMKEYBUCKETS,LIST_NUMKEYCHAINS,RefStringHash, RefStringCompare, RefStringFree);
	return g_SBRefStrList;
	
	GSI_UNUSED(slist);
}

void SBRefStrHashCleanup(SBServerList *slist)
{
	if (g_SBRefStrList != NULL && TableCount(g_SBRefStrList) == 0)
	{
		TableFree(g_SBRefStrList);
		g_SBRefStrList = NULL;
	}

	GSI_UNUSED(slist);
}


void SBServerFree(void *elem)
{
	SBServer server = *(SBServer *)elem;
	//free all the keys..
	TableFree(server->keyvals);
	server->keyvals = NULL;
	gsifree(server);
}

void SBServerAddKeyValue(SBServer server, const char *keyname, const char *value)
{
	SBKeyValuePair kv;
	kv.key = SBRefStr(NULL, keyname);
	kv.value = SBRefStr(NULL, value);
	TableEnter(server->keyvals, &kv);
}

void SBServerAddIntKeyValue(SBServer server, const char *keyname, int value)
{
	char stemp[20];
	sprintf(stemp, "%d", value);
	SBServerAddKeyValue(server, keyname, stemp);
}

typedef struct 
{
	SBServerKeyEnumFn EnumFn;
	void *instance;
} SBServerEnumData;

/* ServerEnumKeys 
-----------------
Enumerates the keys/values for a given server by calling KeyEnumFn with each
key/value. The user-defined instance data will be passed to the KeyFn callback */

static void KeyMapF(void *elem, void *clientData)
{
	SBKeyValuePair *kv = (SBKeyValuePair *)elem;
	SBServerEnumData *ped = (SBServerEnumData *)clientData;
	ped->EnumFn((char *)kv->key, (char *)kv->value, ped->instance);
}
void SBServerEnumKeys(SBServer server, SBServerKeyEnumFn KeyFn, void *instance)
{
	SBServerEnumData ed;

	ed.EnumFn = KeyFn;
	ed.instance = instance;
	TableMap(server->keyvals, KeyMapF, &ed);
}


const char *SBServerGetStringValue(SBServer server, char *keyname, char *def)
{
	SBKeyValuePair kv, *ptr;
	
	kv.key = keyname;
	ptr =  (SBKeyValuePair *)TableLookup(server->keyvals, &kv);
	if (ptr == NULL)
		return def;
	return ptr->value;
	
}

int SBServerGetIntValue(SBServer server, char *key, int idefault)
{
	const char *s;

	if (strcmp(key,"ping") == 0) //ooh! they want the ping!
		return SBServerGetPing(server);
	s = SBServerGetStringValue(server, key, NULL);
	if (s == NULL)
		return idefault;
	else
		return atoi(s);
}

double SBServerGetFloatValue(SBServer server, char *key, double fdefault)
{
	const char *s;
	s = SBServerGetStringValue(server, key, NULL);
	if (s == NULL)
		return fdefault;
	else
		return atof(s);
}

SBBool SBServerGetBoolValue(SBServer server, char *key, SBBool bdefault)
{
	const char *s;
	s = SBServerGetStringValue(server, key, NULL);
	if (!s)
		return bdefault;

	// check the first char for known "false" values
	if('0' == s[0]|| 'F' == s[0] || 'f' == s[0] || 'N' == s[0] || 'n' == s[0])
		return SBFalse;

	// presume that all other non-zero values are "true"
	return SBTrue;
}

const char *SBServerGetPlayerStringValue(SBServer server, int playernum, char *key, char *sdefault)
{
	char keyname[128];
	sprintf(keyname, "%s_%d", key, playernum);
	return SBServerGetStringValue(server, keyname, sdefault);
}
int SBServerGetPlayerIntValue(SBServer server, int playernum, char *key, int idefault)
{
	char keyname[128];
	sprintf(keyname, "%s_%d", key, playernum);
	return SBServerGetIntValue(server, keyname, idefault);
}
double SBServerGetPlayerFloatValue(SBServer server, int playernum, char *key, double fdefault)
{
	char keyname[128];
	sprintf(keyname, "%s_%d", key, playernum);
	return SBServerGetFloatValue(server, keyname, fdefault);
}

const char *SBServerGetTeamStringValue(SBServer server, int teamnum, char *key, char *sdefault)
{
	char keyname[128];
	sprintf(keyname, "%s_t%d", key, teamnum);
	return SBServerGetStringValue(server, keyname, sdefault);
}
int SBServerGetTeamIntValue(SBServer server, int teamnum, char *key, int idefault)
{
	char keyname[128];
	sprintf(keyname, "%s_t%d", key, teamnum);
	return SBServerGetIntValue(server, keyname, idefault);
}
double SBServerGetTeamFloatValue(SBServer server, int teamnum, char *key, double fdefault)
{
	char keyname[128];
	sprintf(keyname, "%s_t%d", key, teamnum);
	return SBServerGetFloatValue(server, keyname, fdefault);
}


SBBool SBServerHasBasicKeys(SBServer server)
{
	return (server->state & STATE_BASICKEYS);
}

SBBool SBServerHasFullKeys(SBServer server)
{
	return (server->state & STATE_FULLKEYS);
}



char *SBServerGetPublicAddress(SBServer server)
{
	return inet_ntoa(*(struct in_addr *)&server->publicip);
}

unsigned int SBServerGetPublicInetAddress(SBServer server)
{
	return server->publicip;
}


unsigned short SBServerGetPublicQueryPort(SBServer server)
{
	return ntohs(server->publicport);
}

unsigned short SBServerGetPublicQueryPortNBO(SBServer server)
{
	return server->publicport;
}

SBBool SBServerHasPrivateAddress(SBServer server)
{
	return (server->flags & PRIVATE_IP_FLAG);
}



SBBool SBServerDirectConnect(SBServer server)
{
	return (server->flags & UNSOLICITED_UDP_FLAG);
}

char *SBServerGetPrivateAddress(SBServer server)
{
	return inet_ntoa(*(struct in_addr *)&server->privateip);
}

unsigned int SBServerGetPrivateInetAddress(SBServer server)
{
	return server->privateip;
}


unsigned short SBServerGetPrivateQueryPort(SBServer server)
{
	return ntohs(server->privateport);
}

void SBServerSetNext(SBServer server, SBServer next)
{
	server->next = next;
}

SBServer SBServerGetNext(SBServer server)
{
	return server->next;
}

static int CheckValidKey(char *key)
{
	const char *InvalidKeys[]={"queryid","final"};
	int i;
	for (i = 0; i < sizeof(InvalidKeys)/sizeof(InvalidKeys[0]); i++)
	{
		if (strcmp(key,InvalidKeys[i]) == 0)
			return 0;
	}
	return 1;
}

static char *mytok(char *instr, char delim)
{
	char *result;
	static char *thestr;
	
	if (instr)
		thestr = instr;
	result=thestr;
	while (*thestr && *thestr != delim)
	{
		thestr++;
	}
	if (thestr == result)
		result = NULL;
	if (*thestr) //not the null term
		*thestr++ = '\0';
	return result;
}


void SBServerParseKeyVals(SBServer server, char *keyvals)
{
	char *k, *v;
	
	k = mytok(++keyvals,'\\'); //skip over starting backslash
	while (k != NULL)
	{
		v = mytok(NULL,'\\');
		if (v == NULL)
			v = "";
		if (CheckValidKey(k))
			SBServerAddKeyValue(server, k, v);
		k = mytok(NULL,'\\');		
	}
}


/*
Query Response Format
Packet Type: 1 Byte
Request Key: 4 Bytes (copied from request packet)

Server Keys (if number of keys requested > 0)
If server key list specified:
Server Values: NTS, one per key specified
If server key list not specified:
Server Keys / Server Values: NTS Pairs, terminated with NUL
	
Player Keys (if number of keys requested > 0)
Number of Players: 2 Bytes
If player key list NOT specified
	Player Keys: NTS, one per key, terminated with NUL
Player Values: NTS, one per key specified, per player

Team Keys (if number of keys requested > 0)
Number of Teams: 2 Bytes
If team key list NOT specified
	Team Keys: NTS, one per key, terminated with NUL
Team Values: NTS, one per key specified, per team

*/
void SBServerParseQR2FullKeys(SBServer server, char *data, int len)
{
	int dlen;
	char *k;
	char *v;
	char *keys;
	int nkeys;
	unsigned short nunits;
	int pflag;
	int i,j;
	char tempkey[128];
	//first pull out all the server keys/values
	while (*data)
	{
		dlen = NTSLengthSB(data, len);
		if (dlen < 0)
			return; //not a full NTS
		k = data;
		data += dlen;
		len -= dlen;
		dlen = NTSLengthSB(data, len);
		if (dlen < 0)
			return; //not a full NTS
		v = data;
		data += dlen;
		len -= dlen;		
		SBServerAddKeyValue(server, k, v);		
	}
	//skip the NUL
	data++;
	len--;
	//now get out the # of players (or teams) .. we do this whole thing 2X, once for players once for teams
	for (pflag = 0 ; pflag < 2 ; pflag++)
	{
		if (len < 2)
			return;
		memcpy(&nunits, data, 2);
		nunits = ntohs(nunits);
		data += 2;
		len -= 2;
		keys = data;
		nkeys = 0;
		//count up the number of keys..
		while (*data)
		{
			dlen = NTSLengthSB(data, len);
			if (dlen < 0)
				return; //not all there
			if (dlen > 100) //key is too long, may cause buffer overrun
				return;
			nkeys++;
			data += dlen;
			len -= dlen;
		}
		//skip the NUL
		data++;
		len--;
		//now for each player/team
		for (i = 0 ; i < nunits ; i++)
		{
			k = keys;
			//for each key..
			for (j = 0 ; j < nkeys ; j++)
			{
				dlen = NTSLengthSB(data, len);
				if (dlen < 0)
					return; //not all there
				sprintf(tempkey, "%s%d", k, i);
				SBServerAddKeyValue(server, tempkey, data);
				data += dlen;
				len -= dlen;
				k += strlen(k) + 1; //skip to the next key
			}
		}		
	}

	
}

/***********
 * UTILITY FUNCTIONS
 **********/
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

static void KeyValFree(void *elem)
{
	SBKeyValuePair *kv = (SBKeyValuePair *)elem;
	SBReleaseStr(NULL, kv->key);
	SBReleaseStr(NULL, kv->value);
}

static int KeyValHashKey(const void *elem, int numbuckets)
{
	return StringHash(((SBKeyValuePair *)elem)->key, numbuckets);
}


static int KeyValCompareKey(const void *entry1, const void *entry2)
{
   	return strcasecmp(((SBKeyValuePair *)entry1)->key, ((SBKeyValuePair *)entry2)->key);
}


int SBServerGetPing(SBServer server)
{
	return (int) server->updatetime;
}

#define NUM_BUCKETS 8
#define NUM_CHAINS 4

//todo: benchmark sorted darray vs. hashtable - memory + speed
SBServer SBAllocServer(SBServerList *slist, goa_uint32 publicip, unsigned short publicport)
{
	SBServer server;
	server = (SBServer)gsimalloc(sizeof(struct _SBServer));
	if (server == NULL)
		return NULL;
	server->keyvals = TableNew2(sizeof(SBKeyValuePair),NUM_BUCKETS,NUM_CHAINS, KeyValHashKey, KeyValCompareKey, KeyValFree);
	if (server->keyvals == NULL)
	{
		gsifree(server);
		return NULL;
	}
	server->state = 0;
	server->flags = 0;
	server->next = NULL;
	server->updatetime = 0;
	server->icmpip = 0;
	server->publicip = publicip;
	server->publicport = publicport;
	server->privateip = 0;
	server->privateport = 0;
	return server;
	
	GSI_UNUSED(slist);
}



void SBServerSetFlags(SBServer server, unsigned char flags)
{
	server->flags = flags;
}
void SBServerSetPublicAddr(SBServer server, goa_uint32 ip, unsigned short port)
{
	server->publicip = ip;
	server->publicport = port;

}
void SBServerSetPrivateAddr(SBServer server, goa_uint32 ip, unsigned short port)
{
	server->privateip = ip;
	server->privateport = port;

}
void SBServerSetICMPIP(SBServer server, goa_uint32 icmpip)
{
	server->icmpip = icmpip;

}
void SBServerSetState(SBServer server, unsigned char state)
{
	server->state = state;

}
unsigned char SBServerGetState(SBServer server)
{
	return server->state;

}
unsigned char SBServerGetFlags(SBServer server)
{
	return server->flags;

}

int SBIsNullServer(SBServer server)
{
	return (server == SBNullServer) ? 1 : 0;
}

SBServer SBNullServer = NULL;
