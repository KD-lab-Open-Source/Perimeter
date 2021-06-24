/*
GameSpy Peer SDK 
Dan "Mr. Pants" Schoenblum
dan@gamespy.com

Copyright 1999-2001 GameSpy Industries, Inc

18002 Skypark Circle
Irvine, California 92614
949.798.4200 (Tel)
949.798.4299 (Fax)
devsupport@gamespy.com
*/

#ifndef _PEERPING_H_
#define _PEERPING_H_

/*************
** INCLUDES **
*************/
#include "peerMain.h"

/**************
** FUNCTIONS **
**************/
PEERBool piPingInit(PEER peer);
void piPingCleanup(PEER peer);
void piPingThink(PEER peer);
PEERBool piPingInitPlayer(PEER peer, piPlayer * player);
void piPingPlayerJoinedRoom(PEER peer, piPlayer * player, RoomType roomType);
void piPingPlayerLeftRoom(PEER peer, piPlayer * player);
void piUpdateXping(PEER peer, const char * nick1, const char * nick2, int ping);
PEERBool piGetXping(PEER peer, const char * nick1, const char * nick2, int * ping);

#endif
