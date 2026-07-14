#include <iostream>
#include <fstream>
#include <sstream>

#include "torrent_file.h"

using namespace std;


int main(int argc, char* argv[]){

    if(string(argv[1])!="download"){
        cout<<"Invalid command\nUsage:"<<argv[0]<<" download <torrent file> -o <downlod dir>(optional)\n";
    }

    TorrentFile t;
    string file=argv[2];
    cout<<file<<"\n";
    t.parse(file);

    cout<<t.announce;


    return 0;
}