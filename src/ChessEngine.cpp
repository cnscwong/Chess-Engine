#include <bitset>
#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <unistd.h>
#include <windows.h>
#include <assert.h>
#include <bits/stdc++.h>

/*----------------------------------*/
/*             Bitboard             */
/*----------------------------------*/

// 8x8 chess board represented as 64 bits
#define Bitboard unsigned long long

enum fileIndex {FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H};
enum rankIndex {RANK_8, RANK_7, RANK_6, RANK_5, RANK_4, RANK_3, RANK_2, RANK_1};

// bitboards representing the files and ranks of a chessboard
constexpr Bitboard files[] = {
    0x0101010101010101,
    0x0202020202020202,
    0x0404040404040404,
    0x0808080808080808,
    0x1010101010101010,
    0x2020202020202020,
    0x4040404040404040,
    0x8080808080808080
};

constexpr Bitboard ranks[] = {
    0x00000000000000FF,
    0x000000000000FF00,
    0x0000000000FF0000,
    0x00000000FF000000,
    0x000000FF00000000,
    0x0000FF0000000000,
    0x00FF000000000000,
    0xFF00000000000000
};

constexpr Bitboard ALL_SET = 0xFFFFFFFFFFFFFFFF;

void init_sqr(){
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            std::cout << (1ULL << (i*8 + j)) << "ULL, ";
        }
        std::cout << std::endl;
    }
}

//Lookup table where sqr[index] represents 1ULL << index
Bitboard sqr[64] = {1ULL, 2ULL, 4ULL, 8ULL, 16ULL, 32ULL, 64ULL, 128ULL, 
256ULL, 512ULL, 1024ULL, 2048ULL, 4096ULL, 8192ULL, 16384ULL, 32768ULL,
65536ULL, 131072ULL, 262144ULL, 524288ULL, 1048576ULL, 2097152ULL, 4194304ULL, 8388608ULL,
16777216ULL, 33554432ULL, 67108864ULL, 134217728ULL, 268435456ULL, 536870912ULL, 1073741824ULL, 2147483648ULL, 
4294967296ULL, 8589934592ULL, 17179869184ULL, 34359738368ULL, 68719476736ULL, 137438953472ULL, 274877906944ULL, 549755813888ULL,
1099511627776ULL, 2199023255552ULL, 4398046511104ULL, 8796093022208ULL, 17592186044416ULL, 35184372088832ULL, 70368744177664ULL, 140737488355328ULL, 
281474976710656ULL, 562949953421312ULL, 1125899906842624ULL, 2251799813685248ULL, 4503599627370496ULL, 9007199254740992ULL, 18014398509481984ULL, 36028797018963968ULL,
72057594037927936ULL, 144115188075855872ULL, 288230376151711744ULL, 576460752303423488ULL, 1152921504606846976ULL, 2305843009213693952ULL, 4611686018427387904ULL, 9223372036854775808ULL};

// Bitboard macros
#define set(board, square) ((board) |= sqr[square])
#define getSquare(board, square) (((board) & sqr[square]) ? 1:0)
#define clear(board, square) ((board) &= ~sqr[square])

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

// Lookup table for square to string representation
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

// Enum for adding a number to a square to move in a certain direction
enum Direction {UP = -8, DOWN = 8, LEFT = -1, RIGHT = 1};

// Enum used to track colour of pieces... etc.
enum Colour {White, Black, Both};

// returns enemy side
static inline int getEnemy(int side){
    return side^1;
}

// Enum used to track piece type, capitalized represents white
enum Pieces {P, N, B, R, Q, K, p, n, b, r, q, k, numPieces};

// Character representations of pieces
const std::string pieceToChar = "PNBRQKpnbrqk";

// Char to piece
std::map<char, int> charToPiece = {{'P', P}, {'N', N}, {'B', B}, {'R', R}, {'Q', Q}, {'K', K}, {'p', p}, {'n', n}, {'b', b}, {'r', r}, {'q', q}, {'k', k}};

// Enum for castling rights
enum Castling {wKingside = 1, wQueenside = 2, bKingside = 4, bQueenside = 8, ALL_CASTLE_SET = 15};

// FEN debug positions
#define empty_board "8/8/8/8/8/8/8/8 w - - "
#define start_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
#define killer_position "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"

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
            std::cout << getSquare(board, (rank*8 + file)) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << "    A B C D E F G H\n\nULL:" << board << std::endl << std::endl;
}

/*----------------------------------*/
/*          RANDOM NUMBERS          */
/*----------------------------------*/

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

/*----------------------------------*/
/*        ZOBRIST RANDOM KEYS       */
/*----------------------------------*/

// random hash keys
// random piece hash keys[piece][square]
Bitboard piece_keys[12][64];
// random enpassant keys[square]
Bitboard enpassant_keys[64];
// random castling keys
Bitboard castle_keys[16];
// random side key
Bitboard side_key;

void init_random_keys(){
    randNum = 1804289383;

    for(int i = 0; i < 12; i++){
        for(int j = 0; j < 64; j++){
            piece_keys[i][j] = random64();
        }
    }

    for(int i = 0; i < 64; i++){
        enpassant_keys[i] = random64();
    }

    for(int i = 0; i < 16; i++){
        castle_keys[i] = random64();
    }

    side_key = random64();
}

/*----------------------------------*/
/*           TIME CONTROL           */
/*----------------------------------*/

// BlueFeverSoftware implementation

bool quit = false;

// UCI movestogo move counter
int movestogo = 30;

// UCI movetime command time counter
int movetime = -1;

// UCI time command holder (ms)
int timeTracker = -1;

// UCI inc command's time increment holder
int inc = 0;

//UCI starttime command time holder
int starttime = 0;

// UCI stoptime command time holder
int stoptime = 0;

// variable to flag time control availability
bool timeset = false;

// variable to flag when the time is up
bool stopped = false;

// Gets current ms time
int get_time_ms(){
    return GetTickCount();
}

int input_waiting(){
    static int init = 0, pipe;
    static HANDLE inh;
    DWORD dw;

    if (!init)
    {
        init = 1;
        inh = GetStdHandle(STD_INPUT_HANDLE);
        pipe = !GetConsoleMode(inh, &dw);
        if (!pipe)
        {
            SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT));
            FlushConsoleInputBuffer(inh);
        }
    }
    
    if (pipe)
    {
        if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL)) return 1;
        return dw;
    }
    
    else
    {
        GetNumberOfConsoleInputEvents(inh, &dw);
        return dw <= 1 ? 0 : dw;
    }
}

// read GUI/user input
void read_input()
{
    // bytes to read holder
    int bytes;
    
    // GUI/user input
    char input[256] = "", *endc;

    // "listen" to STDIN
    if (input_waiting())
    {
        // tell engine to stop calculating
        stopped = true;
        
        // loop to read bytes from STDIN
        do
        {
            // read bytes from STDIN
            bytes=read(fileno(stdin), input, 256);
        }
        
        // until bytes available
        while (bytes < 0);
        
        // searches for the first occurrence of '\n'
        endc = strchr(input,'\n');
        
        // if found new line set value at pointer to 0
        if (endc) *endc=0;
        
        // if input is available
        if (strlen(input) > 0)
        {
            // match UCI "quit" command
            if (!strncmp(input, "quit", 4))
            {
                // tell engine to terminate exacution    
                quit = true;
            }

            // // match UCI "stop" command
            else if (!strncmp(input, "stop", 4))    {
                // tell engine to terminate exacution
                quit = true;
            }
        }   
    }
}

