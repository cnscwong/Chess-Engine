#include <bitset>
#include <iostream>
#include <string>
#include <cstring>
#include <map>

/*----------------------------------*/
/*             Bitboard             */
/*----------------------------------*/

// 8x8 chess board represented as 64 bits
#define Bitboard unsigned long long

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

// Bitboard macros
#define sqr(square) (1ULL << (square))
#define set(board, square) ((board) |= sqr(square))
#define get(board, square) (((board) & sqr(square)) ? 1:0)
#define clear(board, square) ((board) &= ~sqr(square))

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
    numSquares, no_sq
};

std::string toSquare[] = {
"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"
};

// Enum used to track colour of pieces... etc.
enum Colour {White, Black, Both};

// Enum used to track piece type, capitalized represents white
enum Pieces {P, N, B, R, Q, K, p, n, b, r, q, k, numPieces};

// Character representations of pieces
const std::string pieceToChar = "PNBRQKpnbrqk";

// Char to piece
std::map<char, int> charToPiece = {{'P', P}, {'N', N}, {'B', B}, {'R', R}, {'Q', Q}, {'K', K}, {'p', p}, {'n', n}, {'b', b}, {'r', r}, {'q', q}, {'k', k}};

// Enum for castling rights
enum Castling {wKingside = 1, wQueenside = 2, bKingside = 4, bQueenside = 8};

// FEN debug positions
#define empty_board "8/8/8/8/8/8/8/8 w - - "
#define start_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
#define killer_position "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define cmk_position "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "

bool validSquare(int square){
    return square >= 0 && square < 64;
}

// counts number of set bits in bitboard
static inline int countBits(Bitboard board){
    int count = 0;
    while(board){
        count++;
        board &= board - 1;
    }

    return count;
}

// finds lsb and returns index
static inline int findLSB(Bitboard b){
    if(b == 0){
        return -1;
    }

    return countBits((b & -b) - 1);
}

