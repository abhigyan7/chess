#ifndef AI_H_
#define AI_H_

#include <sys/time.h>

#include "board.h"
#include "legal_moves.h"
#include "evaluation.h"

unsigned int n_states_explored = 0;

float eval_minimax(game_state*s, int depth)
{
    n_states_explored ++;
    if (depth == 0)
        return eval_comprehensive(s);

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

float minimax_eval_alpha_beta_pruning(game_state*s, int depth)
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
                    float val_of_new_state = eval_minimax(&new_state, depth-1);
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

int choose_best_move(game_state* s, int* from, int* to, double* time_taken_for_search_milliseconds)
{

    struct timeval t1, t2;
    n_states_explored = 0;

    float best_val;
    int best_to;
    int best_from;

    int ret = -1;

    if (s->turn == WHITE)
    {
        best_val =  -1000000;
    } else {
        best_val = 1000000;
    }

    // starting search
    gettimeofday(&t1, NULL);

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
                    float val_of_new_state = eval_minimax(&new_state,3);
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
    // ending seach
    gettimeofday(&t2, NULL);

    int secs = t2.tv_sec - t1.tv_sec;
    int usecs = t2.tv_usec - t1.tv_usec;

    if(usecs < 0)
    {
        --secs;
        usecs += 1000000;
    }

    *time_taken_for_search_milliseconds = (secs * 1000 + usecs / 1000.0 + 0.5);
    fprintf(stderr, "Explored %u states in %F milliseconds.\n", n_states_explored, *time_taken_for_search_milliseconds);
    return ret;
}

#endif // AI_H_
