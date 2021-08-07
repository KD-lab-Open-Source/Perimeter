#include "StdAfx.h"
#include "P2P_interface.h"


#include "GameShell.h"
#include "Universe.h"

#include <algorithm>

#include "P2P_interfaceAux.h"

#define XDP_CHECK_HR(hr, msg) {if(FAILED(hr)) ErrH.Abort(msg);}

#define NO_PERIMETR_DEFAULT_PORT

#define IP1(x) (x & 0xff)
#define IP2(x) ((x>>8) & 0xff)
#define IP3(x) ((x>>16) & 0xff)
#define IP4(x) ((x>>24) & 0xff)

const unsigned int PERIMETER_MAX_NETWORK_PLAYER=20;
// {A0B71D62-8E88-4767-8439-184327615B42}
const GUID guidPerimeterGame = 
{ 0xa0b71d62, 0x8e88, 0x4767, { 0x84, 0x39, 0x18, 0x43, 0x27, 0x61, 0x5b, 0x42 } };

void XDPInit()
{
}

void XDPClose()
{
}

bool PNetCenter::Init(void)
{
    //TODO

	SetConnectionTimeout(TIMEOUT_DISCONNECT); //30s //3600000

	return 1;
}

int PNetCenter::ServerStart(const char* _name, int port)
{
    //TODO

    return 1;
}

void PNetCenter::SetConnectionTimeout(int ms) {
    //TODO
}

void PNetCenter::RemovePlayer(DPNID dpnid)
{
    if(isHost() && dpnid==m_localDPNID && dpnid==m_hostDPNID){
        ExecuteInternalCommand(PNC_COMMAND__END_GAME, false);
        ExecuteInterfaceCommand(PNC_INTERFACE_COMMAND_HOST_TERMINATED_GAME);
    }
    else {
        //TODO
    }
}

void PNetCenter::Close(bool flag_immediatle)
{
    //TODO
	flag_connected=false;
}

int PNetCenter::Connect(GUID _hostID) //const char* lpszHost, int port)
{
    //TODO

	return 1;
}

int PNetCenter::Connect(unsigned int ip)//, int port
{
	//if(!ip)
	//	return Connect((const char*)0, port);

	///ServerStart("qqq", PERIMETER_DEFAULT_PORT);
	char ip_string[17];
	memset(ip_string, 0, 17);
	sprintf(ip_string, "%d.%d.%d.%d", IP1(ip), IP2(ip), IP3(ip), IP4(ip));

	//TODO

	GUID hostID;
	return Connect(hostID);
}

bool PNetCenter::isConnected()
{
	return flag_connected;
}


int PNetCenter::Send(const char* buffer, int size, DPNID dpnid, bool flag_guaranted)
{
    //TODO

	return size;
}

bool PNetCenter::StartFindHostDP(const char* lpszHost)
{
    //TODO
	return false;
}

void PNetCenter::StopFindHostDP(void)
{
    //TODO
}

