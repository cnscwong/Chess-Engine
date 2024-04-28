#include "ChessBoard.h"
ChessBoard::ChessBoard() {
    currTurn = White;

    WP_position <<= 8;

    BK_position <<= 56;
    BQ_position <<= 56;
    BB_position <<= 56;
    BN_position <<= 56;
    BR_position <<= 56;
    BP_position <<= 48;
}

void printBoard(){
    std::string board[64];
    
}