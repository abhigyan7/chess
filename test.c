#include <stdio.h>
#include <stdint.h>

#include "board.h"
#include "legal_moves.h"
#include "bitutils.h"
#include "ai.h"

int main(int argc, char *argv[])
{
    game_state s;
    read_state(&s, test_fenstring_4);
    set_flags_new_state(&s);
    Move moves[100];
    int nmoves = get_legal_moves_as_move_array(&s, moves);
    Move move;
    double time = 0;
    n_states_explored = 0;
    choose_best_move_2(&s, &move, &time);
    n_states_explored = 0;
    time = 0;
    sort_moves_by_static_eval(&s, moves, nmoves);
}
