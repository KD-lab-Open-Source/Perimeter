/*
GameSpy Peer SDK 
Dan "Mr. Pants" Schoenblum
dan@gamespy.com

Copyright 1999-2002 GameSpy Industries, Inc

18002 Skypark Circle
Irvine, California 92614
949.798.4200 (Tel)
949.798.4299 (Fax)
devsupport@gamespy.com
*/

#ifndef _PEERSB_H_
#define _PEERSB_H_

/*************
** INCLUDES **
*************/
#include "peerMain.h"

/**************
** FUNCTIONS **
**************/
PEERBool piSBInit(PEER peer);
void piSBCleanup(PEER peer);
PEERBool piSBStartListingGames(PEER peer, const unsigned char * fields, int numFields, const char * filter);
void piSBStopListingGames(PEER peer);
void piSBUpdateGame(PEER peer, SBServer server, PEERBool fullUpdate);
PEERBool piSBStartListingGroups(PEER peer, const char * fields);
void piSBStopListingGroups(PEER peer);
void piSBThink(PEER peer);
void piSendNatNegotiateCookie(PEER peer, unsigned int ip, unsigned short port, int cookie);
PEERBool piSBStartListingAutoMatches(PEER peer);
void piSBStopListingAutoMatches(PEER peer);
SBServer piSBCloneServer(SBServer server);
void piSBFreeHostServer(PEER peer);

#endif
