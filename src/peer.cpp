#include <arpa/inet.h>
#include <cstring>


#include "peer.h"


bool PeerConnection::connect(){
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0){
        return false; //todo error
    }

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(peer.port); 
    inet_pton(AF_INET, peer.ip.c_str(), &server.sin_addr);

    if (::connect(sockfd, (struct sockaddr*)&server, sizeof(server))<0) {
        return false;
    }
    return true;

}

bool PeerConnection::handshake(){
    vector<uint8_t> msg=PeerMessage::make_handshake(info_hash,peer_id);

    if(!send_all(msg.data(), msg.size())){
        return false;
    }
    vector<uint8_t> buff(68);
    if(!recv_all(buff.data(), buff.size())){
        return false;
    }

    if(buff[0]!=19){
        return false;
    }

    if(memcmp(buff.data()+28, info_hash.data(), 20)!=0){
        return false;
    }
    return true;
}

bool PeerConnection::send_msg(PeerMessage& m){
    vector<uint8_t> msg=m.serialize();
    return send_all(msg.data(), msg.size());
}

bool PeerConnection::has_piece(int idx){
    if(idx<0 || idx>=pieces.size()){
        return false;
    }
    return pieces[idx];
}

bool PeerConnection::send_all(void* data, size_t len){
    size_t sent=0;
    uint8_t* buf = static_cast<uint8_t*>(data);
    while(sent<len){
        int s=send(sockfd, buf+sent, len-sent, 0);
        if(s<=0){
            return false;
        }
        sent += s;
    }
    return true;
}

bool PeerConnection::recv_all(void* buff, size_t len){
    size_t recieved=0;
    uint8_t* buf = static_cast<uint8_t*>(buff);
    while(recieved<len){
        int r=recv(sockfd, buf+recieved, len-recieved, 0);
        if(r<=0){
            return false;
        }
        recieved += r;
    }
    return true;
}


optional<PeerMessage> PeerConnection::recv_msg(){
    uint32_t l=0;

    if(!recv_all(&l, 4)){
        return {};
    }
    l=ntohl(l);

    if(l==0) return {};

    uint8_t id=0;
    if(!recv_all(&id, 1)) return {};

    vector<uint8_t> payload;

    if(l>1){
        payload.resize(l-1);
        if(!recv_all(payload.data(), l-1)) return {};
    }
    return PeerMessage{static_cast<MessageType>(id), payload};
}

void PeerConnection::recv_bitfield(vector<bool>& bitfield){
    auto msg=recv_msg();
    if(msg->type!=MessageType::BITFIELD){
        return;
    }
    for(int i=0;i<msg->payload.size();i++){
        uint8_t b=msg->payload[i];
        for(int j=0;j<8;j++){
            int idx=i*8+j;
            if(idx<bitfield.size()){
                bitfield[idx]=(b & (1 << (7-j))) != 0;
            }
        }
    }
}
