#include "NetIncludes.h"
#include "P2P_interface.h"
#include "NetConnectionAux.h"
#include "NetRelay.h"

const char* getPrimaryNetRelayAddress() {
    const char* cmdline_relay = check_command_line("netrelay");
    return cmdline_relay ? cmdline_relay : NET_RELAY_DEFAULT_ADDRESS;
}

bool receiveNetRelayMessage(
        NetConnection* relay,
        NETID source,
        NetRelayMessage* result,
        NetRelayMessageType result_type_expected
) {
    if (!relay) {
        xassert(0);
        return false;
    }

    //Receive packet from relay
    NetConnectionMessage* packet = nullptr;
    int32_t ret = relay->receive(&packet, CONNECTION_HANDSHAKE_TIMEOUT);
    if (0 < ret) {
        if (packet == nullptr) {
            xassert(0);
            fprintf(stderr, "Missing packet?\n");
            ret = -1;
        } else if (packet->source != relay->getNETID()) {
            fprintf(stderr, "Unexpected relay message source 0x%" PRIX64 " expected 0x%" PRIX64 "\n", packet->source, relay->getNETID());
            ret = -1;
        } else if (packet->destination != source) {
            fprintf(stderr, "Unexpected relay message destination 0x%" PRIX64 " expected 0x%" PRIX64 "\n", packet->destination, source);
            ret = -1;
        }
    }

    //Parse header first
    if (0 < ret) {
        packet->set(0);
        result->read_relay_header(*packet);
        if (result->protocol_version != NET_RELAY_PROTOCOL_VERSION) {
            xassert(0);
            fprintf(stderr, "Unexpected relay protocol 0x%" PRIX32 "\n", result->protocol_version);
            ret = -1;
        } else if (result->msg_type == RELAY_MSG_CLOSE) {
            static NetRelayMessage_Close msg_close;
            msg_close.read(*packet);
            fprintf(stderr, "Relay connection sent close msg: 0x%" PRIX32 "\n", msg_close.code);
            relay->close();
            ret = -1;
        } else if (result_type_expected != RELAY_MSG_UNKNOWN && result->msg_type != result_type_expected) {
            xassert(0);
            fprintf(stderr, "Unexpected relay message type 0x%" PRIX32 " expected 0x%" PRIX32 "\n", result->msg_type, result_type_expected);
            ret = -1;
        }
    }

    //Read rest of content if header is OK
    if (0 < ret) {
        result->read(*packet);
    }
    
    //Error occurred
    if (packet) {
        delete packet;
        packet = nullptr;
    }
    if (ret == 0) {
        fprintf(stderr, "Timeout receiving relay response\n");
    } else if (ret < 0) {
        fprintf(stderr, "Error receiving relay response: %" PRIi32"\n", ret);
    }
    return 0 < ret;
}

bool sendNetRelayMessage(
        NetConnection* relay,
        const NetRelayMessage* msg,
        NETID source
) {
    //Assemble buffer and send it
    static XBuffer buf(1024, true);
    buf.init();
    msg->write_relay_header(buf);
    msg->write(buf);
    int ret = relay->send(&buf, source, relay->getNETID(), CONNECTION_RELAY_TIMEOUT);
    if (ret <= 0) {
        return false;
    }

    //Avoid excess grow
    if (buf.length() > 10240) {
        buf.realloc(10240);
    }
    
    return true;
}

void closeNetRelay(NetConnection* relay) {
    if (relay->hasTransport()) {
        static NetRelayMessage_Close msg;
        sendNetRelayMessage(relay, &msg, NETID_NONE);
    }
    relay->close();
}

void NetRelayMessage::read_relay_header(XBuffer& in) {
    uint32_t tmp;
    in > tmp;
    protocol_version = (tmp >> 24) & 0xFF;
    msg_type = static_cast<NetRelayMessageType>(tmp & 0xFFFFFF);
}

void NetRelayMessage::write_relay_header(XBuffer& out) const {
    uint32_t tmp = static_cast<uint32_t>(msg_type) & 0xFFFFFF;
    tmp |= static_cast<uint32_t>(protocol_version & 0xFF) << 24;
    out < tmp;
}

