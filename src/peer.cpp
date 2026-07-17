#pragma once
#include <arpa/inet.h>
#include <cstring>
#include <iostream>

#include "peer.h"
#include "sha1.h"

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
    while(true){
        uint32_t l=0;

        if(!recv_all(&l, 4)){
            return {};
        }
        l=ntohl(l);

        if(l==0) continue;

        uint8_t id=0;
        if(!recv_all(&id, 1)) return {};

        vector<uint8_t> payload;

        if(l>1){
            payload.resize(l-1);
            if(!recv_all(payload.data(), l-1)) return {};
        }
        return PeerMessage{static_cast<MessageType>(id), payload};
    }
}

void PeerConnection::recv_bitfield(vector<bool>& bitfield){
    bitfield.resize(pieces.size(), false);

    while (true) {
        auto msg = recv_msg();
        if(!msg) break; 
        cout<<"Received message of type "<<static_cast<int>(msg->type)<<"\n"; if(msg->type == MessageType::HAVE) cout<<"HAVE piece "<<ntohl(*reinterpret_cast<uint32_t*>(msg->payload.data()))<<"\n";
        if(msg->type == MessageType::BITFIELD){
            for(int i = 0; i < msg->payload.size(); i++){
                uint8_t b = msg->payload[i];
                for(int j = 0; j < 8; j++){
                    int idx = i * 8 + j;
                    if(idx < bitfield.size()){
                        bitfield[idx] = (b & (1 << (7 - j))) != 0;
                    }
                }
            }
            pieces = bitfield;
            return; 
        }
        else if(msg->type == MessageType::UNCHOKE){
            chok = false;
        }
        else if(msg->type == MessageType::CHOKE){
            chok = true;
        }
        else if(msg->type == MessageType::HAVE){
            uint32_t idx = ntohl(*reinterpret_cast<uint32_t*>(msg->payload.data()));
            cout << "Peer has piece " << idx << "\n";
            if(idx < bitfield.size()){
                bitfield[idx] = true;
            }
            pieces = bitfield;
        }
    }
    pieces = bitfield;
}


bool PeerConnection::is_unchoked(){
    if(!interested){
        PeerMessage msg=PeerMessage::make_interested();
        if(!send_msg(msg)){
            return false;
        }
        interested=true;
    }
    if(!chok) return true;
    PeerMessage msg=PeerMessage::make_interested();
    if(!send_msg(msg)){
        return false;
    }

    for(int i=0;i<30;i++){
        auto msg=recv_msg();
        if(!msg.has_value()){
            return false;
        }
        if(msg->type==MessageType::UNCHOKE){
            chok=false;
            return true;
        }
        if(msg->type==MessageType::CHOKE){
            chok=true;
            return false;
        }
    }
    return false;
}

bool PeerConnection::download_piece(uint32_t index, int length, vector<uint8_t>& data, string& expected_hash){

    if(!has_piece(index)){
        cout<<"Peer does not have piece "<<index<<"\n";
        return false;
    }
    if(!is_unchoked()){
        cout<<"Peer is choked, cannot download piece "<<index<<"\n";
        return false;
    }

    data.resize(length);

    int block_size=16384;

    for(int offset=0;offset<length;offset+=block_size){
        int req_length=min(block_size, length-offset);
        
        PeerMessage req=PeerMessage::make_request(index, offset, req_length);
        if(!send_msg(req)){
            cout<<"Failed to send request for piece "<<index<<" offset "<<offset<<"\n";
            return false;
        }

        bool received_blk=false;
        while(!received_blk){
            auto msg=recv_msg();
            if(!msg) return false;
            if(msg->type==MessageType::PIECE){
                uint32_t recv_index=ntohl(*reinterpret_cast<uint32_t*>(msg->payload.data()));  //todo explaination
                int recv_offset=ntohl(*reinterpret_cast<uint32_t*>(msg->payload.data()+4)); //todo explaination
                
                if(recv_index==index && recv_offset==offset){
                    int recv_length=msg->payload.size()-8;
                    memcpy(data.data()+offset, msg->payload.data()+8, recv_length);
                    received_blk=true;
                }
            }
            else if(msg->type==MessageType::CHOKE){
                chok=true;
                cout<<"Peer choked us while downloading piece "<<index<<"\n";
                return false;
            }
        }
    }
    string str_data(reinterpret_cast<char*>(data.data()), data.size());
    string hash=sha1_hash(str_data);
    if(hash!=expected_hash){
        //todo error 
        cout<<"Hash mismatch for piece "<<index<<"\n";
        return false;
    }
    return true;
}
    