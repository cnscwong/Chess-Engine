# Chess-Engine

## Board Representation (Bitboards)
The chessboard is represented by a 64-bit unsigned integer (bitboard) where each bit corresponds to a square on the board. A set bit corresponds to the square being occupied. Multiple bitboards are required to represent a chessboard, typically there is one bitboard per piece for each colour. Bitboards are useful as multiple bit manipulation techniques/algorithms can be used to optimize the code.

## Move Generation