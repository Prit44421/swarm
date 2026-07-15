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

    void parse(string& f);
    int num_pieces();
    int get_piece_length(int idx);


};
