#ifndef PERIMETER_SERVERLOBBY_H
#define PERIMETER_SERVERLOBBY_H

//Contains host connection info provided by server list or for direct connections
class GameHostConnection {
private:
public:
    uint64_t server_id; //ID provided by server list, 0 for direct connections
    IPaddress host_ip; //IP provided by server list upon connection request

    void operator=(const GameHostConnection& other) {
        this->server_id = other.server_id;
        this->host_ip.host = other.host_ip.host;
        this->host_ip.port = other.host_ip.port;
    }

    GameHostConnection();
    explicit GameHostConnection(uint64_t server_id);
    explicit GameHostConnection(const IPaddress& host_ip);

    std::string getString() const;
    std::string getStringIP() const;  
};

class ServerLobby {
private:
    bool findingHosts = false;
    std::vector<GameHostConnection> foundLobbyHosts;

public:
    ServerLobby();
    ~ServerLobby();
    void startHostFind();
    void stopHostFind();
};

#endif //PERIMETER_SERVERLOBBY_H
