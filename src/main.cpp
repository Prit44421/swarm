#include <iostream>
#include <fstream>
#include <sstream>

// #include "torrent_file.h"
#include "tracker.h"

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


    cout<<peers.size();
    for(auto i:peers){
        cout<<i.ip<<" : "<<i.port<<"\n";
    }

    return 0;
}