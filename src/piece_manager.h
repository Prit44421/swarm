#include <vector>
#include <string>
#include <mutex>

using namespace std;


enum class PieceState{
    NEEDED,
    PENDING,
    COMPLETE
};


struct PieceManager{
    int n_pieces;
    int p_len;
    int64_t total_len;
    vector<string> pieces_hash;
    vector<PieceState> pieces_state;
    mutex mtx;


    int next_piece(vector<bool>& peer_bitfield);
    void mark_complete(int idx);

    void mark_needed(int idx);

    bool is_complete();

    int pieces_done();

    int get_piece_length(int idx);

};