// debug print function
void printBitBoard(Bitboard board){
    for(int rank = 0; rank < 8; rank++){
        std::cout << 8 - rank << "   ";
        for(int file = 0; file < 8; file++){
            std::cout << get(board, (rank*8 + file)) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << "    A B C D E F G H" << std::endl << std::endl << "ULL:" << board << std::endl << std::endl;
}

/*----------------------------------*/
/*           LOOKUP TABLES          */
/*----------------------------------*/

Bitboard pawn_attacks[2][numSquares];
Bitboard knight_attacks[numSquares];
Bitboard king_attacks[numSquares];
Bitboard bishop_masks[numSquares];
Bitboard rook_masks[numSquares];
Bitboard bishop_attacks[numSquares][512];
Bitboard rook_attacks[numSquares][4096];

/*----------------------------------*/
/*          BISHOP AND ROOK         */
/*----------------------------------*/

enum bishopOrRook {rook, bishop};

// generates a bitmask for bishop at specified square to use for magic bitboard
Bitboard genBishopMask(int square){
    Bitboard mask = 0ULL;

    int sRank = square / 8;
    int sFile = square % 8;
    int i, j;

    for(i = sRank + 1, j = sFile + 1; i < 7 && j < 7; i++, j++){
        set(mask, (i*8 + j));
    }

    for(i = sRank - 1, j = sFile - 1; i > 0 && j > 0; i--, j--){
        set(mask, (i*8 + j));
    }

    for(i = sRank + 1, j = sFile - 1; i < 7 && j > 0; i++, j--){
        set(mask, (i*8 + j));
    }

    for(i = sRank - 1, j = sFile + 1; i > 0 && j < 7; i--, j++){
        set(mask, (i*8 + j));
    }

    return mask;
}

//bit counts for all masks generated by genBishopMask()
const int bishopMaskBitcount[64] = {
    6, 5, 5, 5, 5, 5, 5, 6, 
    5, 5, 5, 5, 5, 5, 5, 5, 
    5, 5, 7, 7, 7, 7, 5, 5, 
    5, 5, 7, 9, 9, 7, 5, 5, 
    5, 5, 7, 9, 9, 7, 5, 5, 
    5, 5, 7, 7, 7, 7, 5, 5, 
    5, 5, 5, 5, 5, 5, 5, 5, 
    6, 5, 5, 5, 5, 5, 5, 6
};

// generates possible bishop moves until it hits a piece (Hits both colours, attacks will be handled during move generation)
Bitboard genBishopMoves(int square, Bitboard boardState){
    Bitboard moves = 0ULL;

    int sRank = square / 8;
    int sFile = square % 8;
    int i, j;

    for(i = sRank + 1, j = sFile + 1; i <= 7 && j <= 7; i++, j++){
        set(moves, (i*8 + j));
        if(get(boardState, (i*8 + j))) break;
    }

    for(i = sRank - 1, j = sFile - 1; i >= 0 && j >= 0; i--, j--){
        set(moves, (i*8 + j));
        if(get(boardState, (i*8 + j))) break;
    }

    for(i = sRank + 1, j = sFile - 1; i <= 7 && j >= 0; i++, j--){
        set(moves, (i*8 + j));
        if(get(boardState, (i*8 + j))) break;
    }

    for(i = sRank - 1, j = sFile + 1; i >= 0 && j <= 7; i--, j++){
        set(moves, (i*8 + j));
        if(get(boardState, (i*8 + j))) break;
    }

    return moves;
}

// generates a bitmask for rook at specified square to use for magic bitboard
Bitboard genRookMask(int square){
    Bitboard mask = 0ULL;

    int sRank = square / 8;
    int sFile = square % 8;
    int i, j;

    for(i = sRank + 1; i < 7; i++){
        set(mask, (i*8 + sFile));
    }

    for(i = sRank - 1; i > 0; i--){
        set(mask, (i*8 + sFile));
    }

    for(j = sFile + 1;j < 7; j++){
        set(mask, (sRank*8 + j));
    }

    for(j = sFile - 1;j > 0; j--){
        set(mask, (sRank*8 + j));
    }

    return mask;
}

//bit counts for all masks generated by genRookMask()
const int rookMaskBitcount[64] = {
    12, 11, 11, 11, 11, 11, 11, 12, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    12, 11, 11, 11, 11, 11, 11, 12
};

// generates possible rook moves until it hits a piece (Hits both colours, attacks will be handled during move generation)
Bitboard genRookMoves(int square, Bitboard boardState){
    Bitboard moves = 0ULL;

    int sRank = square / 8;
    int sFile = square % 8;
    int i, j;

    for(i = sRank + 1; i <= 7; i++){
        set(moves, (i*8 + sFile));
        if(get(boardState, (i*8 + sFile))) break;
    }

    for(i = sRank - 1; i >= 0; i--){
        set(moves, (i*8 + sFile));
        if(get(boardState, (i*8 + sFile))) break;
    }

    for(j = sFile + 1;j <= 7; j++){
        set(moves, (sRank*8 + j));
        if(get(boardState, (sRank*8 + j))) break;
    }

    for(j = sFile - 1;j >= 0; j--){
        set(moves, (sRank*8 + j));
        if(get(boardState, (sRank*8 + j))) break;
    }

    return moves;
}

/*----------------------------------*/
/*           MAGIC NUMBERS          */
/*----------------------------------*/

// Bishop magic numbers generated from find_magic() function
const Bitboard bishop_magics[64] = {
    0x40040844404084ULL,
    0x2004208a004208ULL,
    0x10190041080202ULL,
    0x108060845042010ULL,
    0x581104180800210ULL,
    0x2112080446200010ULL,
    0x1080820820060210ULL,
    0x3c0808410220200ULL,
    0x4050404440404ULL,
    0x21001420088ULL,
    0x24d0080801082102ULL,
    0x1020a0a020400ULL,
    0x40308200402ULL,
    0x4011002100800ULL,
    0x401484104104005ULL,
    0x801010402020200ULL,
    0x400210c3880100ULL,
    0x404022024108200ULL,
    0x810018200204102ULL,
    0x4002801a02003ULL,
    0x85040820080400ULL,
    0x810102c808880400ULL,
    0xe900410884800ULL,
    0x8002020480840102ULL,
    0x220200865090201ULL,
    0x2010100a02021202ULL,
    0x152048408022401ULL,
    0x20080002081110ULL,
    0x4001001021004000ULL,
    0x800040400a011002ULL,
    0xe4004081011002ULL,
    0x1c004001012080ULL,
    0x8004200962a00220ULL,
    0x8422100208500202ULL,
    0x2000402200300c08ULL,
    0x8646020080080080ULL,
    0x80020a0200100808ULL,
    0x2010004880111000ULL,
    0x623000a080011400ULL,
    0x42008c0340209202ULL,
    0x209188240001000ULL,
    0x400408a884001800ULL,
    0x110400a6080400ULL,
    0x1840060a44020800ULL,
    0x90080104000041ULL,
    0x201011000808101ULL,
    0x1a2208080504f080ULL,
    0x8012020600211212ULL,
    0x500861011240000ULL,
    0x180806108200800ULL,
    0x4000020e01040044ULL,
    0x300000261044000aULL,
    0x802241102020002ULL,
    0x20906061210001ULL,
    0x5a84841004010310ULL,
    0x4010801011c04ULL,
    0xa010109502200ULL,
    0x4a02012000ULL,
    0x500201010098b028ULL,
    0x8040002811040900ULL,
    0x28000010020204ULL,
    0x6000020202d0240ULL,
    0x8918844842082200ULL,
    0x4010011029020020ULL,
};

// Rook magic numbers generated from find_magic() function
const Bitboard rook_magics[64] = {
    0x8a80104000800020ULL,
    0x140002000100040ULL,
    0x2801880a0017001ULL,
    0x100081001000420ULL,
    0x200020010080420ULL,
    0x3001c0002010008ULL,
    0x8480008002000100ULL,
    0x2080088004402900ULL,
    0x800098204000ULL,
    0x2024401000200040ULL,
    0x100802000801000ULL,
    0x120800800801000ULL,
    0x208808088000400ULL,
    0x2802200800400ULL,
    0x2200800100020080ULL,
    0x801000060821100ULL,
    0x80044006422000ULL,
    0x100808020004000ULL,
    0x12108a0010204200ULL,
    0x140848010000802ULL,
    0x481828014002800ULL,
    0x8094004002004100ULL,
    0x4010040010010802ULL,
    0x20008806104ULL,
    0x100400080208000ULL,
    0x2040002120081000ULL,
    0x21200680100081ULL,
    0x20100080080080ULL,
    0x2000a00200410ULL,
    0x20080800400ULL,
    0x80088400100102ULL,
    0x80004600042881ULL,
    0x4040008040800020ULL,
    0x440003000200801ULL,
    0x4200011004500ULL,
    0x188020010100100ULL,
    0x14800401802800ULL,
    0x2080040080800200ULL,
    0x124080204001001ULL,
    0x200046502000484ULL,
    0x480400080088020ULL,
    0x1000422010034000ULL,
    0x30200100110040ULL,
    0x100021010009ULL,
    0x2002080100110004ULL,
    0x202008004008002ULL,
    0x20020004010100ULL,
    0x2048440040820001ULL,
    0x101002200408200ULL,
    0x40802000401080ULL,
    0x4008142004410100ULL,
    0x2060820c0120200ULL,
    0x1001004080100ULL,
    0x20c020080040080ULL,
    0x2935610830022400ULL,
    0x44440041009200ULL,
    0x280001040802101ULL,
    0x2100190040002085ULL,
    0x80c0084100102001ULL,
    0x4024081001000421ULL,
    0x20030a0244872ULL,
    0x12001008414402ULL,
    0x2006104900a0804ULL,
    0x1004081002402ULL,
};

// returns occupancy map for each index from 0 to 4095
Bitboard mapOccupancy(int index, int bits, Bitboard mask){
    Bitboard map = 0ULL;

    for(int c = 0; c < bits; c++){
        int square = findLSB(mask);
        clear(mask, square);
        if(index & (1 << c)) set(map, square);
    }

    return map;
}

unsigned int randNum = 1804289383;

// 32 bit random number generator
unsigned int random32(){
    unsigned int num = randNum;

    num ^= num << 13;
    num ^= num >> 17;
    num ^= num << 5;

    randNum = num;
    
    return num;
}

//64 bit random number generator
unsigned long long random64(){
    unsigned long long u1, u2, u3, u4;
    
    u1 = (unsigned long long)(random32()) & 0xFFFF;
    u2 = (unsigned long long)(random32()) & 0xFFFF;
    u3 = (unsigned long long)(random32()) & 0xFFFF;
    u4 = (unsigned long long)(random32()) & 0xFFFF;

    return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

// macro to generate magic number candidates
#define generateMagic() (random64() & random64() & random64())

Bitboard find_magic(int square, int bits, int isBishop){
    Bitboard occupancies[4096];
    Bitboard attacks[4096];
    Bitboard used_atks[4096];
    Bitboard atk_mask = isBishop ? genBishopMask(square):genRookMask(square);

    int occ_indices = 1 << bits;

    for(int index = 0; index < occ_indices; index++){
        occupancies[index] = mapOccupancy(index, bits, atk_mask);

        attacks[index] = isBishop ? genBishopMoves(square, occupancies[index]) : genRookMoves(square, occupancies[index]);
    }

    for(int mag_count = 0; mag_count < 100000000; mag_count++){
        Bitboard magic = generateMagic();

        if(countBits((magic * atk_mask) & RANK_1) < 6) continue;

        std::memset(used_atks, 0ULL, sizeof(used_atks));

        int ind, fail;

        for(ind = 0, fail = 0; !fail && ind < occ_indices; ind++){
            int magic_index = (int)((occupancies[ind] * magic) >> (64 - bits));

            if(used_atks[magic_index] == 0ULL){
                used_atks[magic_index] = attacks[ind];
            }else if(used_atks[magic_index] != attacks[ind]){
                fail = 1;
            }
        }

        if(!fail){
            return magic;
        }
    }

    return 0ULL;
}

void init_magic_numbers(){
    for(int square = 0; square < 64; square++){
        std::cout << "0x" << std::hex << find_magic(square, rookMaskBitcount[square], rook) << "ULL," << std::endl;
    }

    std::cout << std::endl << std::endl;

    for(int square = 0; square < 64; square++){
        std::cout << "0x" << std::hex << find_magic(square, bishopMaskBitcount[square], bishop) << "ULL," << std::endl;
    }
}

/*----------------------------------*/
/*              ATTACKS             */
/*----------------------------------*/

// generates possible W/B pawn moves at specified square
Bitboard genPawnAttacks(Colour c, int square){
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
Bitboard genKnightMoves(int square){
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

// generates possible king moves at specified square
Bitboard genKingMoves(int square){
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

void initLeaperAttacks(){
    for(int i = 0; i < 64; i++){
        pawn_attacks[White][i] = genPawnAttacks(White, i);
        pawn_attacks[Black][i] = genPawnAttacks(Black, i);
        
        king_attacks[i] = genKingMoves(i);

        knight_attacks[i] = genKnightMoves(i);
    }
}

void initSliderAttacks(){
    Bitboard attack_mask, occupancy;
    int occ_indice;
    int magic_index;

    for(int square = 0; square < 64; square++){
        bishop_masks[square] = genBishopMask(square);
        attack_mask = bishop_masks[square];
        occ_indice = 1 << bishopMaskBitcount[square];

        for(int index = 0; index < occ_indice; index++){
            occupancy = mapOccupancy(index, bishopMaskBitcount[square], attack_mask);
            magic_index = (occupancy * bishop_magics[square]) >> (64 - bishopMaskBitcount[square]);
            bishop_attacks[square][magic_index] = genBishopMoves(square, occupancy);
        }
    }

    for(int square = 0; square < 64; square++){
        rook_masks[square] = genRookMask(square);
        attack_mask = rook_masks[square];
        occ_indice = 1 << rookMaskBitcount[square];

        for(int index = 0; index < occ_indice; index++){
            occupancy = mapOccupancy(index, rookMaskBitcount[square], attack_mask);
            magic_index = (occupancy * rook_magics[square]) >> (64 - rookMaskBitcount[square]);
            rook_attacks[square][magic_index] = genRookMoves(square, occupancy);
        }
    }
}

// get bishop attacks
static inline Bitboard getBishopAttacks(int square, Bitboard occupancy){
    occupancy &= bishop_masks[square];
    occupancy *= bishop_magics[square];
    occupancy >>= 64 - bishopMaskBitcount[square];

    return bishop_attacks[square][occupancy];
}

// get rook attacks
static inline Bitboard getRookAttacks(int square, Bitboard occupancy){
    occupancy &= rook_masks[square];
    occupancy *= rook_magics[square];
    occupancy >>= 64 - rookMaskBitcount[square];

    return rook_attacks[square][occupancy];
}

// get queen attacks
static inline Bitboard getQueenAttacks(int square, Bitboard occupancy){
    return getBishopAttacks(square, occupancy) | getRookAttacks(square, occupancy);
}

/*----------------------------------*/
/*       BOARD REPRESENTATION       */
/*----------------------------------*/

class Chessboard{
    public:
    // Bitboard for each piece
    Bitboard pieceBoards[numPieces] = {0};
    //White, Black, and all occupancies
    Bitboard occupancies[3] = {0};
    
    Colour side = White;
    int enpassant = no_sq;
    int castle = 0;

    Chessboard(){
        // Init white pieces
        pieceBoards[P] = RANK_2;
        pieceBoards[N] = sqr(B1) | sqr(G1);
        pieceBoards[B] = sqr(C1) | sqr(F1);
        pieceBoards[R] = sqr(A1) | sqr(H1);
        pieceBoards[Q] = sqr(D1);
        pieceBoards[K] = sqr(E1);

        // Init black pieces
        pieceBoards[p] = RANK_7;
        pieceBoards[n] = sqr(B8) | sqr(G8);
        pieceBoards[b] = sqr(C8) | sqr(F8);
        pieceBoards[r] = sqr(A8) | sqr(H8);
        pieceBoards[q] = sqr(D8);
        pieceBoards[k] = sqr(E8);

        for(int i = 0; i < 6; i++){
            occupancies[White] |= pieceBoards[i];
            occupancies[Black] |= pieceBoards[i + 6];
        }

        occupancies[Both] = occupancies[White] | occupancies[Black];
        side = White;
        enpassant = no_sq;
        castle = wKingside | wQueenside | bKingside | bQueenside;
    }

    // Chessboard initializer with a FEN string
    Chessboard(std::string fen){
        int square = 0;
        int index = 0;
        while(square < numSquares){
            if(isalpha(fen[index])){
                int piece = charToPiece[fen[index]];

                pieceBoards[piece] |= sqr(square);
                square++;
            }else if(isdigit(fen[index])){
                square += (fen[index] - '0');
            }

            index++;
        }

        index++;
        side = fen[index] == 'w' ? White:Black;
        index += 2;

        while(fen[index] != ' '){
            switch (fen[index])
            {
            case 'K':
                castle |= wKingside;
                break;
            case 'Q':
                castle |= wQueenside;
                break;
            case 'k':
                castle |= bKingside;
                break;
            case 'q':
                castle |= bQueenside;
                break;
            default:
                break;
            }

            index++;
        }
        index++;

        if(fen[index] != '-'){
            int file = fen[index++] - 'a';
            int rank = 8 - (fen[index] - '0');

            enpassant = rank*8 + file;            
        }else{
            enpassant = no_sq;
        }

        for(int i = 0; i < 6; i++){
            occupancies[White] |= pieceBoards[i];
            occupancies[Black] |= pieceBoards[i + 6];
        }

        occupancies[Both] = occupancies[White] | occupancies[Black];
    }

    /*----------------------------------*/
    /*          MOVE GENERATION         */
    /*----------------------------------*/

    //Checks if square is attacked by given side
    inline bool isAttacked(int square, int side){
        // Checks if square is attacked by white pawn
        if(side == White && (pawn_attacks[Black][square] & pieceBoards[P])) return true;
        // Checks if square is attacked by black pawn
        if(side == Black && (pawn_attacks[White][square] & pieceBoards[p])) return true;

        // Checks if square is attacked by knight
        if(knight_attacks[square] & (side ? pieceBoards[n]:pieceBoards[N])) return true;

        // Checks if square is attacked by king
        if(king_attacks[square] & (side ? pieceBoards[k]:pieceBoards[K])) return true;

        // Checks if square is attacked by bishop
        if(getBishopAttacks(square, occupancies[Both]) & (side ? pieceBoards[b]:pieceBoards[B])) return true;

        // Checks if square is attacked by bishop
        if(getRookAttacks(square, occupancies[Both]) & (side ? pieceBoards[r]:pieceBoards[R])) return true;

        // Checks if square is attacked by bishop
        if(getQueenAttacks(square, occupancies[Both]) & (side ? pieceBoards[q]:pieceBoards[Q])) return true;

        return false;
    }

    inline void generateMoves(){
        // Integers representing the initial and final squares of a piece after a move
        int from, to;
    }

    /*----------------------------------*/
    /*            BOARD DEBUG           */
    /*----------------------------------*/

    void printAttackedState(int side){
        for(int rank = 0; rank < 8; rank++){
            std::cout << 8 - rank << "   ";
            for(int file = 0; file < 8; file++){
                int square = rank*8 + file;

                std::cout << (isAttacked(square, side) ? 1:0) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        std::cout << "    A B C D E F G H" << std::endl << std::endl;
    }

    // debug print function
    void printChessboard(){
        for(int rank = 0; rank < 8; rank++){
            std::cout << 8 - rank << "   ";
            for(int file = 0; file < 8; file++){
                int square = rank*8 + file;
                int piece = -1;
                
                for(int i = P; i < numPieces; i++){
                    if(get(pieceBoards[i], square)){
                        piece = i;
                        break;
                    }
                }

                if(piece == -1){
                    std::cout << ". ";
                }else{
                    std::cout << pieceToChar[piece] << " ";
                }
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        std::cout << "    A B C D E F G H" << std::endl << std::endl;
        std::cout << "Side: " << (side ? "Black":"White") << std::endl;
        std::cout << "En Passant: " << (enpassant != no_sq ? toSquare[enpassant]:"None") << std::endl;
        std::cout << "Castling: " << (castle & wKingside ? "WK":"-") << ", " << (castle & wQueenside ? "WQ":"-") << ", " << (castle & bKingside ? "BK":"-") << ", " << (castle & bQueenside ? "BQ":"-") << std::endl << std::endl << std::endl;
    }
};

/*----------------------------------*/
/*          INITIALIZATION          */
/*----------------------------------*/

void init(){
    initLeaperAttacks();

    initSliderAttacks();
}

/*----------------------------------*/
/*               MAIN               */
/*----------------------------------*/

int main(){
    init();

    Chessboard chessboard = Chessboard(tricky_position);
    chessboard.printChessboard();
    chessboard.printAttackedState(Black);

    return 0;
}