// a bridge function to interact between search and GUI input
static void communicate() {
	// if time is up break here
    if(timeset && get_time_ms() > stoptime) {
		// tell engine to stop calculating
		stopped = true;
	}
	
    // read GUI input
	read_input();
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
        if(getSquare(boardState, (i*8 + j))) break;
    }

    for(i = sRank - 1, j = sFile - 1; i >= 0 && j >= 0; i--, j--){
        set(moves, (i*8 + j));
        if(getSquare(boardState, (i*8 + j))) break;
    }

    for(i = sRank + 1, j = sFile - 1; i <= 7 && j >= 0; i++, j--){
        set(moves, (i*8 + j));
        if(getSquare(boardState, (i*8 + j))) break;
    }

    for(i = sRank - 1, j = sFile + 1; i >= 0 && j <= 7; i--, j++){
        set(moves, (i*8 + j));
        if(getSquare(boardState, (i*8 + j))) break;
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
        if(getSquare(boardState, (i*8 + sFile))) break;
    }

    for(i = sRank - 1; i >= 0; i--){
        set(moves, (i*8 + sFile));
        if(getSquare(boardState, (i*8 + sFile))) break;
    }

    for(j = sFile + 1;j <= 7; j++){
        set(moves, (sRank*8 + j));
        if(getSquare(boardState, (sRank*8 + j))) break;
    }

    for(j = sFile - 1;j >= 0; j--){
        set(moves, (sRank*8 + j));
        if(getSquare(boardState, (sRank*8 + j))) break;
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

        if(countBits((magic * atk_mask) & ranks[RANK_1]) < 6) continue;

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
        std::cout << "0x" << std::hex << find_magic(square, rookMaskBitcount[square], rook) << "ULL,\n";
    }

    std::cout << std::endl << std::endl;

    for(int square = 0; square < 64; square++){
        std::cout << "0x" << std::hex << find_magic(square, bishopMaskBitcount[square], bishop) << "ULL,\n";
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
        attack |= ~files[FILE_A] & temp >> 7;
        attack |= ~files[FILE_H] & temp >> 9;
    }else{
        attack |= ~files[FILE_A] & temp << 9;
        attack |= ~files[FILE_H] & temp << 7;
    }

    return attack;
}

// generates possible knight moves at specified square
Bitboard genKnightMoves(int square){
    Bitboard temp = 1ULL << square;
    Bitboard moves = 0ULL;

    moves |= ~files[FILE_A] & ~files[FILE_B] & temp >> 6;
    moves |= ~files[FILE_H] & ~files[FILE_G] & temp >> 10;
    moves |= ~files[FILE_A] & temp >> 15;
    moves |= ~files[FILE_H] & temp >> 17;

    moves |= ~files[FILE_H] & ~files[FILE_G] & temp << 6;
    moves |= ~files[FILE_A] & ~files[FILE_B] & temp << 10;
    moves |= ~files[FILE_H] & temp << 15;
    moves |= ~files[FILE_A] & temp << 17;

    return moves;
}

