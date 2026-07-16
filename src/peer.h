#include <string>
#include <vector>
#include <sys/socket.h>
#include <unistd.h>
#include <optional>

#include "message.h"
#include "tracker.h"

using namespace std;


class PeerConnection{
private:
    int sockfd=-1;
    Peer peer;
    string info_hash;
    string peer_id;
    bool chok=true;
    bool interested=false;
    vector<bool> pieces;

public:
    PeerConnection(Peer p, string& ih, string& pid, int n){
        peer=p;
        info_hash=ih;
        peer_id=pid;
        pieces.resize(n,false);
    }

    ~PeerConnection(){
        close(sockfd);
    }

    bool connect();
    bool handshake();
    void recv_bitfield(vector<bool>& bitfield);

    bool send_msg(PeerMessage& m);
    optional<PeerMessage> recv_msg();
    bool send_all(void* data, size_t len);
    bool recv_all(void* buff, size_t len);
    bool has_piece(int idx);


};