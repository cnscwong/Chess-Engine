#include "ChessBoard.h"

ChessBoard::ChessBoard(){
    currTurn = White;

    White_Positions[King] = 0b1000;
    White_Positions[Queen] = 0b10000;
    White_Positions[Bishop] = 0b100100;
    White_Positions[Knight] = 0b1000010;
    White_Positions[Rook] = 0b10000001;
    White_Positions[Pawn] = 0xFF << 8;

    Black_Positions[King] = 0b1000ULL << 56;
    Black_Positions[Queen] = 0b10000ULL << 56;
    Black_Positions[Bishop] = 0b100100ULL << 56;
    Black_Positions[Knight] = 0b1000010ULL << 56;
    Black_Positions[Rook] = 0b10000001ULL << 56;
    Black_Positions[Pawn] = 0xFF << 48;
}

void printBitBoard(std::bitset<64> board){
    for(int i = 63; i > -1; i--){
        std::cout << board[i];
        if(i % 8 == 0){
            std::cout << std::endl;
        }
    }
}