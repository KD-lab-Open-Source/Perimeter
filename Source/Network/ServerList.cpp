#include "NetIncludes.h"
#include "ServerList.h"

ServerList::ServerList() {
}

ServerList::~ServerList() = default;

void ServerList::startHostFind() {
    if (findingHosts) return;
    findingHosts = true;
    //TODO
}

void ServerList::stopHostFind() {
    if (!findingHosts) return;
    findingHosts = false;
    clearHostInfoList();
    //TODO
}

void ServerList::clearHostInfoList() {
    std::vector<GameHostInfo>::iterator p;
    gameHostInfoList.erase(gameHostInfoList.begin(), gameHostInfoList.end());
}

void ServerList::refreshHostInfoList()
{
    if (!findingHosts) return;
    clearHostInfoList();

    //TODO here we should parse the whatever content the listing server gave us and create GameHostInfo for gameHostInfoList
}