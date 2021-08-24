#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "SDL2/SDL.h"
#include <SDL2/SDL_image.h>

#include "board.h"
#include "legal_moves.h"


SDL_Window* window;
SDL_Renderer* renderer;
SDL_Surface* temp_surface;
SDL_Texture* texture_board;
SDL_Texture* texture_selected_square;
SDL_Texture* texture_check_square;
SDL_Texture* texture_pieces[20];

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;
const int SQUARE_WIDTH = 75;

const int PIECES_TEXTURE_WIDTH = 60;
const int PIECES_TEXTURE_HEIGHT = 60;

SDL_Event event;
int mouse_x, mouse_y;
game_state cur_state = starting_state;
uint64_t check_status = 0;
int from=-1, to=-1;
int stop_main_loop = 0;
uint64_t legal_movesss;
int king_index = -1;
int king_we_re_searching_for = W_KING;
int init_graphics()

{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
    {
        printf("SDL couldnt initialize: %s\n", SDL_GetError());
        return -1;
    }

    window = SDL_CreateWindow("SDL tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (window == NULL)
    {
        printf("SDL couldnt initialize window: %s\n", SDL_GetError());
        return -1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

    int img_flags = IMG_INIT_PNG;
    if (!(IMG_Init(img_flags) & img_flags))
    {
        printf("SDL couldnt initialize! IMG_Error: %s\n", IMG_GetError());
        return -1;
    }

    return 0;
}

SDL_Texture* load_image(char* filename)
{
    SDL_Texture* target_surface = NULL;
    SDL_Surface* temp_surface = IMG_Load(filename);
    if (temp_surface == NULL)
    {
        printf("Cannot read image %s. %s\n", filename, IMG_GetError());
        return NULL;
    }
    target_surface = SDL_CreateTextureFromSurface(renderer, temp_surface);
    SDL_SetTextureBlendMode(target_surface, SDL_BLENDMODE_BLEND);
    SDL_FreeSurface(temp_surface);
    return target_surface;
}

int init_textures()
{
    texture_board = load_image("assets/board.png");
    texture_selected_square  = load_image("assets/selected-square.png");
    texture_check_square     = load_image("assets/check-square.png");
    texture_pieces[B_ROOK]   = load_image("assets/black-rook.png");
    texture_pieces[B_PAWN]   = load_image("assets/black-pawn.png");
    texture_pieces[B_KING]   = load_image("assets/black-king.png");
    texture_pieces[B_QUEEN]  = load_image("assets/black-queen.png");
    texture_pieces[B_BISHOP] = load_image("assets/black-bishop.png");
    texture_pieces[B_KNIGHT] = load_image("assets/black-knight.png");
    texture_pieces[W_ROOK]   = load_image("assets/white-rook.png");
    texture_pieces[W_PAWN]   = load_image("assets/white-pawn.png");
    texture_pieces[W_KING]   = load_image("assets/white-king.png");
    texture_pieces[W_QUEEN]  = load_image("assets/white-queen.png");
    texture_pieces[W_BISHOP] = load_image("assets/white-bishop.png");
    texture_pieces[W_KNIGHT] = load_image("assets/white-knight.png");
    return 0;
}

int board_idx_to_rank(int idx)
{
    return idx / 8;
}

int board_idx_to_file(int idx)
{
    return idx % 8;
}


SDL_Rect board_idx_to_piece_rect(int idx)
{

    int rank = board_idx_to_rank(idx);
    int file = board_idx_to_file(idx);
    SDL_Rect srcrect;

    srcrect.x = (file) * SQUARE_WIDTH + (SQUARE_WIDTH - PIECES_TEXTURE_WIDTH)/2;
    srcrect.y = (rank) * SQUARE_WIDTH + (SQUARE_WIDTH - PIECES_TEXTURE_HEIGHT)/2;
    srcrect.w = PIECES_TEXTURE_WIDTH;
    srcrect.h = PIECES_TEXTURE_HEIGHT;
    return srcrect;
}

SDL_Rect board_idx_to_square_rect(int idx)
{

    int rank = board_idx_to_rank(idx);
    int file = board_idx_to_file(idx);
    SDL_Rect srcrect;

    srcrect.x = (file) * SQUARE_WIDTH;
    srcrect.y = (rank) * SQUARE_WIDTH;
    srcrect.w = SQUARE_WIDTH;
    srcrect.h = SQUARE_WIDTH;
    return srcrect;
}

int pixel_coords_to_board_idx(int x, int y)
{
    int board_x = x / SQUARE_WIDTH;
    int board_y = y / SQUARE_WIDTH;
    return board_x + 8*board_y;
}

void cleanup()
{
    SDL_DestroyWindow(window);
    window = NULL;
    IMG_Quit();
    SDL_Quit();
}

void process_click()
{
    if (from == -1)
    {
    from = pixel_coords_to_board_idx(mouse_x, mouse_y);
    printf("tried to select square %d.\n", from);
    if (!(get_player(cur_state.squares[from]) == cur_state.turn) || cur_state.squares[from] == BLANK)
        from = -1;
        legal_movesss = legal_moves(&cur_state, from);
        printf("Selected state %d.\n", from);
    } else if (to == -1) {
        to = pixel_coords_to_board_idx(mouse_x, mouse_y);
        if (is_move_legal(legal_movesss, to))
        {
            cur_state = make_move(cur_state, from, to);
            king_we_re_searching_for = (cur_state.turn==BLACK)? B_KING: W_KING;
            king_index = find_piece(&cur_state, king_we_re_searching_for);
            check_status = which_pieces_check_king(&cur_state, king_index);
            if (check_status)
                if (is_check_mate(&cur_state, king_index))
                    stop_main_loop = 1;
            printf("Going to state %d.\n", to);
        }
        to = -1;  from=-1;
    }
}


void process_event(){
    if (event.type == SDL_QUIT)
        stop_main_loop = 1;
    else if(event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
    {
        mouse_x = event.button.x;
        mouse_y = event.button.y;
        process_click();
    }

}

int main(int argc, char *argv[])
{

    init_graphics();
    init_textures();

    while (!stop_main_loop)
    {
        while (SDL_PollEvent(&event))
        {
            process_event();
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture_board, NULL, NULL);
        SDL_Rect rect;
        if (from != -1 && to == -1)
        {
            rect = board_idx_to_square_rect(from);
            SDL_RenderCopy(renderer, texture_selected_square, NULL, &rect);
            for (int z = 0; z < 64; z++)
            {
                if (get_nth_bit(legal_movesss, z))
                {
                    rect = board_idx_to_square_rect(z);
                    SDL_RenderCopy(renderer, texture_selected_square, NULL, &rect);
                }
            }
        }

        if (check_status)
        {
            rect = board_idx_to_piece_rect(king_index);
            SDL_RenderCopy(renderer, texture_check_square, NULL, &rect);

            for (int i = 0; i < 64; i++)
            {
                if (get_nth_bit(check_status, i) == 0)
                    continue;
                rect = board_idx_to_piece_rect(i);
                SDL_RenderCopy(renderer, texture_check_square, NULL, &rect);
            }

        }
        for (int i = 0; i < 64; i++)
        {
            rect = board_idx_to_piece_rect(i);
            SDL_RenderCopy(renderer, texture_pieces[cur_state.squares[i]], NULL, &rect);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(33);
    }
    cleanup();
    return 0;
}
