#ifndef EVALUATION_H_
#define EVALUATION_H_
#include <stdio.h>
#include <math.h>
#include "board.h"
#include "legal_moves.h"

#define KNIGHT_MOB_VAL 0.875
#define BISHOP_MOB_VAL 1.149
#define ROOK_MOB_VAL 1.17
#define QUEEN_MOB_VAL 1.093

float power(float a,int n){
    float result=1;
    for(int i=1;i<=n;i++){
        result *=a;
    }
    return result;
}

float eval_random(game_state* s)
{
    float x = (float)rand()/(float)(RAND_MAX/1000);
    return x;
}

const float Piece_Value[14] = {100.0,60.0,60.0,900.0,500.0,10.0,0.0,0.0,-100.0,-60.0,-60.0,-900.0,-500.0,-10.0};
float eval_space_coverage(game_state *s){
    float space_covered=0.0;
    float space;
    for(int i=0;i<64;i++){
        int piece = s->squares[i];
        if(piece==W_PAWN ||piece==B_PAWN){//white  pawn
            space =(float)board_index_to_coord_y(i);
            space_covered = (piece==W_PAWN)? space_covered+space+1.0:space_covered-8.0+space;
        }
    }
    return space_covered;
}

//only for major pieces like bishop, queen, rook, knight
float eval_major_pieces_mobility(game_state *s){
    float mobility=0.0;
    uint64_t possible_moves;
    int count;
    for(int i=0;i<64;i++){
        int piece =s->squares[i];
        if(!is_blank(piece)){
            switch (piece){
                case B_KNIGHT:
                case W_KNIGHT:
                    possible_moves =legal_move_knight(s,i);
                    count = popcount(possible_moves);
                    if(get_player(piece)==WHITE){
                        mobility += KNIGHT_MOB_VAL*count; //mobility for knight is linear function of places available
                    }
                    else{
                        mobility -= KNIGHT_MOB_VAL*count;
                    }
                    break;
                case B_BISHOP:
                case W_BISHOP:
                    possible_moves =legal_move_bishop(s,i);
                    count = popcount(possible_moves);
                    if(get_player(piece)==WHITE){
                        mobility += power(BISHOP_MOB_VAL,count);//mobility for bishop is exp func of places
                    }
                    else{
                        mobility -= power(BISHOP_MOB_VAL,count);
                    }
                    break;
                case W_ROOK:
                case B_ROOK:
                    possible_moves =legal_move_bishop(s,i);
                    count = popcount(possible_moves);
                    if(get_player(piece)==WHITE){
                        mobility += power(ROOK_MOB_VAL,count);//mobility for rook too is exp func of places
                    }
                    else{
                        mobility -= power(ROOK_MOB_VAL,count);
                    }
                    break;
                case W_QUEEN:
                case B_QUEEN:
                    possible_moves =legal_move_bishop(s,i);
                    count = popcount(possible_moves);
                    if(get_player(piece)==WHITE){
                        mobility += power(QUEEN_MOB_VAL,count);//mobility of queen is also kept as exp. function
                    }
                    else{
                        mobility -= power(QUEEN_MOB_VAL,count);
                    }
                    break;
            }
        }
    }
    return mobility;
}

float eval_material(game_state *s)
{
    float evaluation = 0.0;
    for (int i = 0; i < 64; i++)
    {
        int piece =s->squares[i];
        if(piece!=BLANK){
            //material addition
            evaluation +=Piece_Value[piece];
        }
    }
    return evaluation;
}
//main evaluation function
float eval_comprehensive(game_state *s){
    float evaluation=0.0;
    float material=eval_material(s);
    float mobility=eval_major_pieces_mobility(s);
    float space_covered=eval_space_coverage(s);
    evaluation = 0.75*material+0.05*space_covered+0.2*mobility;
    return evaluation;
}

#endif