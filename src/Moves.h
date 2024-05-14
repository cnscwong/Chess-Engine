#pragma once

#include "common.h"
#include <bitset>

enum Square {
    H1, G1, F1, E1, D1, C1, B1, A1,
    H2, G2, F2, E2, D2, C2, B2, A2,
    H3, G3, F3, E3, D3, C3, B3, A3,
    H4, G4, F4, E4, D4, C4, B4, A4,
    H5, G5, F5, E5, D5, C5, B5, A5,
    H6, G6, F6, E6, D6, C6, B6, A6,
    H7, G7, F7, E7, D7, C7, B7, A7,
    H8, G8, F8, E8, D8, C8, B8, A8,
    numSquares
}; // enum with each var representing the bit shift to get to that square on the bitboard

enum Direction {
    UP = 8,
    DOWN = -UP,
    LEFT = 1,
    RIGHT = -LEFT
}; // enum representing direction that you need to add to square

const int knight_moves[] = 
    {2*UP + LEFT, 2*UP + RIGHT, 2*DOWN + LEFT, 2*DOWN + RIGHT, 2*RIGHT + UP, 2*RIGHT + DOWN, 2*LEFT + UP, 2*LEFT + DOWN}; //All possible knight moves
std::bitset<64> knightLookup[numSquares];

bool valid_square(int s); //Checks if s is a valid square within the 8x8 chessboard
void init_knight();
void init();
