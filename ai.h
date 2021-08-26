#ifndef AI_H_
#define AI_H_


#include "board.h"
#include "legal_moves.h"

#define KNIGHT_MOB_VAL 0.875
#define BISHOP_MOB_VAL 1.149
#define ROOK_MOB_VAL 1.17
#define QUEEN_MOB_VAL 1.093

#define VALUE_DECAY_FACTOR 0.98

float power(float a,int n){
    float result=1;
    for(int i=1;i<=n;i++){
        result *=a;
    }
    return result;
}

//evaluation function simply returns a float value that corresponds to game_state
float(*eval_function)(game_state* s, int a);

float eval_random(game_state* s, int a)
{
    float x = (float)rand()/(float)(RAND_MAX/1000);
    return x;
}

const float Piece_Value[14] = {100.0,60.0,60.0,900.0,500.0,10.0,0.0,0.0,-100.0,-60.0,-60.0,-900.0,-500.0,-10.0};
float eval_space_coverage(game_state *s, int a){
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
float eval_major_pieces_mobility(game_state *s, int a){
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

float eval_material(game_state *s, int a)
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
float eval_comprehensive(game_state *s, int a){
    float evaluation=0.0;
    float material=0.0;
    float mobility=eval_major_pieces_mobility(s, 0);
    float space_covered=eval_space_coverage(s, 0);
    for(int i =0;i<64;i++){
        int piece =s->squares[i];
        if(piece!=BLANK){
            //material addition
            material +=Piece_Value[piece];
            }

    }
    evaluation = 0.8*material+0.05*space_covered+1.5*mobility;
    return evaluation;
}

float minimax_eval(game_state*s, int depth)
{
    if (depth == 0)
        return eval_comprehensive(s, 0);

    float best_val;
    if (s->turn == WHITE)
        best_val = -10000000;
    else {
        best_val = 10000000;
    }
    int any_moves_found = 0;
    for (int i = 0; i < 64; i++)
    {
        if (get_player(s->squares[i]) == s->turn)
        {
            uint64_t all_moves_from_i = get_legal_moves(s, i);
            any_moves_found = any_moves_found || all_moves_from_i;
            for (int j = 0; j < 64; j++)
            {
                if (get_nth_bit(all_moves_from_i, j))
                {
                    game_state new_state = make_move(s, i, j);
                    float val_of_new_state = VALUE_DECAY_FACTOR * minimax_eval(&new_state, depth-1);
                    if (s->turn == WHITE)
                    {
                        if (best_val <= val_of_new_state)
                        {
                            best_val = val_of_new_state;
                        }
                    } else {
                        if (best_val >= val_of_new_state)
                        {
                            best_val = val_of_new_state;
                        }
                    }
                }
            }
        }
    }
    if (any_moves_found == 0)
        return s->turn ? 1000 : -1000;
    return best_val;
}

int choose_best_move(game_state* s, int* from, int* to)
{
    float best_val;
    int best_to;
    int best_from;

    int ret = -1;

    if (s->turn == WHITE)
    {
        eval_function = minimax_eval;
        best_val =  -1000000;
    } else {
        eval_function = minimax_eval;
        best_val = 1000000;
    }

    for (int i = 0; i < 64; i++)
    {
        if (get_player(s->squares[i]) == s->turn)
        {
            uint64_t all_moves_from_i = get_legal_moves(s, i);

            for (int j = 0; j < 64; j++)
            {
                if (get_nth_bit(all_moves_from_i, j))
                {
                    game_state new_state = make_move(s, i, j);
                    float val_of_new_state = eval_function(&new_state, 3);
                    //fprintf(stderr,"Value for moving %s from %d to %d = %.2f.\n", chars_for_pieces[s->squares[i]], i, j, val_of_new_state);
                    if (s->turn == WHITE)
                    {
                        if (best_val <= val_of_new_state)
                        {
                            best_val = val_of_new_state;
                            best_from = i;
                            best_to = j;
                            ret = 1;
                        }
                    } else {
                        if (best_val >= val_of_new_state)
                        {
                            best_val = val_of_new_state;
                            best_from = i;
                            best_to = j;
                            ret = 1;
                        }
                    }
                }
            }
        }
    }
    *from = best_from;
    *to = best_to;
    fprintf(stderr,"Moving %s from %d to %d with a value of %.2f.\n", chars_for_pieces[s->squares[best_from]], best_from, best_to, best_val);
    return ret;
}

#endif // AI_H_
