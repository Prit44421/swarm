#include "piece_manager.h"



int PieceManager::next_piece(vector<bool>& peer_bitfield){
    mtx.lock();
    for(int i=0;i<n_pieces;i++){
        if(peer_bitfield[i] && pieces_state[i]==PieceState::NEEDED){
            pieces_state[i]=PieceState::PENDING;
            mtx.unlock();
            return i;
        }
    }
    mtx.unlock();
    return {};
}

void PieceManager::mark_complete(int idx){
    mtx.lock();
    pieces_state[idx]=PieceState::COMPLETE;
    mtx.unlock();
}

void PieceManager::mark_needed(int idx){
    mtx.lock();
    pieces_state[idx]=PieceState::NEEDED;
    mtx.unlock();
}

bool PieceManager::is_complete(){
    mtx.lock();
    for(int i=0;i<n_pieces;i++){
        if(pieces_state[i]!=PieceState::COMPLETE){
            mtx.unlock();
            return false;
        }
    }
    mtx.unlock();
    return true;
}

int PieceManager::pieces_done(){
    mtx.lock();
    int count=0;
    for(int i=0;i<n_pieces;i++){
        if(pieces_state[i]==PieceState::COMPLETE){
            count++;
        }
    }
    mtx.unlock();
    return count;
}

int PieceManager::get_piece_length(int idx){
    if(idx<0 || idx>=n_pieces){
        return -1;
    }
    if(idx==n_pieces-1){
        return total_len-(p_len*(n_pieces-1));
    }
    return p_len;
}