#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "board.h"
//#include "moves_generation.h"
#include "legal_moves.h"


SDL_Window* window;
SDL_Renderer* renderer;
SDL_Surface* temp_surface;
SDL_Texture* texture_board;
SDL_Texture* texture_selected_square;
SDL_Texture* texture_pieces[20];

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;
const int SQUARE_WIDTH = SCREEN_WIDTH/8;

const int PIECES_TEXTURE_WIDTH = 60;
const int PIECES_TEXTURE_HEIGHT = 60;

int graphics_init()
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
    // target_surface = SDL_ConvertSurface( temp_surface, surface_screen->format, 0 );
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

int init_graphics()
{
    texture_board = load_image("assets/board.png");
    texture_selected_square = load_image("assets/selected-square.png");
    texture_pieces[B_ROOK]= load_image("assets/black-rook.png");
    texture_pieces[B_PAWN]=load_image( "assets/black-pawn.png");
    texture_pieces[B_KING]=load_image( "assets/black-king.png");
    texture_pieces[B_QUEEN]=load_image( "assets/black-queen.png");
    texture_pieces[B_BISHOP]=load_image( "assets/black-bishop.png");
    texture_pieces[B_KNIGHT]=load_image( "assets/black-knight.png");
    texture_pieces[W_ROOK]=load_image( "assets/white-rook.png");
    texture_pieces[W_PAWN]=load_image( "assets/white-pawn.png");
    texture_pieces[W_KING]=load_image( "assets/white-king.png");
    texture_pieces[W_QUEEN]=load_image( "assets/white-queen.png");
    texture_pieces[W_BISHOP]=load_image( "assets/white-bishop.png");
    texture_pieces[W_KNIGHT]=load_image( "assets/white-knight.png");
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

typedef struct {
    int from; int to;
} move;

game_state make_move(game_state s, move m)
{
    game_state ret;
    ret.turn = 8-(s.turn);
    memcpy(ret.squares, s.squares, sizeof(s.squares));
    ret.squares[m.to] = s.squares[m.from];
    ret.squares[m.from] = BLANK;
    return ret;
}

void cleanup()
{
    //SDL_FreeSurface();
    //image = NULL;

    SDL_DestroyWindow(window);
    window = NULL;

    IMG_Quit();

    SDL_Quit();
}

int is_move_legal(game_state s, int from, int to)
{
    uint64_t possible_moves = legal_moves(&s, from);
    return get_nth_bit(possible_moves, to);
}

int main(int argc, char *argv[])
{

    int initc = graphics_init();
    printf("init: %d\n", initc);
    int load = init_graphics();
    printf("Load: %d\n", load);

    SDL_Event event;
    int mouse_x, mouse_y;
    int mouse_button;

    game_state cur_state = starting_state;

    move m = {-1, -1};
    int stop_main_loop = 0;
    while (!stop_main_loop)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                stop_main_loop = 1;
            else if (event.type == SDL_MOUSEBUTTONUP && event.button.button==SDL_BUTTON_LEFT)
            {
                mouse_x = event.button.x;
                mouse_y = event.button.y;
                if (m.from == -1)
                {
                    m.from = pixel_coords_to_board_idx(mouse_x, mouse_y);
                    printf("tried to select square %d.\n", m.from);
                    printf("%d, %d\n", cur_state.squares[m.from], cur_state.turn);
                    if (!((cur_state.squares[m.from] & 8) == cur_state.turn) || cur_state.squares[m.from] == BLANK)
                        m.from = -1;
                    printf("Selected state %d.\n", m.from);
                } else if (m.to == -1) {
                    m.to = pixel_coords_to_board_idx(mouse_x, mouse_y);
                    if (is_move_legal(cur_state, m.from, m.to))
                    {
                        cur_state = make_move(cur_state, m);
                        printf("Going to state %d.\n", m.to);
                    }
                    m.to = -1;  m.from=-1;
                }
            }
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture_board, NULL, NULL);
        SDL_Rect rect;
        mouse_button = SDL_GetMouseState(&mouse_x, &mouse_y);
        if (m.from != -1 && m.to == -1)
        {
            rect = board_idx_to_square_rect(m.from);
            SDL_RenderCopy(renderer, texture_selected_square, NULL, &rect);
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
