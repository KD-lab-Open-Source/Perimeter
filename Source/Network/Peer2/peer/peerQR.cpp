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
#include "peerGlobalCallbacks.h"
#include "peerPlayers.h"
#include "peerAutoMatch.h"
#include "peerOperations.h"

#include <iostream>

/**************
** CALLBACKS **
**************/
static void piQRServerKeyCallback
(
	int key,
	qr2_buffer_t buffer,
	PEER peer
)
{
	PEER_CONNECTION;

	// check if we should report it
	if(connection->inRoom[StagingRoom] && (!connection->playing || (connection->reportingOptions & PEER_REPORT_INFO)))
	{
		// check if it's one of our keys
		switch(key)
		{
		case HOSTNAME_KEY:
			qr2_buffer_add(buffer, NAMES[StagingRoom]);
			return;
		case NUMPLAYERS_KEY:
			qr2_buffer_add_int(buffer, connection->numPlayers[StagingRoom]);
			return;
		case MAXPLAYERS_KEY:
			if(!connection->maxPlayers)
				break;
			qr2_buffer_add_int(buffer, connection->maxPlayers);
			return;
		case GAMEMODE_KEY:
			if(connection->playing)
				break;
			qr2_buffer_add(buffer, "openstaging");
			return;
		case PASSWORD_KEY:
			qr2_buffer_add_int(buffer, connection->passwordedRoom?1:0);
			return;
		}
	}

	// we always report groupid
	if(key == GROUPID_KEY)
	{
		qr2_buffer_add_int(buffer, connection->reportingGroupID);
		return;
	}

	// pass it to the app
	if(connection->callbacks.qrServerKey)
		connection->callbacks.qrServerKey(peer, key, buffer, connection->callbacks.param);
}

static void piQRPlayerKeyCallback
(
	int key,
	int index,
	qr2_buffer_t buffer,
	PEER peer
)
{
	piPlayer * player;
	PEER_CONNECTION;

	// check if we should report it
	if(connection->inRoom[StagingRoom] && (!connection->playing || (connection->reportingOptions & PEER_REPORT_PLAYERS)))
	{
		// check if it's a key we report
		if((key == PLAYER__KEY) || (key == PING__KEY))
		{
			// convert the index to a player
			player = piFindPlayerByIndex(peer, StagingRoom, index);
			if(!player)
				qr2_buffer_add(buffer, "");
			else if(key == PLAYER__KEY)
				qr2_buffer_add(buffer, player->nick);
			else if(key == PING__KEY)
				qr2_buffer_add_int(buffer, player->pingAverage);

			return;
		}
	}

	// pass it to the app
	if(connection->callbacks.qrPlayerKey)
		connection->callbacks.qrPlayerKey(peer, key, index, buffer, connection->callbacks.param);
}

static void piQRTeamKeyCallback
(
	int key,
	int index,
	qr2_buffer_t buffer,
	PEER peer
)
{
	PEER_CONNECTION;

	// pass it to the app
	if(connection->callbacks.qrTeamKey)
		connection->callbacks.qrTeamKey(peer, key, index, buffer, connection->callbacks.param);
}

static void piQRKeyListCallback
(
	qr2_key_type type,
	qr2_keybuffer_t keyBuffer,
	PEER peer
)
{
	PEER_CONNECTION;

	// add our own keys
	switch(type)
	{
	case key_server:
		if(!connection->autoMatchReporting)
		{
			qr2_keybuffer_add(keyBuffer, HOSTNAME_KEY);
			qr2_keybuffer_add(keyBuffer, GAMEMODE_KEY);
			if(connection->passwordedRoom)
				qr2_keybuffer_add(keyBuffer, PASSWORD_KEY);
			if(connection->reportingGroupID)
				qr2_keybuffer_add(keyBuffer, GROUPID_KEY);
		}
		qr2_keybuffer_add(keyBuffer, NUMPLAYERS_KEY);
		if(connection->maxPlayers)
			qr2_keybuffer_add(keyBuffer, MAXPLAYERS_KEY);
		break;
	case key_player:
		qr2_keybuffer_add(keyBuffer, PLAYER__KEY);
		qr2_keybuffer_add(keyBuffer, PING__KEY);
		break;
	default:
		break;
	}

	// pass it to the app
	if(connection->callbacks.qrKeyList)
		connection->callbacks.qrKeyList(peer, type, keyBuffer, connection->callbacks.param);
}

static int piQRCountCallback
(
	qr2_key_type type,
	PEER peer
)
{
	PEER_CONNECTION;

	// we only handle player counts
	if(type == key_player)
	{
		// check if we should report it
		if(connection->inRoom[StagingRoom] && (!connection->playing || (connection->reportingOptions & PEER_REPORT_PLAYERS)))
			return connection->numPlayers[StagingRoom];
	}

	// pass it to the app
	if(connection->callbacks.qrCount)
		return connection->callbacks.qrCount(peer, type, connection->callbacks.param);

	// app's not handling it
	return 0;
}

static void piQRAddErrorCallback
(
	qr2_error_t error,
	char * errorString,
	PEER peer
)
{
	PEER_CONNECTION;

	// handle this if automatching
	if(peerIsAutoMatching(peer))
	{
		PEERAutoMatchStatus status;

		// track the error
		connection->autoMatchQRFailed = PEERTrue;

		// switch to a new state
		if(connection->autoMatchSBFailed && (connection->autoMatchStatus != PEERStaging))
			status = PEERFailed;
		else
			status = PEERSearching;
		piSetAutoMatchStatus(peer, status);

		return;
	}

	// pass it to the app
	if(connection->callbacks.qrAddError)
		connection->callbacks.qrAddError(peer, error, errorString, connection->callbacks.param);
}

