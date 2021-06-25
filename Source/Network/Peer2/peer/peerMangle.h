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

#ifndef _PEERMANGLE_H_
#define _PEERMANGLE_H_

/*************
** INCLUDES **
*************/
#include "peer.h"
#include "peerMain.h"

/************
** DEFINES **
************/
#define PI_USER_MAX_LEN        128

/**************
** FUNCTIONS **
**************/
void piMangleTitleRoom(char buffer[PI_ROOM_MAX_LEN], const char * title);
void piMangleGroupRoom(char buffer[PI_ROOM_MAX_LEN], int groupID);
void piMangleStagingRoom(char buffer[PI_ROOM_MAX_LEN], const char * title,
                         unsigned int publicIP, unsigned int privateIP, unsigned short privatePort);
void piMangleUser(char buffer[PI_USER_MAX_LEN], unsigned int IP, int profileID);
PEERBool piDemangleUser(const char buffer[PI_USER_MAX_LEN], unsigned int * IP, int * profileID);
void piMangleIP(char buffer[11], unsigned int IP);
unsigned int piDemangleIP(const char buffer[11]);

#endif
