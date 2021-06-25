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

#ifndef _PEERHOST_H_
#define _PEERHOST_H_

/*************
** INCLUDES **
*************/
#include "peerMain.h"

/**************
** FUNCTIONS **
**************/
PEERBool piStartHosting(PEER peer, SOCKET socket, unsigned short port);
void piStopHosting(PEER peer, PEERBool stopReporting);

#endif