static void piQRNatNegotiateCallback
(
	int cookie,
	PEER peer
)
{
	PEER_CONNECTION;

	// pass it to the app
	if(connection->callbacks.qrNatNegotiateCallback)
		connection->callbacks.qrNatNegotiateCallback(peer, cookie, connection->callbacks.param);
}

/**************
** FUNCTIONS **
**************/
PEERBool piStartReporting
(
	PEER peer,
	SOCKET socket,
	unsigned short port
)
{
	int rcode;

	PEER_CONNECTION;

	// Check that we're not reporting.
	//////////////////////////////////
	assert(!connection->queryReporting);
	if(connection->queryReporting)
		piStopReporting(peer);

	// Init qr2.
	////////////
	if(socket == INVALID_SOCKET)
	{
		rcode = qr2_init(&connection->queryReporting, NULL, PI_QUERYPORT, connection->title, connection->qrSecretKey,
			1, connection->natNegotiate,
			piQRServerKeyCallback,
			piQRPlayerKeyCallback,
			piQRTeamKeyCallback,
			piQRKeyListCallback,
			piQRCountCallback,
			piQRAddErrorCallback,
			peer);
	}
	else
	{
		rcode = qr2_init_socket(&connection->queryReporting, socket, port, connection->title, connection->qrSecretKey,
			1, connection->natNegotiate,
			piQRServerKeyCallback,
			piQRPlayerKeyCallback,
			piQRTeamKeyCallback,
			piQRKeyListCallback,
			piQRCountCallback,
			piQRAddErrorCallback,
			peer);
	}

	if(rcode != 0)
		return PEERFalse;

	// Store the ID of the group room we're in.
	///////////////////////////////////////////
	connection->reportingGroupID = connection->groupID;
	
	// Setup the nat-negotiate callback.
	////////////////////////////////////
	qr2_register_natneg_callback(connection->queryReporting, piQRNatNegotiateCallback);
	
	// No options.
	//////////////
	connection->reportingOptions = 0;

	return PEERTrue;
}

void piStopReporting
(
	PEER peer
)
{
	PEER_CONNECTION;

	// Check that we're reporting.
	//////////////////////////////
	if(!connection->queryReporting)
		return;

	// Clean up query-reporting.
	////////////////////////////
	qr2_shutdown(connection->queryReporting);
	connection->queryReporting = NULL;
}

void piSendStateChanged
(
	PEER peer
)
{
	PEER_CONNECTION;

	// Check that we're reporting.
	//////////////////////////////
	if(!connection->queryReporting)
		return;

	// Send the statechanged.
	/////////////////////////
	qr2_send_statechanged(connection->queryReporting);
}

void piQRThink
(
	PEER peer
)
{
	PEER_CONNECTION;

	if(connection->queryReporting)
		qr2_think(connection->queryReporting);
	if(connection->autoMatchReporting)
		qr2_think(connection->autoMatchReporting);
}

PEERBool piStartAutoMatchReporting
(
	PEER peer
)
{
	piOperation * operation;
	char autoMatchTitle[PI_TITLE_MAX_LEN];
	int rcode;

	PEER_CONNECTION;

	// Check that we're not reporting.
	//////////////////////////////////
	assert(!connection->autoMatchReporting);
	if(connection->autoMatchReporting)
		piStopAutoMatchReporting(peer);

	// Get the operation.
	/////////////////////
	operation = connection->autoMatchOperation;
	assert(operation);

	// Setup the AutoMatch gamename.
	////////////////////////////////
	strzcpy(autoMatchTitle, connection->title, sizeof(autoMatchTitle));
	strzcat(autoMatchTitle, "am", sizeof(autoMatchTitle));

	// Init qr2.
	////////////
	if(operation->socket == INVALID_SOCKET)
	{
		rcode = qr2_init(&connection->autoMatchReporting, NULL, PI_QUERYPORT, autoMatchTitle, connection->qrSecretKey,
			1, connection->natNegotiate,
			piQRServerKeyCallback,
			piQRPlayerKeyCallback,
			piQRTeamKeyCallback,
			piQRKeyListCallback,
			piQRCountCallback,
			piQRAddErrorCallback,
			peer);
	}
	else
	{
		rcode = qr2_init_socket(&connection->autoMatchReporting, operation->socket, operation->port, autoMatchTitle, connection->qrSecretKey,
			1, connection->natNegotiate,
			piQRServerKeyCallback,
			piQRPlayerKeyCallback,
			piQRTeamKeyCallback,
			piQRKeyListCallback,
			piQRCountCallback,
			piQRAddErrorCallback,
			peer);

		// If we created the socket, hand over responsibility for it to qr2.
		////////////////////////////////////////////////////////////////////
		if(operation->socketClose)
		{
			operation->socketClose = PEERFalse;
			connection->autoMatchReporting->read_socket = 1;
		}
	}

	// Set the error flag.
	//////////////////////
	connection->autoMatchQRFailed = (rcode == 0)?PEERTrue:PEERFalse;

	// Return false if there was an error.
	//////////////////////////////////////
	if(rcode != 0)
		return PEERFalse;

	return PEERTrue;
}

void piStopAutoMatchReporting
(
	PEER peer
)
{
	PEER_CONNECTION;

	// Check that we're reporting.
	//////////////////////////////
	if(!connection->autoMatchReporting)
		return;

	// Clean up query-reporting.
	////////////////////////////
	qr2_shutdown(connection->autoMatchReporting);
	connection->autoMatchReporting = NULL;
}
