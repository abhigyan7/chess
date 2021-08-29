#ifndef AI_H_
#define AI_H_

#include <sys/time.h>

#include "board.h"
#include "legal_moves.h"
#include "evaluation.h"
#include "stdlib.h"

#define VALUE_DECAY_FACTOR 0.9

int SEARCH_DEPTH = 4;

unsigned int n_states_explored = 0;

typedef struct {
    Move move;
    float value;
} MoveWithValue;

int compare_two_moves(const void* a, const void* b)
{
    const MoveWithValue* mva = (const MoveWithValue*) a;
    const MoveWithValue* mvb = (const MoveWithValue*) b;

    return (mva->value > mvb->value) - (mva->value < mvb->value);
}

void create_move_value_array_from_move_array(game_state *s, const Move* moves, MoveWithValue* mwv, int n)
{
    for (int i = 0; i < n; i++)
    {
        mwv[i].move = moves[i];
        game_state new = make_move_2(s, moves[i]);
        mwv[i].value = eval_comprehensive(&new) * ((s->turn == WHITE) ? -1.0 : 1.0);
    }
}

void create_move_array_from_move_value_array(const MoveWithValue* mwvs, Move* moves, int n)
{
    for (int i = 0; i < n; i++)
    {
        // printf("Move %hu, Value %f\n", mwvs[i].move, mwvs[i].value);
        moves[i] = mwvs[i].move;
    }
}

void sort_moves_by_static_eval(game_state* s, Move* moves, int n)
{
    MoveWithValue mwvs[n];
    create_move_value_array_from_move_array(s, moves, mwvs, n);
    qsort(mwvs, n, sizeof(MoveWithValue), &compare_two_moves);
    create_move_array_from_move_value_array(mwvs, moves, n);
}

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
                    float val_of_new_state = VALUE_DECAY_FACTOR * eval_minimax(&new_state, depth-1);
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

float max(float a, float b)
{
    return (a > b) ? a : b;
}

float min(float a, float b)
{
    return (a < b) ? a : b;
}

float minimax_eval_alpha_beta_pruning(game_state*s, int depth, float alpha, float beta)
{
    n_states_explored ++;
    if (depth == 0)
        return eval_comprehensive(s);

    float best_val;
    if (s->turn == WHITE)
        best_val = -1000000;
    else {
        best_val =  1000000;
    }

    int any_moves_found = 0;
    for (int i = 0; i < 64; i++)
    {
        if (!(get_player(s->squares[i]) == s->turn))
            continue;
        uint64_t all_moves_from_i = get_legal_moves(s, i);
        any_moves_found = any_moves_found || all_moves_from_i;
        for (int j = 0; j < 64; j++)
        {
            if (!get_nth_bit(all_moves_from_i, j))
                continue;
            game_state new_state = make_move(s, i, j);
            float val_of_new_state = minimax_eval_alpha_beta_pruning(&new_state, depth-1, alpha, beta);
            if (s->turn == WHITE)
            {
                best_val = max(best_val, val_of_new_state);
                if (val_of_new_state >= beta)
                {
                    printf("Beta cutoff");
                    goto ABORT;
                }
                alpha = max(alpha, val_of_new_state);
            } else {
                best_val = min(best_val, val_of_new_state);
                if (val_of_new_state <= alpha)
                {
                    printf("Alpha cutoff");
                    goto ABORT;
                }
                beta = min(beta, val_of_new_state);
            }
        }
    }
    ABORT:
    if (any_moves_found == 0)
        return s->turn ? 1000 : -1000;
    return best_val;
}

float minimax_eval_alpha_beta_pruning_2(game_state*s, int depth, float alpha, float beta)
{
    n_states_explored ++;
    if (depth == 0)
        return eval_comprehensive(s);

    float best_val;
    if (s->turn == WHITE)
        best_val = -1000000;
    else {
        best_val =  1000000;
    }

    Move moves[256];
    int n_moves = get_legal_moves_as_move_array(s, moves);
    if (n_moves == 0)
    {
        if (is_king_in_check(s, find_piece(s, (s->turn==WHITE)? W_KING : B_KING)))
            return s->turn ? 1000 : -1000;
        else {
            return s->turn ? -500 : 500;
        }
    }
    if (depth > 1)
        sort_moves_by_static_eval(s, moves, n_moves);
    for (int i = 0; i < n_moves; i++)
    {
        game_state new_state = make_move_2(s, moves[i]);
        float val_of_new_state = VALUE_DECAY_FACTOR * minimax_eval_alpha_beta_pruning_2(&new_state, depth-1, alpha, beta);
        if (s->turn == WHITE)
        {
            best_val = max(best_val, val_of_new_state);
            alpha = max(alpha, val_of_new_state);
            if (val_of_new_state > beta)
            {
                break;
            }
        } else {
            best_val = min(best_val, val_of_new_state);
            beta = min(beta, val_of_new_state);
            if (val_of_new_state < alpha)
            {
                break;
            }
        }
    }
    return best_val;
}

int choose_best_move_2(game_state* s, Move* move, double* time_taken_for_search_milliseconds)
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

    Move moves[256];
    Move best_move;
    int n_moves = get_legal_moves_as_move_array(s, moves);
    for (int i = 0; i < n_moves; i++)
    {
        int from = get_from_bits(moves[i]);
        int to = get_to_bits(moves[i]);

        printf("Move from %d to %d\n", from, to);

        game_state new_state = make_move_2(s, moves[i]);
        float val_of_new_state = minimax_eval_alpha_beta_pruning_2(&new_state,SEARCH_DEPTH, -1000000, 1000000);
        //fprintf(stderr,"Value for moving %s from %d to %d = %.2f.\n", chars_for_pieces[s->squares[i]], i, j, val_of_new_state);
        if (s->turn == WHITE)
        {
            if (best_val <= val_of_new_state)
            {
                best_val = val_of_new_state;
                best_move = moves[i];
                best_from = from;
                best_to = to;
                ret = 1;
            }
        } else {
            if (best_val >= val_of_new_state)
            {
                best_val = val_of_new_state;
                best_move = moves[i];
                best_from = from;
                best_to = to;
                ret = 1;
            }
        }
    }
    fprintf(stderr,"Moving %s from %d to %d with a value of %.2f.\n", chars_for_pieces[(int)s->squares[best_from]], best_from, best_to, best_val);
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
    *move = best_move;
    return ret;
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
                    float val_of_new_state = minimax_eval_alpha_beta_pruning(&new_state,SEARCH_DEPTH, -1000000, 1000000);
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
    fprintf(stderr,"Moving %s from %d to %d with a value of %.2f.\n", chars_for_pieces[(int)s->squares[best_from]], best_from, best_to, best_val);
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
