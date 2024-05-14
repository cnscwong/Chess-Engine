#pragma once

#include <stdint.h>
#include <bitset>
#include <iostream>
#include "Moves.h"
#include "common.h"

constexpr std::bitset<64> FILE_A = 0x8080808080808080;
constexpr std::bitset<64> FILE_B = 0x4040404040404040;
constexpr std::bitset<64> FILE_C = 0x2020202020202020;
constexpr std::bitset<64> FILE_D = 0x1010101010101010;
constexpr std::bitset<64> FILE_E = 0x0808080808080808;
constexpr std::bitset<64> FILE_F = 0x0404040404040404;
constexpr std::bitset<64> FILE_G = 0x0202020202020202;
constexpr std::bitset<64> FILE_H = 0x0101010101010101;

constexpr std::bitset<64> RANK_1 = 0x00000000000000FF;
constexpr std::bitset<64> RANK_2 = 0x000000000000FF00;
constexpr std::bitset<64> RANK_3 = 0x0000000000FF0000;
constexpr std::bitset<64> RANK_4 = 0x00000000FF000000;
constexpr std::bitset<64> RANK_5 = 0x000000FF00000000;
constexpr std::bitset<64> RANK_6 = 0x0000FF0000000000;
constexpr std::bitset<64> RANK_7 = 0x00FF000000000000;
constexpr std::bitset<64> RANK_8 = 0xFF00000000000000;

class ChessBoard {
    public:
        // Positions
        std::bitset<64> boardState, whiteState, blackState;
        std::bitset<64> White_Positions[numPieces];
        std::bitset<64> Black_Positions[numPieces];

        // Other
        Colour currTurn;
        ChessBoard();
};

void printBitBoard(std::bitset<64> board);