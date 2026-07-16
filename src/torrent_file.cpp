#include <fstream>
#include <sstream>

#include "torrent_file.h"
#include "bencode.h"
#include "sha1.h"



void TorrentFile::parse(string & s){

    ifstream file(s,ios::binary);

    stringstream ss;
    ss<<file.rdbuf();

    string data=ss.str();
    
    // cout<<data.substr(0,100)<<"\n";
    BencodeDecoder decoder(data);
    BencodeVal val=decoder.decoder();

    announce=get<string>((get<map<string, BencodeVal>>(val.value).at("announce")).value);

    map<string, BencodeVal> info=get<map<string, BencodeVal>>(get<map<string, BencodeVal>>(val.value).at("info").value);

    name=get<string>(info.at("name").value);
    length=get<int64_t>(info.at("length").value);
    piece_length=get<int64_t>(info.at("piece length").value);
    
    string piece=get<string>(info.at("pieces").value);

    vector<string> pieces_hash;

    for(int i=0;i<piece.size();i+=20){
        pieces_hash.push_back(piece.substr(i,20));
    }

    BencodeEncoder encode;
    

    string info_ben=encode.encoder((get<map<string, BencodeVal>>(val.value).at("info")));

    info_hash=sha1_hash(info_ben);

}

int TorrentFile::num_pieces(){
    return pieces.size()/piece_length;
}