void NetRelayMessage::write_string(XBuffer& out, const std::string& str, uint16_t max_len) {
    uint16_t len = static_cast<uint16_t>(str.length());
    max_len = min(max_len, NET_RELAY_MAX_STRING_LENGTH);
    if (len > max_len) {
        xassert(0);
        len = max_len;
    }
    out < len;
    out.write(str.c_str(), len, true);
}

void NetRelayMessage::write_list(XBuffer& out, const std::vector<std::string>& list, uint16_t max_elements, uint16_t max_element_len) {
    uint16_t len = static_cast<uint16_t>(list.size());
    max_elements = min(max_elements, NET_RELAY_MAX_LIST_ELEMENTS);
    if (len > max_elements) {
        xassert(0);
        len = max_elements;
    }
    out < len;
    for (int i = 0; i < len; ++i) {
        write_string(out, list[i], max_element_len);
    }
}

void NetRelayMessage::write_map(XBuffer& out, const std::unordered_map<std::string, std::string>& map, uint16_t max_elements, uint16_t max_key_len, uint16_t max_value_len) {
    uint16_t len = static_cast<uint16_t>(map.size());
    max_elements = min(max_elements, NET_RELAY_MAX_MAP_ELEMENTS);
    if (len > max_elements) {
        xassert(0);
        len = max_elements;
    }
    out < len;
    for (auto& pair : map) {
        write_string(out, pair.first, max_key_len);
        write_string(out, pair.second, max_value_len);
    }
}

void NetRelayMessage_Close::read(XBuffer &in) {
    in > code;
}

void NetRelayMessage_Close::write(XBuffer& out) const {
    out < code;
}

void NetRelayMessage_PeerListLobbyHosts::write(XBuffer& out) const {
    //Game identifier 
    write_string(out, "perimeter", 32);
    //Game version required
    write_string(out, PERIMETER_VERSION, 32);
    //Send in XPrm format
    out < NET_RELAY_FORMAT_XPRM;
}

void NetRelayMessage_PeerListLobbies::write(XBuffer& out) const {
    //Game identifier 
    write_string(out, "perimeter", 32);
    //Game version required
    write_string(out, PERIMETER_VERSION, 32);
    //Send in XPrm format
    out < NET_RELAY_FORMAT_XPRM;
}

void NetRelayMessage_PeerSetupRoom::write(XBuffer& out) const {
    //Game identifier 
    write_string(out, "perimeter", 32);
    //Game version required
    write_string(out, PERIMETER_VERSION, 32);
    write_string(out, roomName, 32);
    out < static_cast<uint8_t>(hasPasword);
    out < static_cast<uint8_t>(isClosed);
    out < static_cast<uint8_t>(gameStarted);
    out < playersCount;
    out < playersMax;
    out < RELAY_TOPOLOGY_SERVER_CLIENT;
    //Game specific data for this room
    //In this case the scenario name
    write_map(out, {
        { "scenario", scenarioName },
        { "game_content", std::to_string(gameContent) },
        { "arch", std::to_string(NetConnectionInfo::computeArchFlags()) },
    }, 32, 64, 128);
}

void NetRelayMessage_PeerJoinRoom::write(XBuffer& out) const {
    //Game identifier 
    write_string(out, "perimeter", 32);
    //Game version required
    write_string(out, PERIMETER_VERSION, 32);
    //Desired room to join
    out < room;
}

void NetRelayMessage_PeerClosePeer::write(XBuffer& out) const {
    out < netid;
}

void NetRelayMessage_PeerPingResponse::write(XBuffer& out) const {
    out < secs;
    out < subsecs;
}

void NetRelayMessage_RelayListLobbyHosts::read(XBuffer& in) {
    data = in;
}

void NetRelayMessage_RelayListLobbies::read(XBuffer& in) {
    data = in;
}

void NetRelayMessage_RelayPing::read(XBuffer& in) {
    in > secs;
    in > subsecs;
}

void NetRelayMessage_RelayListPeers::read(XBuffer& in) {
    uint32_t len = 0; 
    NETID netid = 0;
    in > len;
    for (uint32_t i = 0; i < len; ++i) {
        in > netid;
        this->peers.push_back(netid);
    }
}

void NetRelayMessage_RelayAddPeer::read(XBuffer& in) {
    in > netid;
}

void NetRelayMessage_RelayRemovePeer::read(XBuffer& in) {
    in > netid;
}
