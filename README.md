# Chess-Engine

## Board Representation (Bitboards)
The chessboard is represented by a 64-bit unsigned integer (bitboard) where each bit represents a square on the chessboard. A set bit in the bitboard corresponds to a square being occupied by a piece. Multiple bitboards are required to represent a chessboard, typically there is one bitboard per piece for each colour. Bitboards are useful as multiple bit manipulation techniques/algorithms can be used to optimize the code.

## Move Generation