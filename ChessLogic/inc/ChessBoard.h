#pragma once

#include <stdint.h>
#include <bitset>
#include <string>

enum Side {White, Black, Draw}; //Enum used to track current turn and win/lose/draw

class ChessBoard {
    public:
        // White positions
        std::bitset<64> WK_position{0b1000};
        std::bitset<64> WQ_position{0b10000};
        std::bitset<64> WB_position{0b100100};
        std::bitset<64> WN_position{0b1000010};
        std::bitset<64> WR_position{0b10000001};
        std::bitset<64> WP_position{0b11111111 << 8};

        // Black positions
        std::bitset<64> BK_position{0b00001000};
        std::bitset<64> BQ_position{0b00010000};
        std::bitset<64> BB_position{0b00100100};
        std::bitset<64> BN_position{0b01000010};
        std::bitset<64> BR_position{0b10000001};
        std::bitset<64> BP_position{0b11111111};

        // Other
        Side currTurn;

        void printBoard();
};