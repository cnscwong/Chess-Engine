# Chess-Engine

## Summary
C++ Chess Engine that analyzes a chess position and determines the best possible moves based off of evaluation of the current position, as well as the analysis of different positions that can occur N moves ahead. In chess programming, chess engines can evaluate billions of positions depending on the depth of moves calculated, which requires code to be as efficient as possible.

## Board Representation (Bitboards)
The chessboard is represented by a 64-bit unsigned integer (bitboard) where each bit represents a square on the chessboard. A set bit in the bitboard corresponds to a square being occupied by a piece. Multiple bitboards are required to represent a chessboard, one bitboard per piece for each colour, and 3 bitboards representing if a square is occupied by a white piece, black piece, or either. Bitboards are useful as multiple bit manipulation techniques/algorithms can be used to optimize the code.

## Move Lookup Tables
In order to generate legal moves, the engine needs to know what moves for each piece are actually possible. To avoid recalculating the moves of a piece in the same position multiple times, precalculated move/attack lookup tables are generated for each piece and square beforehand so possible moves takes O(1) time.

## Move Generation