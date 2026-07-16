#include <iostream>
#include <fstream>
#include <sstream>
#include <curl/curl.h>

// #include "torrent_file.h"
// #include "tracker.h"
#include "peer.h"


using namespace std;


int main(int argc, char* argv[]){

    if(string(argv[1])!="download"){
        cout<<"Invalid command\nUsage:"<<argv[0]<<" download <torrent file> -o <downlod dir>(optional)\n";
    }

    TorrentFile t;
    string file=argv[2];
    cout<<file<<"\n";
    t.parse(file);

    cout<<t.announce<<"\n"<<t.name;

    Tracker track;
    vector<Peer> peers= track.query(t);


    // cout<<peers.size();
    for(auto i:peers){
        cout<<i.ip<<" : "<<i.port<<"\n";
    }

    Peer tar=peers[0];

    string peer_id=Tracker::peer_id();
    PeerConnection conn(tar, t.info_hash, peer_id, t.num_pieces());

    if(conn.connect()){
        cout<<"Connected to peer\n";
        if(conn.handshake()){
            cout<<"Handshake successful\n";
            vector<bool> bitfield;
            conn.recv_bitfield(bitfield);
            cout<<"Bitfield received\n";
        }

    }
    curl_global_cleanup();
    return 0;
}