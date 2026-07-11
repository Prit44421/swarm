#include <iostream>
#include <fstream>
#include <sstream>

#include "bencode.h"

using namespace std;


int main(int argc, char* argv[]){

    if(string(argv[1])!="download"){
        cout<<"Invalid command\nUsage:"<<argv[0]<<" download <torrent file> -o <downlod dir>(optional)\n";
    }
    ifstream file(argv[2],ios::binary);

    stringstream buffer;
    buffer<<file.rdbuf();

    string data=buffer.str();

    BencodeDecoder decoder(data);
    BencodeVal val=decoder.decoder();

    val.print_bencode();

    return 0;
}