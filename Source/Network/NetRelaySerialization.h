#ifndef PERIMETER_NETRELAYSERIALIZATION_H
#define PERIMETER_NETRELAYSERIALIZATION_H

struct NetRelay_LobbyHost {
    std::string host_tcp = {};
    std::string host_ws = {};

    SERIALIZE(ar) {
        ar & WRAP_OBJECT(host_tcp);
        ar & WRAP_OBJECT(host_ws);
    }
    
    ///Provides the address of this lobby host based on protocol we can use
    const std::string& getAddress() {
#ifdef EMSCRIPTEN
        //Use WebSocket connection
        return host_ws;
#else
        //Use TCP connection
        return host_tcp;
#endif
    }
};

#endif //PERIMETER_NETRELAYSERIALIZATION_H
