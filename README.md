# Chess-Engine

## Summary
C++ Chess Engine that analyzes a chess position and determines the best possible moves based off of evaluation of the current position, as well as the analysis of different positions that can occur N moves ahead. In chess programming, chess engines can evaluate billions of positions depending on the depth of moves calculated, which requires code to be as efficient as possible.

## Board Representation (Bitboards)
The chessboard is represented by a 64-bit unsigned integer (bitboard) where each bit represents a square on the chessboard. A set bit in the bitboard corresponds to a square being occupied by a piece. Multiple bitboards are required to represent a chessboard, one bitboard per piece for each colour, and 3 bitboards representing if a square is occupied by a white piece, black piece, or either. Bitboards are useful as multiple bit manipulation techniques/algorithms can be used to optimize the code.

## Move Lookup Tables
In order to generate legal moves, the engine needs to know what moves for each piece are actually possible. To avoid recalculating the moves of a piece in the same position multiple times, precalculated move/attack lookup tables are generated for each piece and square beforehand so possible moves takes O(1) time.

## Move Generation
Takes the current board position and who is making the next move(white or black) to generate a list of legal moves. Lookup tables were used to generate the moves for pawns, knights, and the king. Magic bitboards which is an algorithm using random numbers and hashing was used for bishops, rooks, and queens as lookup tables are inefficient for these pieces since they can be blocked by enemy and friendly pieces.

## Evaluation
Board evaluation is the process of scoring a given board position based off of pieces, their positions, and other metrics such as king safety or piece mobility. This score reflects which side is winning in the chess game.

## Move Search
Searching for the best move for a chess position requires generating a "move tree", all possible combinations of legal moves that can occur in n turns(n is dependent on the strength and efficiency of the chess engine). This move tree is searched to find the branch that results in the highest score for the current player while also maximizing the enemy moves scores. This is similar to thinking about what the best move for your opponent would be after you play your move, otherwise, the chess engine would find the best branch on the tree where your opponent makes the worst moves possible. Searching through a move tree is very inefficient, especially as n increases, so pruning algorithms to stop searching through certain branches are used to optimize the search process.

## Acknowledgements
https://www.chessprogramming.org/Main_Page
https://web.archive.org/web/20071026090003/http://www.brucemo.com/compchess/programming/index.htm
https://www.youtube.com/@chessprogramming591/featured
https://www.youtube.com/@BlueFeverSoft
https://www.codeproject.com/Articles/5313417/Worlds-Fastest-Bitboard-Chess-Movegenerator
