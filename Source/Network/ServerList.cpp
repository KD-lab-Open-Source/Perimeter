#include "NetIncludes.h"

#include "ServerList.h"

GameHostConnection::GameHostConnection(uint64_t server_id)
: server_id(server_id), host_ip() {
    host_ip.host = INADDR_NONE;
    host_ip.port = 0;
}

GameHostConnection::GameHostConnection(const IPaddress& host_ip)
: server_id(0), host_ip(host_ip) {
}

GameHostConnection::GameHostConnection(): GameHostConnection(0) {
}

std::string GameHostConnection::getString() const {
    std::string text;
    
    if (server_id) {
        text += "ID: " + std::to_string(server_id);
    }
    
    if (host_ip.host != INADDR_NONE) {
        text += "IP: " + getStringIP();
        if (host_ip.port) {
            text += ":" + std::to_string(host_ip.port);
        }
    }
    
    return text;
}

std::string GameHostConnection::getStringIP() const {
    std::string text;
    for (int i = 0; i < 4; ++i) {
        if (i > 0) text += ".";
        uint8_t v = (host_ip.host >> (8 * i)) & 0xff;
        text += std::to_string(v);
    }
    return text;
}

ServerList::ServerList() {
}


ServerList::~ServerList() {
}

void ServerList::startHostFind() {
    if (findingHosts) return;
    findingHosts = true;
    //TODO
}

void ServerList::stopHostFind() {
    if (!findingHosts) return;
    findingHosts = false;
    //TODO
}
