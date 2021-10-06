#include "NetIncludes.h"
#include "P2P_interface.h"


#include "GameShell.h"
#include "Universe.h"

#include <algorithm>

#define XDP_CHECK_HR(hr, msg) {if(FAILED(hr)) ErrH.Abort(msg);}

#define NO_PERIMETR_DEFAULT_PORT

#define IP1(x) (x & 0xff)
#define IP2(x) ((x>>8) & 0xff)
#define IP3(x) ((x>>16) & 0xff)
#define IP4(x) ((x>>24) & 0xff)

const unsigned int PERIMETER_MAX_NETWORK_PLAYER=20;

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

void PNetCenter::RemovePlayer(NETID netid)
{
    if(isHost() && netid==m_localNETID && netid==m_hostNETID){
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

int PNetCenter::Connect(const GameHostConnection& host)
{
    LogMsg("Connect %du:%du %lu\n", host.host_ip.host, host.host_ip.port, host.server_id);

	//TODO

	return 0;
}

bool PNetCenter::isConnected()
{
	return flag_connected;
}


int PNetCenter::Send(const char* buffer, int size, NETID netid, bool flag_guaranted)
{
    //TODO

	return size;
}

