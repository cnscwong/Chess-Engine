#include <bitset>
#include <iostream>

/*----------------------------------*/
/*       BOARD REPRESENTATION       */
/*----------------------------------*/

// 8x8 chess board represented as 64 bits
#define Bitboard std::bitset<64>

// bitboards representing the files and ranks of a chessboard
constexpr Bitboard FILE_A = 0x0101010101010101;
constexpr Bitboard FILE_B = 0x0202020202020202;
constexpr Bitboard FILE_C = 0x0404040404040404;
constexpr Bitboard FILE_D = 0x0808080808080808;
constexpr Bitboard FILE_E = 0x1010101010101010;
constexpr Bitboard FILE_F = 0x2020202020202020;
constexpr Bitboard FILE_G = 0x4040404040404040;
constexpr Bitboard FILE_H = 0x8080808080808080;

constexpr Bitboard RANK_1 = 0xFF00000000000000;
constexpr Bitboard RANK_2 = 0x00FF000000000000;
constexpr Bitboard RANK_3 = 0x0000FF0000000000;
constexpr Bitboard RANK_4 = 0x000000FF00000000;
constexpr Bitboard RANK_5 = 0x00000000FF000000;
constexpr Bitboard RANK_6 = 0x0000000000FF0000;
constexpr Bitboard RANK_7 = 0x000000000000FF00;
constexpr Bitboard RANK_8 = 0x00000000000000FF;

constexpr Bitboard ALL_SET = 0xFFFFFFFFFFFFFFFF;

// enum for all squares, corresponds to their position on the bitboard
enum Square {
    A8, B8, C8, D8, E8, F8, G8, H8,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A1, B1, C1, D1, E1, F1, G1, H1,
    numSquares
};

//Enum used to track colour of pieces... etc.
enum Colour {White, Black, numColour};

bool validSquare(int square){
    return square >= 0 && square < 64;
}

// debug print function
void printBitBoard(Bitboard board){
    for(int rank = 0; rank < 8; rank++){
        std::cout << 8 - rank << "   ";
        for(int file = 0; file < 8; file++){
            std::cout << board[rank*8 + file] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << "    A B C D E F G H" << std::endl << std::endl << "ULL:" << board.to_ullong() << std::endl << std::endl;
}

/*----------------------------------*/
/*          MAGIC BITBOARDS         */
/*----------------------------------*/
constexpr Bitboard NO_BORDER = 0x007E7E7E7E7E7E00;

Bitboard gen_bishop_mask(int square){
    Bitboard mask = 0ULL;

    int sRank = square / 8;
    int sFile = square % 8;
    int i, j;

    for(i = sRank + 1, j = sFile + 1; i < 7 && j < 7; i++, j++){
        mask |= 1ULL << (i*8 + j);
    }

    for(i = sRank - 1, j = sFile - 1; i > 0 && j > 0; i--, j--){
        mask |= 1ULL << (i*8 + j);
    }

    for(i = sRank + 1, j = sFile - 1; i < 7 && j > 0; i++, j--){
        mask |= 1ULL << (i*8 + j);
    }

    for(i = sRank - 1, j = sFile + 1; i > 0 && j < 7; i--, j++){
        mask |= 1ULL << (i*8 + j);
    }

    return mask;
}

Bitboard gen_rook_mask(int square){
    Bitboard mask = 0ULL;

    int sRank = square / 8;
    int sFile = square % 8;
    int i, j;

    for(i = sRank + 1; i < 7; i++){
        mask |= 1ULL << (i*8 + sFile);
    }

    for(i = sRank - 1; i > 0; i--){
        mask |= 1ULL << (i*8 + sFile);
    }

    for(j = sFile + 1;j < 7; j++){
        mask |= 1ULL << (sRank*8 + j);
    }

    for(j = sFile - 1;j > 0; j--){
        mask |= 1ULL << (sRank*8 + j);
    }

    return mask;
}

/*----------------------------------*/
/*          MOVE GENERATION         */
/*----------------------------------*/

Bitboard pawn_attacks[numColour][numSquares];
Bitboard knight_moves[numSquares];
Bitboard king_moves[numSquares];

// generates possible W/B pawn moves at specified square
Bitboard gen_pawn_attacks(Colour c, int square){
    Bitboard temp = 1ULL << square;
    Bitboard attack = 0ULL;
    if(c == White){
        attack |= ~FILE_A & temp >> 7;
        attack |= ~FILE_H & temp >> 9;
    }else{
        attack |= ~FILE_A & temp << 9;
        attack |= ~FILE_H & temp << 7;
    }

    return attack;
}

// generates possible knight moves at specified square
Bitboard gen_knight_moves(int square){
    Bitboard temp = 1ULL << square;
    Bitboard moves = 0ULL;

    moves |= ~FILE_A & ~FILE_B & temp >> 6;
    moves |= ~FILE_H & ~FILE_G & temp >> 10;
    moves |= ~FILE_A & temp >> 15;
    moves |= ~FILE_H & temp >> 17;

    moves |= ~FILE_H & ~FILE_G & temp << 6;
    moves |= ~FILE_A & ~FILE_B & temp << 10;
    moves |= ~FILE_H & temp << 15;
    moves |= ~FILE_A & temp << 17;

    return moves;
}

// possible king moves at specified square
Bitboard gen_king_moves(int square){
    Bitboard temp = 1ULL << square;
    Bitboard moves = 0ULL;
    
    //right moves
    moves |= ~FILE_A & temp >> 7;
    moves |= ~FILE_A & temp << 1;
    moves |= ~FILE_A & temp << 9;

    //up down
    moves |= temp >> 8;
    moves |= temp << 8;

    //left moves
    moves |= ~FILE_H & temp >> 9;
    moves |= ~FILE_H & temp >> 1;
    moves |= ~FILE_H & temp << 7;

    return moves;
}

void init_lookup_tables(){
    for(int i = 0; i < 64; i++){
        pawn_attacks[White][i] = gen_pawn_attacks(White, i);
        pawn_attacks[Black][i] = gen_pawn_attacks(Black, i);
        
        king_moves[i] = gen_king_moves(i);

        knight_moves[i] = gen_knight_moves(i);
    }
}

int main(){
    for(int i = 0; i < 64; i++)
        printBitBoard(gen_rook_mask(i));

    return 0;
}