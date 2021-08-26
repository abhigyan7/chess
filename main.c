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

UIState ui_state;

int main(int argc, char *argv[])
{
    game_state current_state = starting_state;
    if (argc > 1)
    {
        char* fen_string = argv[1];
        read_state(&current_state, fen_string);
    }

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

        if (current_state.turn == WHITE && ui_state.player_white == AI)
        {
            choose_best_move(&current_state,  &(ui_state.from), &(ui_state.to), &search_time);
            process_move(&current_state, &ui_state);
        }
        if (current_state.turn == BLACK && ui_state.player_black == AI)
        {
            choose_best_move(&current_state,  &(ui_state.from), &(ui_state.to), &search_time);
            process_move(&current_state, &ui_state);
        }
        SDL_Delay(33);
    }
    cleanup(&current_state, &ui_state);
    return 0;
}
