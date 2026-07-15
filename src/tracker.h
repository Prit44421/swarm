#include <string>
#include <vector>
#include <cstdint>

#include "torrent_file.h"

struct Peer{
    string ip;
    uint16_t port;

};

class Tracker{
public:
    vector<Peer> query(TorrentFile& t);

private:
    string url_encode(string& s);

    string peer_id();
};
