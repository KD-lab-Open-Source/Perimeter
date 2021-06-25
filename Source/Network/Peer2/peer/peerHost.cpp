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

/*************
** INCLUDES **
*************/
#include "peerQR.h"
#include "peerRooms.h"

#include <iostream>

/**************
** FUNCTIONS **
**************/
#ifdef __MWERKS__ // CodeWarrior will warn if not prototyped
PEERBool piStartHosting (PEER peer, SOCKET socket, unsigned short port);	// peerOperations.c
void piStopHosting(PEER peer, PEERBool stopReporting);						// peerOperations.c
#endif

PEERBool piStartHosting
(
	PEER peer,
	SOCKET socket,
	unsigned short port
)
{
	PEER_CONNECTION;

	// Check that we're not hosting.
	////////////////////////////////
	assert(!connection->hosting);
	if(connection->hosting)
		return PEERFalse;

	// Now we're hosting.
	/////////////////////
	connection->hosting = PEERTrue;

	// Start reporting.
	///////////////////
	if(!piStartReporting(peer, socket, port))
		return PEERFalse;

	return PEERTrue;
}

void piStopHosting
(
	PEER peer,
	PEERBool stopReporting
)
{
	PEER_CONNECTION;

	// Stop reporting.
	//////////////////
	if(stopReporting)
		piStopReporting(peer);

	// Check that we're hosting.
	////////////////////////////
	if(!connection->hosting)
		return;

	// Reset states.
	////////////////
	connection->hosting = PEERFalse;
	connection->playing = PEERFalse;
	connection->ready = PEERFalse;

	// Set the flags.
	/////////////////
	piSetLocalFlags(peer);
}
