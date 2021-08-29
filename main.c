#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "board.h"
#include "legal_moves.h"
#include "gui.h"
#include "ai.h"
#include "evaluation.h"

UIState ui_state;

int main(int argc, char *argv[])
{
    game_state current_state = starting_state;
    if (argc > 1)
    {
        char* fen_string = argv[1];
        read_state(&current_state, fen_string);
    }

    set_flags_new_state(&current_state);

    select_game_mode(&ui_state);

    construct_new_ui_state(&ui_state);
    init_graphics(&ui_state);
    read_assets(&ui_state);

    double search_time;

    while (!(ui_state.stop_main_loop))
    {
        while (SDL_PollEvent(&(ui_state.event)))
        {
            process_event(&current_state, &ui_state);
        }

        render_game(&current_state, &ui_state);
        SDL_RenderPresent(ui_state.renderer);

        if (!(ui_state.is_check_mate_black || ui_state.is_check_mate_white || ui_state.stalemate))
        {
            if (current_state.turn == WHITE && ui_state.player_white == AI)
            {
                int ret = choose_best_move_2(&current_state, &(ui_state.move), &(ui_state.from), &(ui_state.to), &search_time);
                if (ret == -1)
                    continue;
                process_move(&current_state, &ui_state);
            }
            if (current_state.turn == BLACK && ui_state.player_black == AI)
            {
                int ret = choose_best_move_2(&current_state, &(ui_state.move), &(ui_state.from), &(ui_state.to), &search_time);
                if (ret == -1)
                    continue;
                process_move(&current_state, &ui_state);
            }
        }
        SDL_Delay(33);
    }
    cleanup(&current_state, &ui_state);
    return 0;
}
