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

#ifndef _PEERAUTOMATCH_H_
#define _PEERAUTOMATCH_H_

/*************
** INCLUDES **
*************/
#include "peerMain.h"

/************
** DEFINES **
************/
#define PI_AUTOMATCH_RATING_KEY "gsi_am_rating"

/**************
** FUNCTIONS **
**************/
void piSetAutoMatchStatus(PEER peer, PEERAutoMatchStatus status);
void piStopAutoMatch(PEER peer);
PEERBool piJoinAutoMatchRoom(PEER peer, SBServer server);

#endif
