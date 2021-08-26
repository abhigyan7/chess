#ifndef AI_H_
#define AI_H_
#include "board.h"
#include "legal_moves.h"
#include "evaluation.h"

float minimax_eval(game_state*s, int depth)
{
    if (depth == 0)
        return eval_comprehensive(s, 0);

    float best_val;
    if (s->turn == WHITE)
        best_val = -1000000;
    else {
        best_val = 1000000;
    }
    int any_moves_found;
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
                    float val_of_new_state = minimax_eval(&new_state, depth-1);
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
    //if (any_moves_found == 0)
    //    return s->turn ? -10000000 : 10000000;
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
