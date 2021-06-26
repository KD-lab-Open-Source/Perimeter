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

#ifndef _PEERQR_H_
#define _PEERQR_H_

/*************
** INCLUDES **
*************/
#include "peerMain.h"

/************
** DEFINES **
************/
#define PI_QUERYPORT       6500

/**************
** FUNCTIONS **
**************/
PEERBool piStartReporting(PEER peer, SOCKET socket, unsigned short port);
void piStopReporting(PEER peer);
void piSendStateChanged(PEER peer);
void piQRThink(PEER peer);
PEERBool piStartAutoMatchReporting(PEER peer);
void piStopAutoMatchReporting(PEER peer);

#endif
