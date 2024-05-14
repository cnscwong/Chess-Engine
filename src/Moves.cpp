#include "Moves.h"

bool valid_square(int s){
    return s >= H1 && s <= A8;
}

void init_knight(){
    int temp;
    for(int s = H1; s < numSquares; s++){
        for(int move: knight_moves){
            temp = s + move;
            if(valid_square(temp)){
                knightLookup[s] |= 1 << temp;
            }
        }
    }
}