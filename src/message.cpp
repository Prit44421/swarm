#include <arpa/inet.h>
#include <cstring>

#include "message.h"

vector<uint8_t> PeerMessage::make_handshake(string& info_hash, string& peer_id){
    vector<uint8_t> v;
    v.push_back(19);
    string proto="BitTorrent protocol";
    v.insert(v.end(), proto.begin(), proto.end());
    v.insert(v.end(), 8, 0);
    v.insert(v.end(), info_hash.begin(), info_hash.end());
    v.insert(v.end(), peer_id.begin(), peer_id.end());
    return v;
}

PeerMessage PeerMessage::make_request(uint32_t index, uint32_t offset, uint32_t length){

    vector<uint8_t> payload(12);
    uint32_t i=htonl(index);
    uint32_t o=htonl(offset);
    uint32_t l=htonl(length);

    memcpy(payload.data(), &i, 4);
    memcpy(payload.data()+4, &o, 4);
    memcpy(payload.data()+8, &l, 4);

    return PeerMessage{MessageType::REQUEST, payload};
}

vector<uint8_t> PeerMessage::serialize(){
    vector<uint8_t> v(4);
    uint32_t length=htonl(payload.size()+1);
    memcpy(v.data(), &length, 4);
    v.push_back(static_cast<uint8_t>(type));
    v.insert(v.end(), payload.begin(), payload.end());
    return v;
}

PeerMessage PeerMessage::make_interested(){
    return PeerMessage{MessageType::INTERESTED, {}};
}