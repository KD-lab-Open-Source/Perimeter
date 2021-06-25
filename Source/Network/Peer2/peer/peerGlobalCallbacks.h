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

#ifndef _PEERGLOBALCALLBACKS_H_
#define _PEERGLOBALCALLBACKS_H_

/*************
** INCLUDES **
*************/
#include "peerMain.h"

/************
** DEFINES **
************/
#define PI_UTM_LAUNCH          "GML"
#define PI_UTM_XPING           "PNG"

/**************
** FUNCTIONS **
**************/

/* Chat.
*******/
void piChatDisconnected(CHAT chat, const char * reason, PEER peer);
void piChatPrivateMessage(CHAT chat, const char * user, const char * message, int type, PEER peer);
void piSetChannelCallbacks(PEER peer, chatChannelCallbacks * channelCallbacks);

#endif
