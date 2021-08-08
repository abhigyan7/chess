#ifndef MOVES_H_
#define MOVES_H_
#include <stdio.h>
#include "board.h"
/*checks if the move is castling or not*/
int is_castling(game_state s,int from,int to){
    if(s.turn){
        if((s.squares[from]==B_KING) && (s.squares[to]==B_ROOK)){
            return 1;
        }
    }
    else{
        if((s.squares[from]==W_KING) && (s.squares[to]==W_ROOK)){
            return 1;
        }
    }
    return 0;
}
/*swaps two pieces in case of castling and moving a piece to a free space*/
void swap(int *a,int *b){
    int temp =*a;
    *a=*b;
    *b=temp;
    
}

/*
The following function checks if a given state in starting state is empty
NOTE: We are supposed to check this in current state 
    but for now lets take starting_state
*/
int is_empty(game_state *s,int index){
    if((*s).squares[index]==BLANK){
        return 1;
    }
    return 0;
}
/*converts the string to board's index e.g c3 to 42
if invalid string is given it gives -1*/
int str_to_boardindex(char *square){
    int index =-1;
    if(*(square+2)=='\0'){
        index=0;
        //for alphabetical form
        int ascii = *square;
        if(ascii>=97 && ascii<=104){
            index += ascii-97;
        }
        else{
            index =-1;
        }
        //for numerical part
        ascii = *(square+1);
        if(ascii>=49 && ascii<=56){
            ascii -=48;
            index += (8-ascii)*8;
        }
        else{
            index =-1;
        }
    }
    return index;
}
/*this function converts string of positions to index and then works on piece movement
1.moving to a blank space
2.capturing a piece
3.castling
*/
void move(char *from,char *to){
    int from_index = str_to_boardindex(from);
    int to_index = str_to_boardindex(to);
    
    if(from_index!=-1 && to_index!=-1 && (starting_state.squares[from_index]!= BLANK)){
        int castling = is_castling(starting_state,from_index,to_index);
        //moving a piece to blank
        if(starting_state.squares[to_index]==BLANK){
            printf("move %d to %d\n",from_index,to_index);
            swap(&starting_state.squares[from_index],&starting_state.squares[to_index]);
            printf("\nmove %d to %d\n",starting_state.squares[from_index],starting_state.squares[to_index]);
            print_board_state(&starting_state);
            
        }
        //captures
        else if(starting_state.squares[to_index]!=BLANK && (!castling)){
            starting_state.squares[to_index]= BLANK;
            swap(&starting_state.squares[from_index],&starting_state.squares[to_index]);
            printf("\ncapture %d x %d\n",starting_state.squares[from_index],starting_state.squares[to_index]);
            print_board_state(&starting_state);
        }
        //if not above cases castling is true
        else{

        }
    }
    
}

#endif  
