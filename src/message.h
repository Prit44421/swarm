#include <cstdint>
#include <vector>
#include <string>

using namespace std;

enum class MessageType {
    CHOKE = 0,
    UNCHOKE = 1,
    INTERESTED = 2,
    NOT_INTERESTED = 3,
    HAVE = 4,
    BITFIELD = 5,
    REQUEST = 6,
    PIECE = 7,
    CANCEL = 8
};

struct PeerMessage {
    MessageType type;
    vector<uint8_t> payload;

    vector<uint8_t> static make_handshake(string& info_hash, string& peer_id);

    PeerMessage make_interested();

    PeerMessage make_request(uint32_t index, uint32_t offset, uint32_t length);

    vector<uint8_t> serialize();
};

