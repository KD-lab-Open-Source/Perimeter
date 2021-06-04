/******
sbctest.c
GameSpy Server Browsing SDK
  
Copyright 1999-2002 GameSpy Industries, Inc

18002 Skypark Circle
Irvine, California 92614
949.798.4200 (Tel)
949.798.4299 (Fax)
devsupport@gamespy.com

******

 Please see the GameSpy Server Browsing SDK documentation for more 
 information

******/
#include <stdio.h>
#include <string.h>
#include "../sb_serverbrowsing.h"
#include "../../qr2/qr2.h"
#include <stdlib.h>

#ifndef _PS2	// PS2 will truncate file if it seems an unsupported pragma
#pragma comment (lib, "wsock32")
#endif



#define NUM_SIMUL_QUERIES 30


static void SBCallback(ServerBrowser sb, SBCallbackReason reason, SBServer server, void *instance)
{
	int updatePercent = 0;
	switch (reason)
	{
	case sbc_serveradded:
		printf("Server Added: %s:%d\n", SBServerGetPublicAddress(server), SBServerGetPublicQueryPort(server));
		break;
	case sbc_serverupdated:
		//determine our update percent
		if (ServerBrowserCount(sb) > 0)
			updatePercent = (ServerBrowserCount(sb) - ServerBrowserPendingQueryCount(sb)) * 100 / ServerBrowserCount(sb);
		printf("Server Updated: %s:%d [%d] - %s - %d/%d (%d%%)\n", SBServerGetPublicAddress(server), SBServerGetPublicQueryPort(server), SBServerGetPing(server),
			SBServerGetStringValue(server, "hostname", ""), SBServerGetIntValue(server, "numplayers", 0), SBServerGetIntValue(server, "maxplayers", 0), updatePercent);
		break;
	case sbc_serverupdatefailed:
		printf("Update Failed: %s:%d\n", SBServerGetPublicAddress(server), SBServerGetPublicQueryPort(server));
		break;
	case sbc_updatecomplete:
		printf("Server Browser Update Complete\n");
		break;
	case sbc_queryerror:
		printf("Query Error: %s\n", ServerBrowserListQueryError(sb));
		break;
	default:
		break;
	}

	GSI_UNUSED(instance);
}


#if defined(_PS2)
	#ifdef __MWERKS__ // CodeWarrior will warn if not prototyped
		int test_main(int argc, char **argp);
	#endif	
int test_main(int argc, char **argp)
#else
int main(int argc, char **argp)
#endif // _PS2
{

	ServerBrowser sb;
	unsigned char basicFields[] = {HOSTNAME_KEY, GAMETYPE_KEY,  MAPNAME_KEY, NUMPLAYERS_KEY, MAXPLAYERS_KEY};
	int numFields = sizeof(basicFields) / sizeof(basicFields[0]);


	//create a new server browser object
	sb = ServerBrowserNew ("ut", "gmtest", "HA6zkS", 0, 40, QVERSION_GOA, SBCallback, NULL);

	//begin the update (async)
	ServerBrowserUpdate(sb, SBTrue, SBTrue, basicFields, numFields, NULL);
	
	while (1)
	{
		//think while the update is in progress
		ServerBrowserThink(sb);
		msleep(10);
	}
	
	ServerBrowserFree(sb); //how to clean up, if we were to actually get here

	return 0;
	
	GSI_UNUSED(argc);
	GSI_UNUSED(argp);
}
