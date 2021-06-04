/******
querytest.c
GameSpy Server Browsing SDK
  
Copyright 1999-2002 GameSpy Industries, Inc

18002 Skypark Circle
Irvine, California 92614
949.798.4200 (Tel)
949.798.4299 (Fax)
devsupport@gamespy.com

******

This program is provided as a diagnostic tool for the Query and Reporting 2 SDK, but is
also provided here as a sample to demonstrate how to query a single server for full rules using
the Server Browisng SDK.

******/
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include "../sb_serverbrowsing.h"
#include "../sb_internal.h"
#include "../../qr2/qr2.h"
#include <stdlib.h>


#pragma comment (lib, "wsock32")


#define NUM_SIMUL_QUERIES 10

void PrintValues(char *key, char *value, void *instance)
{
	printf("%s = %s\n", key, value);
}


void PrintResultsCallback(ServerBrowser sb, SBCallbackReason reason, SBServer server, void *instance)
{
	if (reason == sbc_serverupdated)
	{
		printf("Got server data: %s:%d [%d]\nKeys/Values:\n",inet_ntoa(*(struct in_addr *)&server->publicip), ntohs(server->publicport), SBServerGetPing(server));
		SBServerEnumKeys(server, PrintValues, NULL);

	} else if (reason == sbc_serverupdatefailed)
	{
		printf("Server update timed out\n");
	}
	
}

#if defined(_PS2)
int test_main(int argc, char **argp)
#else
int main(int argc, char **argp)
#endif
{
	ServerBrowser sb;

	if (argc != 3)
	{
		printf("Invalid parameters!\nQueryTest Usage:\nquerytest.exe [ip] [port]\nExample: querytest.exe 1.2.3.4 28000\n");
		return 1;
	}
	sb = ServerBrowserNew ("gmtest", "gmtest", "HA6zkS", 0, 10, QVERSION_QR2, PrintResultsCallback, NULL);
	ServerBrowserAuxUpdateIP(sb, argp[1], (unsigned short)atoi(argp[2]), SBFalse, SBFalse, SBTrue);
	ServerBrowserFree(sb);
	return 0;
}
