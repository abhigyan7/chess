#ifndef MOVES_H_
#define MOVES_H_
#include <stdio.h>
#include "board.h"
vector king_vector ={-2,2};
vector rook_vector ={3,-2};
//to check between the from and to whether the places are blank or not #for castling
int check_blank(game_state *s,int from,int to){
    if(from>to){
        for(int i=to+1;i<from;i++){
            if((*s).squares[i]!=BLANK){
                printf("yes");
                return 0;
                break;
            }
        }
    }
    else{
        for(int i=from+1;i<to;i++){
            if((*s).squares[i]!=BLANK){
                return 0;
                break;
            }
        }
    }
    return 1;
}





/*checks if the move is castling or not*/
int is_castling(game_state *s,int from,int to){
    if((*s).turn){
        if(((*s).squares[from]==B_KING) && ((*s).squares[to]==B_ROOK)){
            return 1;
        }
    }
    else{
        if(((*s).squares[from]==W_KING) && ((*s).squares[to]==W_ROOK)){
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
            ascii -= 48; //8
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
void move_from_strings(game_state *s,char from[],char to[]){
    int from_index = str_to_boardindex(from);
    int to_index = str_to_boardindex(to);
    
    if(from_index!=-1 && to_index!=-1 && ((*s).squares[from_index]!= BLANK)){
        int castling = is_castling(s,from_index,to_index);
        //moving a piece to blank
        if((*s).squares[to_index]==BLANK){
            printf("move %d to %d\n",from_index,to_index);
            swap(&(*s).squares[from_index],&(*s).squares[to_index]);
            printf("\nmove %d to %d\n",(*s).squares[from_index],(*s).squares[to_index]);
            print_board_state(s);
            (*s).turn = !(*s).turn;
            
        }
        //captures
        else if((*s).squares[to_index]!=BLANK && (!castling)){
            (*s).squares[to_index]= BLANK;
            swap(&(*s).squares[from_index],&(*s).squares[to_index]);
            printf("\ncapture %d x %d\n",(*s).squares[from_index],(*s).squares[to_index]);
            print_board_state(&starting_state);
            (*s).turn = !(*s).turn;
        }
        //if not above cases castling is true
        else{
            if(castling){//still we make sure it's castling
                if((!(*s).kings_movement[(*s).turn]) && (check_blank(s,from_index,to_index))){
                    int king_displace ;
                    int rook_displace;
                    int rook_index ;
                    switch(to_index){
                        case 0:
                        case 56:
                            rook_index =0;
                            break;
                        case 7:
                        case 63 :
                            rook_index =1;
                            break;
                    }
                    if(!(*s).rooks_movement[(*s).turn][rook_index]){
                        printf("\n before castling \n");
                        print_board_state(s);
                        king_displace = from_index + king_vector[rook_index];
                        rook_displace = to_index + rook_vector[rook_index];
                        printf("\ncastling \n");
                        swap(&(*s).squares[from_index],&(*s).squares[king_displace]);
                        swap(&(*s).squares[to_index],&(*s).squares[rook_displace]);
                        print_board_state(s);
                        (*s).turn = !(*s).turn;
                    }
                }

            }
        }
        
        printf("Turn: %d.\n",(*s).turn );
    }
    
}

#endif  