// generates possible king moves at specified square
Bitboard genKingMoves(int square){
    Bitboard temp = 1ULL << square;
    Bitboard moves = 0ULL;
    
    //right moves
    moves |= ~files[FILE_A] & temp >> 7;
    moves |= ~files[FILE_A] & temp << 1;
    moves |= ~files[FILE_A] & temp << 9;

    //up down
    moves |= temp >> 8;
    moves |= temp << 8;

    //left moves
    moves |= ~files[FILE_H] & temp >> 9;
    moves |= ~files[FILE_H] & temp >> 1;
    moves |= ~files[FILE_H] & temp << 7;

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
/*       MOVES REPRESENTATION       */
/*----------------------------------*/

// Flags for special move types
enum moveFlags {QUIET = 0, CAPTURE = 1, DOUBLE_PUSH = 2, ENPASSANT = 4, CASTLE = 8};
std::map<int, char> piecePromotion = {{N, 'n'}, {B, 'b'}, {R, 'r'}, {Q, 'q'}, {n, 'n'}, {b, 'b'}, {r, 'r'}, {q, 'q'}};

// Class to store all information needed for a move
class Move{
    public:
    // Squares involving the move
    int from, to;

    // Indicates what piece is being moved or what piece the pawn is being promoted to
    int piece, promotedPiece;

    int flags;

    Move(){
        from = no_sq;
        to = no_sq;
        piece = 0;
        promotedPiece = 0;
        flags = 0;
    }

    Move(int from, int to, int piece, int promotedPiece, int flags){
        this->from = from;
        this->to = to;
        this->piece = piece;
        this->promotedPiece = promotedPiece;
        this->flags = flags;
    }

    bool operator==(const Move &m){
        return this->from == m.from && this->to == m.to && this->piece == m.piece && this->promotedPiece == m.promotedPiece && this->flags == m.flags;
    }

    void print(){
        std::cout << toSquare[from] << toSquare[to] << piecePromotion[promotedPiece];
    }
};

// Class to store all possible moves for a chessboard state
class MoveList{
    public:
    Move moves[256];
    int count = 0;

    MoveList(){
        count = 0;
    }

    void print(){
        if(count == 0){
            std::cout << "Move list empty\n\n";
            return;
        }

        std::cout << "move\tpiece\tcapture\t  double    enpass\tcastle\n\n";
        for(int i = 0; i < count; i++){
            Move temp = moves[i];

            std::cout << toSquare[temp.from] << toSquare[temp.to] << piecePromotion[temp.promotedPiece] << \
            "\t" << pieceToChar[temp.piece] << "\t" << ((temp.flags & CAPTURE) ? 1:0) << \
            "\t  " << ((temp.flags & DOUBLE_PUSH) ? 1:0) << "         " << \
            ((temp.flags & ENPASSANT) ? 1:0) << "\t\t" << ((temp.flags & CASTLE) ? 1:0) << std::endl;
        }

        std::cout << "\n\nTotal moves: " << count << std::endl << std::endl;
    }

    inline void addMove(Move move){
        moves[count] = move;
        count++;
    }
};

/*----------------------------------*/
/*       BOARD REPRESENTATION       */
/*----------------------------------*/

class Chessboard{
    public:
    // Bitboard for each piece
    Bitboard pieceBoards[numPieces] = {0};
    //White, Black, and all occupancies
    Bitboard occupancies[3] = {0};
    
    int side = White;
    int enpassant = no_sq;
    int castle = 0;

    Bitboard hashKey = 0ULL;

    // Tracks old board positions
    Bitboard repetition_table[1000];
    int rep_ind = 0;

    // Default constructor initializes chessboard to starting position
    Chessboard(){
        side = White;
        enpassant = no_sq;
        castle = 0;

        rep_ind = 0;

        // Init white pieces
        pieceBoards[P] = ranks[RANK_2];
        pieceBoards[N] = sqr[B1] | sqr[G1];
        pieceBoards[B] = sqr[C1] | sqr[F1];
        pieceBoards[R] = sqr[A1] | sqr[H1];
        pieceBoards[Q] = sqr[D1];
        pieceBoards[K] = sqr[E1];

        // Init black pieces
        pieceBoards[p] = ranks[RANK_7];
        pieceBoards[n] = sqr[B8] | sqr[G8];
        pieceBoards[b] = sqr[C8] | sqr[F8];
        pieceBoards[r] = sqr[A8] | sqr[H8];
        pieceBoards[q] = sqr[D8];
        pieceBoards[k] = sqr[E8];

        for(int i = 0; i < 6; i++){
            occupancies[White] |= pieceBoards[i];
            occupancies[Black] |= pieceBoards[i + 6];
        }

        occupancies[Both] = occupancies[White] | occupancies[Black];
        side = White;
        enpassant = no_sq;
        castle = wKingside | wQueenside | bKingside | bQueenside;

        hashKey = generateHashKey();
    }

    // Chessboard initializer with a FEN string
    Chessboard(std::string fen){
        side = White;
        enpassant = no_sq;
        castle = 0;

        rep_ind = 0;

        int square = 0;
        int index = 0;
        while(square < numSquares){
            if(isalpha(fen[index])){
                int piece = charToPiece[fen[index]];

                pieceBoards[piece] |= sqr[square];
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

        hashKey = generateHashKey();
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

    // Generates all pseudolegal moves in current chessboard state
    inline void generateMoves(MoveList &move_list){
        move_list.count = 0;
        // Integers representing the initial and final squares of a piece after a move
        int from, to;
        bool canPromote;

        Bitboard pieceBitboard, attacks;

        for(int piece = P; piece <= k; piece++){
            pieceBitboard = pieceBoards[piece];

            if(side == White){
                if(piece == P){
                    while(pieceBitboard){
                        from = findLSB(pieceBitboard);
                        canPromote = sqr[from] & ranks[RANK_7];
                        to = from + UP;

                        if(to >= 0 && !getSquare(occupancies[Both], to)){
                            // Pawn promotion
                            if(canPromote){
                                move_list.addMove(Move(from, to, piece, Q, 0));
                                move_list.addMove(Move(from, to, piece, R, 0));
                                move_list.addMove(Move(from, to, piece, B, 0));
                                move_list.addMove(Move(from, to, piece, N, 0));
                            }else{
                                // Pawn push
                                move_list.addMove(Move(from, to, piece, 0, 0));
                                //Double pawn push
                                if((sqr[from] & ranks[RANK_2]) && !getSquare(occupancies[Both], to + UP)){
                                    move_list.addMove(Move(from, to + UP, piece, 0, DOUBLE_PUSH));
                                }
                            }
                        }

                        attacks = pawn_attacks[White][from] & occupancies[Black];

                        while(attacks){
                            to = findLSB(attacks);

                            if(canPromote){
                                move_list.addMove(Move(from, to, piece, Q, CAPTURE));
                                move_list.addMove(Move(from, to, piece, R, CAPTURE));
                                move_list.addMove(Move(from, to, piece, B, CAPTURE));
                                move_list.addMove(Move(from, to, piece, N, CAPTURE));
                            }else{
                                // Pawn push
                                move_list.addMove(Move(from, to, piece, 0, CAPTURE));
                            }

                            clear(attacks, to);
                        }

                        if(enpassant != no_sq){
                            Bitboard enpassant_attacks = pawn_attacks[White][from] & sqr[enpassant];

                            if(enpassant_attacks){
                                to = findLSB(enpassant_attacks);
                                move_list.addMove(Move(from, to, piece, 0, CAPTURE | ENPASSANT));
                            }
                        }

                        clear(pieceBitboard, from);
                    }
                }
            
                if(piece == K){
                    if(castle & wKingside){
                        if(!getSquare(occupancies[Both], F1) && !getSquare(occupancies[Both], G1)){
                            if(!this->isAttacked(E1, Black) && !this->isAttacked(F1, Black)){
                                move_list.addMove(Move(E1, G1, piece, 0, CASTLE));
                            }
                        }
                    }

                    if(castle & wQueenside){
                        if(!getSquare(occupancies[Both], B1) && !getSquare(occupancies[Both], C1) && !getSquare(occupancies[Both], D1)){
                            if(!this->isAttacked(D1, Black) && !this->isAttacked(E1, Black)){
                                move_list.addMove(Move(E1, C1, piece, 0, CASTLE));
                            }
                        }
                    }
                }
            }else{
                if(piece == p){
                    while(pieceBitboard){
                        from = findLSB(pieceBitboard);
                        canPromote = sqr[from] & ranks[RANK_2];

                        to = from + DOWN;

                        if(to >= 0 && !getSquare(occupancies[Both], to)){
                            // Pawn promotion
                            if(canPromote){
                                move_list.addMove(Move(from, to, piece, q, 0));
                                move_list.addMove(Move(from, to, piece, r, 0));
                                move_list.addMove(Move(from, to, piece, b, 0));
                                move_list.addMove(Move(from, to, piece, n, 0));
                            }else{
                                // Pawn push
                                move_list.addMove(Move(from, to, piece, 0, 0));
                                //Double pawn push
                                if((sqr[from] & ranks[RANK_7]) && !getSquare(occupancies[Both], to + DOWN)){
                                    move_list.addMove(Move(from, to + DOWN, piece, 0, DOUBLE_PUSH));
                                }
                            }
                        }

                        attacks = pawn_attacks[Black][from] & occupancies[White];

                        while(attacks){
                            to = findLSB(attacks);

                            if(canPromote){
                                move_list.addMove(Move(from, to, piece, q, CAPTURE));
                                move_list.addMove(Move(from, to, piece, r, CAPTURE));
                                move_list.addMove(Move(from, to, piece, b, CAPTURE));
                                move_list.addMove(Move(from, to, piece, n, CAPTURE));
                            }else{
                                // Pawn push
                                move_list.addMove(Move(from, to, piece, 0, CAPTURE));
                            }

                            clear(attacks, to);
                        }

                        if(enpassant != no_sq){
                            Bitboard enpassant_attacks = pawn_attacks[Black][from] & sqr[enpassant];

                            if(enpassant_attacks){
                                to = findLSB(enpassant_attacks);
                                move_list.addMove(Move(from, to, piece, 0, CAPTURE | ENPASSANT));
                            }
                        }

                        clear(pieceBitboard, from);
                    }
                }

                if(piece == k){
                    if(castle & bKingside){
                        if(!getSquare(occupancies[Both], F8) && !getSquare(occupancies[Both], G8)){
                            if(!this->isAttacked(E8, White) && !this->isAttacked(F8, White)){
                                move_list.addMove(Move(E8, G8, piece, 0, CASTLE));
                            }
                        }
                    }

                    if(castle & bQueenside){
                        if(!getSquare(occupancies[Both], B8) && !getSquare(occupancies[Both], C8) && !getSquare(occupancies[Both], D8)){
                            if(!this->isAttacked(D8, White) && !this->isAttacked(E8, White)){
                                move_list.addMove(Move(E8, C8, piece, 0, CASTLE));
                            }
                        }
                    }
                }
            }

            // Generate knight moves
            if(side == White ? piece == N:piece == n){
                while(pieceBitboard){
                    from = findLSB(pieceBitboard);
                    
                    attacks = knight_attacks[from] & ~occupancies[side];

                    while(attacks){
                        to = findLSB(attacks);

                        if(!getSquare(occupancies[getEnemy(side)], to)){
                            move_list.addMove(Move(from, to, piece, 0, 0));
                        }else{
                            move_list.addMove(Move(from, to, piece, 0, CAPTURE));
                        }

                        clear(attacks, to);
                    }

                    clear(pieceBitboard, from);
                }
            }

            // Generate bishop moves
            if(side == White ? piece == B:piece == b){
                while(pieceBitboard){
                    from = findLSB(pieceBitboard);
                    
                    attacks = getBishopAttacks(from, occupancies[Both]) & ~occupancies[side];

                    while(attacks){
                        to = findLSB(attacks);

                        if(!getSquare(occupancies[getEnemy(side)], to)){
                            move_list.addMove(Move(from, to, piece, 0, 0));
                        }else{
                            move_list.addMove(Move(from, to, piece, 0, CAPTURE));
                        }

                        clear(attacks, to);
                    }

                    clear(pieceBitboard, from);
                }
            }

            // Generate rook moves
            if(side == White ? piece == R:piece == r){
                while(pieceBitboard){
                    from = findLSB(pieceBitboard);
                    
                    attacks = getRookAttacks(from, occupancies[Both]) & ~occupancies[side];

                    while(attacks){
                        to = findLSB(attacks);

                        if(!getSquare(occupancies[getEnemy(side)], to)){
                            move_list.addMove(Move(from, to, piece, 0, 0));
                        }else{
                            move_list.addMove(Move(from, to, piece, 0, CAPTURE));
                        }

                        clear(attacks, to);
                    }

                    clear(pieceBitboard, from);
                }
            }

            // Generate queen moves
            if(side == White ? piece == Q:piece == q){
                while(pieceBitboard){
                    from = findLSB(pieceBitboard);
                    
                    attacks = getQueenAttacks(from, occupancies[Both]) & ~occupancies[side];

                    while(attacks){
                        to = findLSB(attacks);

                        if(!getSquare(occupancies[getEnemy(side)], to)){
                            move_list.addMove(Move(from, to, piece, 0, 0));
                        }else{
                            move_list.addMove(Move(from, to, piece, 0, CAPTURE));
                        }

                        clear(attacks, to);
                    }

                    clear(pieceBitboard, from);
                }
            }

            // Generate king moves
            if(side == White ? piece == K:piece == k){
                while(pieceBitboard){
                    from = findLSB(pieceBitboard);
                    
                    attacks = king_attacks[from] & ~occupancies[side];

                    while(attacks){
                        to = findLSB(attacks);

                        if(!getSquare(occupancies[getEnemy(side)], to)){
                            move_list.addMove(Move(from, to, piece, 0, 0));
                        }else{
                            move_list.addMove(Move(from, to, piece, 0, CAPTURE));
                        }

                        clear(attacks, to);
                    }

                    clear(pieceBitboard, from);
                }
            }
        }
    }

    /*----------------------------------*/
    /*          CHESSBOARD DEBUG        */
    /*----------------------------------*/

    // Prints which squares are attacked by given side in the current chessboard 
    void printAttackedState(int side){
        for(int rank = 0; rank < 8; rank++){
            std::cout << 8 - rank << "   ";
            for(int file = 0; file < 8; file++){
                int square = rank*8 + file;

                std::cout << (isAttacked(square, side) ? 1:0) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "\n    A B C D E F G H\n\n";
    }

    // debug print function
    void printChessboard(){
        for(int rank = 0; rank < 8; rank++){
            std::cout << 8 - rank << "   ";
            for(int file = 0; file < 8; file++){
                int square = rank*8 + file;
                int piece = -1;
                
                for(int i = P; i < numPieces; i++){
                    if(getSquare(pieceBoards[i], square)){
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
        std::cout << "\n    A B C D E F G H\n\n";
        std::cout << "Side: " << (side ? "Black":"White") << std::endl;
        std::cout << "En Passant: " << (enpassant != no_sq ? toSquare[enpassant]:"None") << std::endl;
        std::cout << "Castling: " << (castle & wKingside ? "WK":"-") << ", " << (castle & wQueenside ? "WQ":"-") << ", " << (castle & bKingside ? "BK":"-") << ", " << (castle & bQueenside ? "BQ":"-") << std::endl;
        std::cout << "Hash key: " << std::hex << hashKey << std::dec << std::endl << std::endl << std::endl;
    }

    /*----------------------------------*/
    /*          ZOBRIST HASHING         */
    /*----------------------------------*/

    Bitboard generateHashKey(){
        Bitboard key = 0ULL;

        Bitboard tempBoard;

        for(int piece = P; piece <= k; piece++){
            tempBoard = pieceBoards[piece];

            while(tempBoard){
                int square = findLSB(tempBoard);

                key ^= piece_keys[piece][square];

                clear(tempBoard, square);
            }
        }

        if(enpassant != no_sq){
            key ^= enpassant_keys[enpassant];
        }

        key ^= castle_keys[castle];

        if(side == Black){
            key ^= side_key;
        }

        return key;
    }
};

/*----------------------------------*/
/*           MAKING MOVES           */
/*----------------------------------*/

// flag to indicate what type the move is in makeMove function
enum { all, captures};

// Array for updating castling rights after every move
const int castling_rights[64] = {
     7, 15, 15, 15,  3, 15, 15, 11, 
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    13, 15, 15, 15, 12, 15, 15, 14,
};

class BoardContainer{
    public:
    Chessboard board;
    Chessboard copy;

    BoardContainer(){
        board = Chessboard();
        copy = Chessboard();
    }

    BoardContainer(std::string fen){
        board = Chessboard(fen);
        copy = Chessboard(fen);
    }

    // takes a copy of the board
    inline void saveBoard(){
        copy = board;
    }

    // restores the board to the last saved state
    inline void restoreBoard(){
        board = copy;
    }

    // Makes a given move on the global board variable
    inline int makeMove(Move move, int moveFlag){
        bool isWhite = board.side == White;

        if(moveFlag == all){
            saveBoard();

            // Changing piece boards
            clear(board.pieceBoards[move.piece], move.from);
            set(board.pieceBoards[move.piece], move.to);

            // Changing occupancy boards
            clear(board.occupancies[board.side], move.from);
            set(board.occupancies[board.side], move.to);

            // Changing hash key
            board.hashKey ^= piece_keys[move.piece][move.from];
            board.hashKey ^= piece_keys[move.piece][move.to];

            if(move.flags & CAPTURE){
                int startInd, endInd;
                if(isWhite){
                    startInd = p;
                    endInd = k;
                }else{
                    startInd = P;
                    endInd = K;
                }

                for(int piece = startInd; piece <= endInd; piece++){
                    if(getSquare(board.pieceBoards[piece], move.to)){
                        // Remove captured piece from pieceBoard
                        clear(board.pieceBoards[piece], move.to);

                        // Remove captured piece from hash key
                        board.hashKey ^= piece_keys[piece][move.to];
                        break;
                    }
                }

                // Remove captured piece from occupancies
                clear(board.occupancies[getEnemy(board.side)], move.to);
            }

            if(move.promotedPiece){
                if(isWhite){
                    clear(board.pieceBoards[P], move.to);
                    board.hashKey ^= piece_keys[P][move.to];
                }else{
                    clear(board.pieceBoards[p], move.to);
                    board.hashKey ^= piece_keys[p][move.to];
                }
                
                set(board.pieceBoards[move.promotedPiece], move.to);
                board.hashKey ^= piece_keys[move.promotedPiece][move.to];
            }

            if(move.flags & ENPASSANT){
                if(isWhite){
                    clear(board.pieceBoards[p], move.to + DOWN);
                    clear(board.occupancies[Black], move.to + DOWN);
                    board.hashKey ^= piece_keys[p][move.to + DOWN];
                }else{
                    clear(board.pieceBoards[P], move.to + UP);
                    clear(board.occupancies[White], move.to + UP);
                    board.hashKey ^= piece_keys[P][move.to + UP];
                }
            }

            if(board.enpassant != no_sq){
                board.hashKey ^= enpassant_keys[board.enpassant];
            }

            board.enpassant = no_sq;

            if(move.flags & DOUBLE_PUSH){
                (isWhite) ? (board.enpassant = move.to + DOWN):(board.enpassant = move.to + UP);
                board.hashKey ^= enpassant_keys[board.enpassant];
            }

            if(move.flags & CASTLE){
                switch(move.to){
                    case(G1):
                        clear(board.pieceBoards[R], H1);
                        set(board.pieceBoards[R], F1);
                        clear(board.occupancies[board.side], H1);
                        set(board.occupancies[board.side], F1);
                        board.hashKey ^= piece_keys[R][H1];
                        board.hashKey ^= piece_keys[R][F1];
                        break;
                    case(C1):
                        clear(board.pieceBoards[R], A1);
                        set(board.pieceBoards[R], D1);
                        clear(board.occupancies[board.side], A1);
                        set(board.occupancies[board.side], D1);
                        board.hashKey ^= piece_keys[R][A1];
                        board.hashKey ^= piece_keys[R][D1];
                        break;
                    case(G8):
                        clear(board.pieceBoards[r], H8);
                        set(board.pieceBoards[r], F8);
                        clear(board.occupancies[board.side], H8);
                        set(board.occupancies[board.side], F8);
                        board.hashKey ^= piece_keys[r][H8];
                        board.hashKey ^= piece_keys[r][F8];
                        break;
                    case(C8):
                        clear(board.pieceBoards[r], A8);
                        set(board.pieceBoards[r], D8);
                        clear(board.occupancies[board.side], A8);
                        set(board.occupancies[board.side], D8);
                        board.hashKey ^= piece_keys[r][A8];
                        board.hashKey ^= piece_keys[r][D8];
                        break;
                }
            }

            // Unhash the last castle state
            board.hashKey ^= castle_keys[board.castle];

            board.castle &= castling_rights[move.from];
            board.castle &= castling_rights[move.to];

            // Hash the new castle state
            board.hashKey ^= castle_keys[board.castle];

            board.occupancies[Both] = board.occupancies[White] | board.occupancies[Black];

            board.side = getEnemy(board.side);

            board.hashKey ^= side_key;

            if(board.isAttacked(!isWhite ? findLSB(board.pieceBoards[k]):findLSB(board.pieceBoards[K]), board.side)){
                restoreBoard();

                return 0;
            }else{
                return 1;
            }
        }else{
            if(move.flags & CAPTURE){
                return makeMove(move, all);
            }else{
                return 0;
            }
        }
    }
};

/*----------------------------------*/
/*        PERFORMANCE TESTING       */
/*----------------------------------*/

const int perftDepth = 5;
unsigned long long node_count[perftDepth + 1] = {0};

// Counts all possible moves for a certain depth to compare with a working chess engine move generator to see if there are any bugs
static inline void perftDriver(int depth, BoardContainer boards){
    node_count[depth]++;
    if(depth == 0){
        return;
    }

    MoveList move_list;

    boards.board.generateMoves(move_list);

    for(int moveIndex = 0; moveIndex < move_list.count; moveIndex++){
        boards.saveBoard();

        if(!boards.makeMove(move_list.moves[moveIndex], all)){
            continue;
        }

        perftDriver(depth - 1, boards);

        boards.restoreBoard();
    }
}

// Outputs counts per node for better debugging in case of move gen bug
void perftTest(int depth, BoardContainer boards){
    std::cout << "Performance Test\n\n";

    MoveList move_list;

    boards.board.generateMoves(move_list);

    long start_time = get_time_ms();

    for(int moveIndex = 0; moveIndex < move_list.count; moveIndex++){
        boards.saveBoard();

        if(!boards.makeMove(move_list.moves[moveIndex], all)){
            continue;
        }

        unsigned long long old_nodes = node_count[0];

        perftDriver(depth - 1, boards);

        unsigned long long nodesPerDriver = node_count[0] - old_nodes;

        boards.restoreBoard();

        std::cout << "Move: " << toSquare[move_list.moves[moveIndex].from] << toSquare[move_list.moves[moveIndex].to] << 
        piecePromotion[move_list.moves[moveIndex].promotedPiece] << "   Nodes: " << std::dec << nodesPerDriver << std::endl;
    }

    std::cout << "\nDepth: " << depth << std::endl;
    std::cout << "Nodes: " << node_count[0] << std::endl;
    std::cout << "Time: " << get_time_ms() - start_time << std::endl << std::endl;
}

// Checks if the move gen is bug free
void moveGenBugCheck(){
    BoardContainer boards = BoardContainer();

    perftDriver(perftDepth, boards);
    assert(node_count[0] == 4865609);
    assert(node_count[1] == 197281);
    assert(node_count[2] == 8902);
    assert(node_count[3] == 400);
    assert(node_count[4] == 20);
    assert(node_count[5] == 1);

    memset(node_count, 0, sizeof(node_count));

    boards = BoardContainer(tricky_position);

    perftDriver(perftDepth, boards);
    assert(node_count[0] == 193690690);
    assert(node_count[1] == 4085603);
    assert(node_count[2] == 97862);
    assert(node_count[3] == 2039);
    assert(node_count[4] == 48);
    assert(node_count[5] == 1);
    std::cout << "Passed! Move Generator is bug free!\n\n";
}

// Perft test to find average time of perftDriver after n attempts
void moveGenAverageTime(std::string fenPos){
    BoardContainer boards = BoardContainer(fenPos);
    int total_time = get_time_ms();
    for(int i = 0; i < 20; i++){
        int start_time = get_time_ms();
        perftDriver(perftDepth, boards);
        std::cout << "Time " << i << ": " << get_time_ms() - start_time << std::endl;
    }
    std::cout << "Average time: " << (get_time_ms() - total_time)/20.0 << std::endl;
}

/*----------------------------------*/
/*            EVALUATION            */
/*----------------------------------*/

// Scores for each instance of the piece being alive on the board
constexpr int materialScore[numPieces] = {
    [P] = 100,
    [N] = 300,
    [B] = 350,
    [R] = 500,
    [Q] = 1000,
    [K] = 10000,
    [p] = -100,
    [n] = -300,
    [b] = -350,
    [r] = -500,
    [q] = -1000,
    [k] = -10000,
};

// pawn positional score
constexpr int pawn_score[64] = 
{
    90,  90,  90,  90,  90,  90,  90,  90,
    30,  30,  30,  40,  40,  30,  30,  30,
    20,  20,  20,  30,  30,  30,  20,  20,
    10,  10,  10,  20,  20,  10,  10,  10,
     5,   5,  10,  20,  20,   5,   5,   5,
     0,   0,   0,   5,   5,   0,   0,   0,
     0,   0,   0, -10, -10,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0
};

// knight positional score
constexpr int knight_score[64] = 
{
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5,   0,   0,  10,  10,   0,   0,  -5,
    -5,   5,  20,  20,  20,  20,   5,  -5,
    -5,  10,  20,  30,  30,  20,  10,  -5,
    -5,  10,  20,  30,  30,  20,  10,  -5,
    -5,   5,  20,  10,  10,  20,   5,  -5,
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5, -10,   0,   0,   0,   0, -10,  -5
};

// bishop positional score
constexpr int bishop_score[64] = 
{
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,  20,   0,  10,  10,   0,  20,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,  10,   0,   0,   0,   0,  10,   0,
     0,  30,   0,   0,   0,   0,  30,   0,
     0,   0, -10,   0,   0, -10,   0,   0

};

// rook positional score
constexpr int rook_score[64] =
{
    50,  50,  50,  50,  50,  50,  50,  50,
    50,  50,  50,  50,  50,  50,  50,  50,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,   0,  20,  20,   0,   0,   0

};

// king positional score
constexpr int king_score[64] = 
{
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   5,   5,   5,   5,   0,   0,
     0,   5,   5,  10,  10,   5,   5,   0,
     0,   5,  10,  20,  20,  10,   5,   0,
     0,   5,  10,  20,  20,  10,   5,   0,
     0,   0,   5,  10,  10,   5,   0,   0,
     0,   5,   5,  -5,  -5,   0,   5,   0,
     0,   0,   5,   0, -15,   0,  10,   0
};

// mirror positional score tables for opposite side
const int mirror_score[64] =
{
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8
};

// Masks for pawn structure evaluation
// file mask [square]
Bitboard fileMask[64];
// rank mask [square]
Bitboard rankMask[64];
// Non-connected pawns mask[square]
Bitboard isolatedMask[64];
// passed pawns [side][square]
Bitboard passedMask[2][64];

constexpr int get_rank[64] =
{
    7, 7, 7, 7, 7, 7, 7, 7,
    6, 6, 6, 6, 6, 6, 6, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    4, 4, 4, 4, 4, 4, 4, 4,
    3, 3, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0
};

constexpr int double_pawn_penalty = -10;
constexpr int isolated_pawn_penalty = -10;
constexpr int passed_pawn_bonus[8] = { 0, 10, 30, 50, 75, 100, 150, 200 };

constexpr int semi_open_file_bonus = 10;
constexpr int open_file_bonus = 15;

constexpr int king_prot_bonus = 5;

void initPawnMasks(){
    for(int rank = RANK_8; rank <= RANK_1; rank++){
        for(int file = FILE_A; file <= FILE_H; file++){
            int square = rank*8 + file;

            // Initializing rank and file masks
            fileMask[square] = files[file];
            rankMask[square] = ranks[rank];

            // Initializing isolated pawn masks
            isolatedMask[square] = ((file == FILE_A) ? 0ULL:files[file - 1]) | ((file == FILE_H) ? 0ULL:files[file + 1]);

            // Initializing passed pawns mask
            passedMask[White][square] = ((file == FILE_A) ? 0ULL:files[file - 1]) | files[file] | ((file == FILE_H) ? 0ULL:files[file + 1]);
            passedMask[Black][square] = ((file == FILE_A) ? 0ULL:files[file - 1]) | files[file] | ((file == FILE_H) ? 0ULL:files[file + 1]);

            for(int a = RANK_1; a >= rank; a--){
                passedMask[White][square] &= ~ranks[a];
            }

            for(int a = RANK_8; a <= rank; a++){
                passedMask[Black][square] &= ~ranks[a];
            }
        }
    }
}

static inline int evaluate(BoardContainer boards){
    int score = 0;

    Bitboard bitboard;

    int square;

    for(int piece = P; piece < numPieces; piece++){
        bitboard = boards.board.pieceBoards[piece];

        while(bitboard){
            square = findLSB(bitboard);

            score += materialScore[piece];

            switch(piece){
                case P:{
                    score += pawn_score[square];

                    int doublePawnCount = countBits(boards.board.pieceBoards[P] & fileMask[square]);

                    if(doublePawnCount > 1){
                        score += doublePawnCount*double_pawn_penalty;
                    }

                    if((boards.board.pieceBoards[P] & isolatedMask[square]) == 0){
                        score += isolated_pawn_penalty;
                    }

                    if((boards.board.pieceBoards[p] & passedMask[White][square]) == 0){
                        score += passed_pawn_bonus[get_rank[square]];
                    }

                    break;
                }case N:{
                    score += knight_score[square];
                    break;
                }case B:{
                    score += bishop_score[square];

                    // Bishop mobility bonus
                    score += countBits(getBishopAttacks(square, boards.board.occupancies[Both]));

                    break;
                }case R:{
                    score += rook_score[square];

                    if((boards.board.pieceBoards[P] & fileMask[square]) == 0){
                        score += semi_open_file_bonus;
                    }

                    if(((boards.board.pieceBoards[P] | boards.board.pieceBoards[p]) & fileMask[square]) == 0){
                        score += open_file_bonus;
                    }

                    break;
                }case Q:{
                    score += countBits(getQueenAttacks(square, boards.board.occupancies[Both]));

                    break;
                }case K:{
                    score += king_score[square];

                    if((boards.board.pieceBoards[P] & fileMask[square]) == 0){
                        score -= semi_open_file_bonus;
                    }

                    if(((boards.board.pieceBoards[P] | boards.board.pieceBoards[p]) & fileMask[square]) == 0){
                        score -= open_file_bonus;
                    }

                    score += king_prot_bonus*countBits(king_attacks[square] & boards.board.occupancies[White]);

                    break;
                }case p:{
                    score -= pawn_score[mirror_score[square]];

                    int doublePawnCount = countBits(boards.board.pieceBoards[p] & fileMask[square]);

                    if(doublePawnCount > 1){
                        score -= doublePawnCount*double_pawn_penalty;
                    }

                    if((boards.board.pieceBoards[p] & isolatedMask[square]) == 0){
                        score -= isolated_pawn_penalty;
                    }

                    if((boards.board.pieceBoards[P] & passedMask[Black][square]) == 0){
                        score -= passed_pawn_bonus[get_rank[mirror_score[square]]];
                    }

                    break;
                }case n:{
                    score -= knight_score[mirror_score[square]];
                    break;
                }case b:{
                    score -= bishop_score[mirror_score[square]];

                    score -= countBits(getBishopAttacks(square, boards.board.occupancies[Both]));

                    break;
                }case r:{
                    score -= rook_score[mirror_score[square]];

                    if((boards.board.pieceBoards[p] & fileMask[square]) == 0){
                        score -= semi_open_file_bonus;
                    }

                    if(((boards.board.pieceBoards[p] | boards.board.pieceBoards[P]) & fileMask[square]) == 0){
                        score -= open_file_bonus;
                    }

                    break;
                }case q:{
                    score -= countBits(getQueenAttacks(square, boards.board.occupancies[Both]));

                    break;
                }case k:{
                    score -= king_score[mirror_score[square]];

                    if((boards.board.pieceBoards[p] & fileMask[square]) == 0){
                        score += semi_open_file_bonus;
                    }

                    if(((boards.board.pieceBoards[p] | boards.board.pieceBoards[P]) & fileMask[square]) == 0){
                        score += open_file_bonus;
                    }

                    score -= king_prot_bonus*countBits(king_attacks[square] & boards.board.occupancies[Black]);

                    break;
                }
            }

            clear(bitboard, square);
        }
    }

    return (boards.board.side == White) ? score:-score;
}

/*----------------------------------*/
/*         SEARCH VARIABLES         */
/*----------------------------------*/

// "infinity" for alpha beta bounds
#define infinity 50000
#define mate_value 49000
#define mate_score 48000

// Most Valuable Victim, Least Valuable Attacker lookup table [attacker][victim]
static int mvv_lva[12][12] = {
 	105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
	104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
	103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
	102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
	101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
	100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600,

	105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
	104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
	103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
	102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
	101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
	100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600
};

// max ply
constexpr int MAX_PLY = 64;

// killer moves [id][ply]
Move killer_moves[2][MAX_PLY];

// history moves [piece][square]
int history_moves[12][64] = {0};

// PV length [ply]
int pv_length[MAX_PLY];

// PV table [ply][ply]
Move pv_table[MAX_PLY][MAX_PLY];

// follow PV and score PV move
int follow_pv, score_pv;

// depth tracker from current node for search
int ply = 0;
unsigned long long nodes = 0;

/*----------------------------------*/
/*      TRANSPOSITION TABLES        */
/*----------------------------------*/

// hash table size
#define HASH_SIZE 800000

#define NOT_FOUND 100000

// hash flags for transposition table implementation
#define hashFlagExact 0 // PV node (score > alpha)
#define hashFlagAlpha 1 // fail low(score <= alpha)
#define hashFlagBeta 2  // fail high(score >= beta)

class transpositionTable{
    public:
    Bitboard hashKey = 0ULL;
    int depth = 0;
    int hashFlag = 0;   // flag the type of node
    int score = 0;

    transpositionTable(){
        hashKey = 0ULL;
        depth = 0;
        hashFlag = 0;
        score = 0;
    }
};

transpositionTable hashTable[HASH_SIZE];

void clearHashTable(){
    for(int index = 0; index < HASH_SIZE; index++){
        hashTable[index].hashKey = 0ULL;
        hashTable[index].depth = 0;
        hashTable[index].hashFlag = 0;
        hashTable[index].score = 0;
    }
}

static inline int readHashEntry(int alpha, int beta, int depth, Bitboard hashKey){
    transpositionTable *hashEntry = &hashTable[hashKey % HASH_SIZE];

    // if table entry matches current position
    if(hashEntry->hashKey == hashKey){
        if(hashEntry->depth >= depth){
            int score = hashEntry->score;

            if(score < -mate_score){
                score += ply;
            }

            if(score > mate_score){
                score -= ply;
            }

            // match PV node score
            if(hashEntry->hashFlag == hashFlagExact){
                return score;
            }

            // match fail-low score
            if((hashEntry->hashFlag == hashFlagAlpha) && (score <= alpha)){
                return alpha;
            }

            // match fail-high score
            if((hashEntry->hashFlag == hashFlagBeta) && (score >= beta)){
                return beta;
            }
        }
    }

    return NOT_FOUND;
}

static inline void storeHashEntry(int score, int depth, int hashFlag, Bitboard hashKey){
    transpositionTable *hashEntry = &hashTable[hashKey % HASH_SIZE];

    if(score < -mate_score){
        score -= ply;
    }

    if(score > mate_score){
        score += ply;
    }

    hashEntry->hashKey = hashKey;
    hashEntry->depth = depth;
    hashEntry->hashFlag = hashFlag;
    hashEntry->score = score;
}

/*----------------------------------*/
/*        MOVE SCORE/ORDERING       */
/*----------------------------------*/

// enable PV move scoring
static inline void enablePVScoring(MoveList move_list){
    // clear follow pv flag
    follow_pv = 0;

    // loop over the moves within move list
    for(int count = 0; count < move_list.count; count++){
        // match with current pv move
        if(pv_table[0][ply] == move_list.moves[count]){
            // set pv flags
            score_pv = 1;
            follow_pv = 1;
        }
    }
}

// Scores a move based off of mvv lva lookup table
static inline int scoreMove(Move move, BoardContainer boards){
    // pv move scoring
    if(score_pv){
        // check if move matches pv move
        if(pv_table[0][ply] == move){
            // clear score pv flag
            score_pv = 0;

            return 20000;
        }
    }

    if(move.flags & CAPTURE){ // Score captures
        int target = P;

        int startInd, endInd;
        if(boards.board.side == White){
            startInd = p;
            endInd = k;
        }else{
            startInd = P;
            endInd = K;
        }

        for(int piece = startInd; piece <= endInd; piece++){
            if(getSquare(boards.board.pieceBoards[piece], move.to)){
                target = piece;
                break;
            }
        }
        
        return mvv_lva[move.piece][target] + 10000;
    }else{ // Score quiet moves
        if(killer_moves[0][ply] == move){
            return 9000;
        }else if(killer_moves[1][ply] == move){
            return 8000;
        }else{
            return history_moves[move.piece][move.to];
        }
    }
    return 0;
}

// prints all move scores
void printMoveScores(MoveList move_list, BoardContainer boards){
    std::cout << "Move Scores\n\n";
    for(int ind = 0; ind < move_list.count; ind++){
        std::cout << "Move: ";
        move_list.moves[ind].print();
        std::cout << " Score: " << scoreMove(move_list.moves[ind], boards) << std::endl;
    }
}

// sort moves in descending order
static inline int sortMoves(MoveList &move_list, BoardContainer boards){
    int moveScores[move_list.count];
    
    for (int ind = 0; ind < move_list.count; ind++){
        moveScores[ind] = scoreMove(move_list.moves[ind], boards);
    }
    
    for (int ind = 0; ind < move_list.count; ind++)    {
        for (int nextInd = ind + 1; nextInd < move_list.count; nextInd++)
        {
            // compare current and next move scores
            if (moveScores[ind] < moveScores[nextInd])
            {
                // swap scores
                int temp_score = moveScores[ind];
                moveScores[ind] = moveScores[nextInd];
                moveScores[nextInd] = temp_score;
                
                // swap moves
                Move temp_move = move_list.moves[ind];
                move_list.moves[ind] = move_list.moves[nextInd];
                move_list.moves[nextInd] = temp_move;
            }
        }
    }

    return 0;
}

/*----------------------------------*/
/*              SEARCH              */
/*----------------------------------*/

static inline bool is_repeated(BoardContainer boards){
    for(int count = 0; count < boards.board.rep_ind; count++){
        if(boards.board.repetition_table[count] == boards.board.hashKey){
            return true;
        }
    }

    return false;
}

// quiescence search
static inline int quiescence(int alpha, int beta, BoardContainer boards){
    // Check gui input every 2047 nodes
    if((nodes & 2047) == 0){
        communicate();
    }
    
    nodes++;

    if(ply > MAX_PLY - 1){
        return evaluate(boards);
    }

    int eval = evaluate(boards);

    // fail hard beta cutoff, node fails high
    if(eval >= beta){
        return beta;
    }

    // fount a better move
    if(eval > alpha){
        // PV node
        alpha = eval;
    }

    MoveList move_list;

    boards.board.generateMoves(move_list);

    sortMoves(move_list, boards);

    for(int ind = 0; ind < move_list.count; ind++){
        boards.saveBoard();

        ply++;

        boards.board.repetition_table[boards.board.rep_ind] = boards.board.hashKey;
        boards.board.rep_ind++;

        if(boards.makeMove(move_list.moves[ind], captures) == 0){
            ply--;

            boards.board.rep_ind--;

            continue;
        }

        int score = -quiescence(-beta, -alpha, boards);

        ply--;

        boards.board.rep_ind--;

        boards.restoreBoard();

        // time is up
        if(stopped){
            return 0;
        }

        // fount a better move
        if(score > alpha){
            // PV node
            alpha = score;

            // fail hard beta cutoff, node fails high
            if(score >= beta){
                return beta;
            }
        }
    }

    return alpha;
}

const int full_depth_moves = 4;
const int reduction_limit = 3;

// negamax alpha beta search
static inline int negamax(int alpha, int beta, int depth, BoardContainer boards){    
    // static evaluation score
    int score;

    int hashFlag = hashFlagAlpha;

    // if repeated position
    if(ply && is_repeated(boards)){
        return 0;
    }

    // Checks if current node is a pv node
    bool isPV = (beta - alpha) > 1;
    
    // check if move has already been searched (is in transposition table)
    if(ply && (score = readHashEntry(alpha, beta, depth, boards.board.hashKey)) != NOT_FOUND && !isPV){
        return score;
    }

    // Check gui input every 2047 nodes
    if((nodes & 2047) == 0){
        communicate();
    }

    // init pv length
    pv_length[ply] = ply;

    // escape condition
    if(depth == 0){
        return quiescence(alpha, beta, boards);
    }

    // ply overflow handling
    if(ply > MAX_PLY - 1){
        return evaluate(boards);
    }

    nodes++;

    int in_check = boards.board.isAttacked((boards.board.side == White) ? findLSB(boards.board.pieceBoards[K]):findLSB(boards.board.pieceBoards[k]), boards.board.side^1);

    // increase depth if king in check
    if(in_check){
        depth++;
    }

    int legal_moves = 0;

    // null move pruning
    if(depth >= 3 && in_check == 0 && ply){
        boards.saveBoard();

        ply++;

        boards.board.repetition_table[boards.board.rep_ind] = boards.board.hashKey;
        boards.board.rep_ind++;

        // Give enemy an extra move
        boards.board.side ^= 1; 
        // Update hash key
        boards.board.hashKey ^= side_key;

        if(boards.board.enpassant != no_sq){
            boards.board.hashKey ^= enpassant_keys[boards.board.enpassant];
        }

        boards.board.enpassant = no_sq;

        // Find beta cutoffs within depth - 1 - R moves
        score = -negamax(-beta, -beta + 1, depth - 1 - 2, boards);

        ply--;

        boards.board.rep_ind--;

        boards.restoreBoard();

        // time is up
        if(stopped){
            return 0;
        }

        // beta cutoff
        if(score >= beta){
            return beta;
        }
    }

    MoveList move_list;

    boards.board.generateMoves(move_list);

    if(follow_pv){
        // enable pv move scoring
        enablePVScoring(move_list);
    }

    sortMoves(move_list, boards);

    // number of moves searched in a move list
    int moves_searched = 0;

    for(int ind = 0; ind < move_list.count; ind++){
        boards.saveBoard();

        ply++;

        boards.board.repetition_table[boards.board.rep_ind] = boards.board.hashKey;
        boards.board.rep_ind++;

        if(boards.makeMove(move_list.moves[ind], all) == 0){
            ply--;

            boards.board.rep_ind--;

            continue;
        }

        legal_moves++;
        // normal alpha beta algo
        if(moves_searched == 0){ // full depth search
            score = -negamax(-beta, -alpha, depth - 1, boards);
        }else{ // Late move reduction
            // Checks if lmr is possible
            if((moves_searched >= full_depth_moves) && (depth >= reduction_limit) && (in_check == 0) && ((move_list.moves[ind].flags & CAPTURE) == 0) && (move_list.moves[ind].promotedPiece == P)){
                score = -negamax(-alpha - 1, -alpha, depth - 2, boards);
            }else{
                score = alpha + 1;
            }

            // principal variation search
            if(score > alpha){
                score = -negamax(-alpha - 1, -alpha, depth - 1, boards);

                if((score > alpha) && (score < beta)){
                    score = -negamax(-beta, -alpha, depth - 1, boards);
                }
            }
        }

        ply--;

        boards.board.rep_ind--;

        boards.restoreBoard();

        // time is up
        if(stopped){
            return 0;
        }

        moves_searched++;

        // found a better move
        if(score > alpha){
            hashFlag = hashFlagExact;

            if((move_list.moves[ind].flags & CAPTURE) == 0){
                // store history moves
                history_moves[move_list.moves[ind].piece][move_list.moves[ind].to] += depth;
            }

            // PV node
            alpha = score;

            // PV move
            pv_table[ply][ply] = move_list.moves[ind];

            for(int next_ply = ply + 1; next_ply < pv_length[ply + 1]; next_ply++){
                // copy move from deeper ply into current ply's line
                pv_table[ply][next_ply] = pv_table[ply + 1][next_ply];
            }

            pv_length[ply] = pv_length[ply + 1];

            // fail hard beta cutoff, node fails high
            if(score >= beta){
                storeHashEntry(beta, depth, hashFlagBeta, boards.board.hashKey);

                if((move_list.moves[ind].flags & CAPTURE) == 0){
                    // store killer moves
                    killer_moves[1][ply] = killer_moves[0][ply];
                    killer_moves[0][ply] = move_list.moves[ind];
                }

                return beta;
            }
        }
    }

    if(legal_moves == 0){
        if(in_check){
            return -mate_value + ply;
        }else{
            return 0;
        }
    }

    storeHashEntry(alpha, depth, hashFlag, boards.board.hashKey);

    // node fails low
    return alpha;
}

void searchPosition(int depth, BoardContainer boards){
    int score;

    // reset nodes, and follow PV flags
    nodes = 0;
    follow_pv = 0;
    score_pv = 0;
    stopped = false;

    // clear pv, killer, and history
    memset(killer_moves, 0, sizeof(killer_moves));
    memset(history_moves, 0, sizeof(history_moves));
    memset(pv_table, 0, sizeof(pv_table));
    memset(pv_length, 0, sizeof(pv_length));
    
    int alpha = -infinity;
    int beta = infinity;

    // iterative deepening
    for(int current_depth = 1; current_depth <= depth; current_depth++){        
        // if time is up
        if(stopped){
            break;
        }
        
        // set follow_pv flag
        follow_pv = 1;

        score = negamax(alpha, beta, current_depth, boards);

        if(score <= alpha || score >= beta){
            alpha = -infinity;
            beta = infinity;
            continue;
        }

        alpha = score - 50;
        beta = score + 50;

        if (score > -mate_value && score < -mate_score){
            std::cout << "info score mate " << (-(score + mate_value) / 2 - 1) << " depth " << current_depth << " nodes " << nodes << " pv ";
        }else if (score > mate_score && score < mate_value){
            std::cout << "info score mate " << ((mate_value - score) / 2 + 1) << " depth " << current_depth << " nodes " << nodes << " pv ";
        }
        else{
            std::cout << "info score cp " << score << " depth " << current_depth << " nodes " << nodes << " pv ";
        }

        for(int count = 0; count < pv_length[0]; count++){
            pv_table[0][count].print();
            std::cout << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "bestmove ";
    pv_table[0][0].print();
    std::cout << std::endl;
}

/*----------------------------------*/
/*                UCI               */
/*----------------------------------*/

BoardContainer boardState = BoardContainer();

// Parses a inputted move string given by UCI protocol or user input
Move parseMove(std::string moveString){
    MoveList move_list;

    boardState.board.generateMoves(move_list);

    int from = (moveString[0] - 'a') + (8 - (moveString[1] - '0'))*8;
    int to = (moveString[2] - 'a') + (8 - (moveString[3] - '0'))*8;

    for(int ind = 0; ind < move_list.count; ind++){
        Move move = move_list.moves[ind];

        if(from == move.from && to == move.to){
            int promotedPiece = move.promotedPiece;
        
            if(promotedPiece){
                if((promotedPiece == Q || promotedPiece == q) && moveString[4] == 'q'){
                    return move;
                }else if((promotedPiece == R || promotedPiece == r) && moveString[4] == 'r'){
                    return move;
                }else if((promotedPiece == B || promotedPiece == b) && moveString[4] == 'b'){
                    return move;
                }else if((promotedPiece == N || promotedPiece == n) && moveString[4] == 'n'){
                    return move;
                }

                // illegal promotion
                continue;
            }

            return move;
        }
    }

    return Move();
}

// Parses a given position string given by UCI protocol or user input
void parsePosition(std::string command){
    int ind = 9;
    if(strncmp(&command[ind], "startpos", 8) == 0){
        boardState.board = Chessboard(start_position);
    }else if(strncmp(&command[ind], "fen", 3) == 0){
        ind += 4;
        boardState.board = Chessboard(&command[ind]);
    }else{
        boardState.board = Chessboard(start_position);
    }

    ind = command.find("moves");
    if(ind != -1){
        ind += 6;
        
        while(&command[ind]){
            Move move = parseMove(&command[ind]);

            // if illegal
            if(move.from == no_sq){
                break;
            }

            boardState.board.repetition_table[boardState.board.rep_ind] = boardState.board.hashKey;
            boardState.board.rep_ind++;

            boardState.makeMove(move, all);

            while(&command[ind] && command[ind] != ' '){
                ind++;
            }
            ind++;
        }
    }

    boardState.board.printChessboard();
}

// BlueFeverSoftware implementation
// parse UCI go command
void parseGo(std::string command)
{
    // init parameters
    int depth = -1;

    // init argument
    char *argument = NULL;

    // infinite search
    if ((argument = strstr(&command[0],"infinite"))) {}

    // match UCI "binc" command
    if ((argument = strstr(&command[0],"binc")) && boardState.board.side == Black){
        // parse black time increment
        inc = atoi(argument + 5);
    }
    // match UCI "winc" command
    if ((argument = strstr(&command[0],"winc")) && boardState.board.side == White){
        // parse white time increment
        inc = atoi(argument + 5);
    }

    // match UCI "wtime" command
    if ((argument = strstr(&command[0],"wtime")) && boardState.board.side == White){
        // parse white time limit
        timeTracker = atoi(argument + 6);
    }

    // match UCI "btime" command
    if ((argument = strstr(&command[0],"btime")) && boardState.board.side == Black){
        // parse black time limit
        timeTracker = atoi(argument + 6);
    }

    // match UCI "movestogo" command
    if ((argument = strstr(&command[0],"movestogo"))){
        // parse number of moves to go
        movestogo = atoi(argument + 10);
    }

    // match UCI "movetime" command
    if ((argument = strstr(&command[0],"movetime"))){
        // parse amount of time allowed to spend to make a move
        movetime = atoi(argument + 9);
    }

    // match UCI "depth" command
    if ((argument = strstr(&command[0],"depth"))){
        // parse search depth
        depth = atoi(argument + 6);
    }

    // if move time is not available
    if(movetime != -1)
    {
        // set time equal to move time
        timeTracker = movetime;

        // set moves to go to 1
        movestogo = 1;
    }

    // init start time
    starttime = get_time_ms();

    // init search depth
    depth = depth;

    // if time control is available
    if(timeTracker != -1)
    {
        // flag we're playing with time control
        timeset = 1;

        // set up timing
        timeTracker /= movestogo;
        if(timeTracker > 1500){
            timeTracker -= 50;
        }
        stoptime = starttime + timeTracker + inc;
    }

    // if depth is not available
    if(depth == -1){
        // set depth to 64 plies (takes ages to complete...)
        depth = 64;
    }

    // print debug info
    printf("time:%d start:%d stop:%d depth:%d timeset:%d\n",
    timeTracker, starttime, stoptime, depth, timeset);

    // search position
    searchPosition(depth, boardState);
}

// main UCI program
void uciLoop(){
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    std::string input;

    std::cout << "id name BitboardChessEngine\n";
    std::cout << "id author CW\n";
    std::cout << "uciok\n";

    while(1){
        fflush(stdout);
        input = "\n\0";
        getline(std::cin, input);
        std::cout << "Parsing input: " << input << std::endl;

        if(input[0] == '\n'){
            continue;
        }
        
        // UCI isready command
        if(strncmp(&input[0], "isready", 7) == 0){
            std::cout << "readyok\n";
            continue;
        }
        
        // UCI position command
        if(strncmp(&input[0], "position", 8) == 0){
            parsePosition(input);

            clearHashTable();
            continue;
        }

        // UCI new game command
        if(strncmp(&input[0], "ucinewgame", 10) == 0){
            parsePosition("position startpos");
            clearHashTable();
            continue;
        }

        // UCI go command
        if(strncmp(&input[0], "go", 2) == 0){
            parseGo(input);
            continue;
        }
        
        // UCI quit command
        if(strncmp(&input[0], "quit", 4) == 0){
            break;
        }

        // UCI uci command
        if(strncmp(&input[0], "uci", 3) == 0){
            std::cout << "id name BitboardChessEngine\n";
            std::cout << "id author CW\n";
            std::cout << "uciok\n";
            continue;
        }
    }
}

/*----------------------------------*/
/*          INITIALIZATION          */
/*----------------------------------*/

void init(){
    initLeaperAttacks();
    initSliderAttacks();
    init_random_keys();
    initPawnMasks();
    clearHashTable();
}

/*----------------------------------*/
/*               MAIN               */
/*----------------------------------*/

int main(){
    init();

    int debug = 0;

    if(debug){
        BoardContainer boards = BoardContainer("6k1/ppppprbp/8/8/8/8/PPPPPRBP/6K1 w - - ");
        boards.board.printChessboard();
        std::cout << "Score: " << evaluate(boards);
    }else{
        uciLoop();
    }

    return 0;
}