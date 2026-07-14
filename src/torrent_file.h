#include <string>
#include <vector>

using namespace std;


struct TorrentFile{
    string announce;
    string name;
    int64_t piece_length;
    int64_t length;
    string info_hash;
    vector<string> pieces;

    TorrentFile parse(string& f);
    int num_pieces();
    int get_piece_length(int idx);


};


// ifstream file(argv[2],ios::binary);

//     stringstream buffer;
//     buffer<<file.rdbuf();

//     string data=buffer.str();

//     BencodeDecoder decoder(data);
//     BencodeVal val=decoder.decoder();

//     val.print_bencode();