#ifndef GUI_H_
#define GUI_H_
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

#include "board.h"
#include "legal_moves.h"
#include "ai.h"

enum { AI, HUMAN };

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 720;
const int SQUARE_WIDTH = 75;

const int PIECES_TEXTURE_WIDTH = 60;
const int PIECES_TEXTURE_HEIGHT = 60;

const int TOP_BOTTOM_BARS_HEIGHT = 60;
const int WHITE_TO_PLAY_X = 0;
const int WHITE_TO_PLAY_Y = 660;
const int BLACK_TO_PLAY_X = 450;
const int BLACK_TO_PLAY_Y = 0;

const int TO_PLAY_HEIGHT = 60;
const int TO_PLAY_WIDTH  = 150;

const int CAPTURED_WHITE_PIECES_X = 0;
const int CAPTURED_WHITE_PIECES_Y = 0;

const int CAPTURED_BLACK_PIECES_X = 525;
const int CAPTURED_BLACK_PIECES_Y = 660;

const int CAPTURED_WHITE_PIECES_DX = 25;
const int CAPTURED_BLACK_PIECES_DX = -25;

typedef struct
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Surface* temp_surface;
    SDL_Texture* texture_board;
    SDL_Texture* texture_selected_square;
    SDL_Texture* texture_check_square;
    SDL_Texture* texture_check_king;
    SDL_Texture* texture_legal_move;
    SDL_Texture* texture_last_moves;
    SDL_Texture* texture_pieces[20];
    SDL_Texture* texture_white_win;
    SDL_Texture* texture_black_win;
    SDL_Texture* texture_stalemate;
    SDL_Texture* texture_white_to_play;
    SDL_Texture* texture_black_to_play;
    SDL_Texture* texture_doge_overlay;
    SDL_Texture* texture_doge_king_check;

    SDL_Event event;
    int mouse_x;
    int mouse_y;
    int from;
    int to;
    int last_from;
    int last_to;
    int stop_main_loop;
    int player_black;
    int player_white;
    uint64_t legal_moves;
    uint64_t white_check_status;
    uint64_t black_check_status;
    int stalemate;

    Move move;
    int is_check_mate_black;
    int is_check_mate_white;

    int counter;

    uint8_t captured_white_pieces[15];
    uint8_t captured_black_pieces[15];
    int n_captured_white_pieces;
    int n_captured_black_pieces;
    uint8_t is_doge_mode;

} UIState;

void construct_new_ui_state(UIState* s)
{
    s->from = -1;
    s->to = -1;
    s->last_from = -1;
    s->last_to = -1;
    s->stop_main_loop = 0;;
    s->counter = 0;
    s->is_check_mate_white = 0;
    s->is_check_mate_black = 0;
    s->stalemate = 0;
    s->is_doge_mode = 0;

    for (int i = 0; i < 15; i++)
    {
        s->captured_white_pieces[i] = 0;
        s->captured_black_pieces[i] = 0;
    }
    s->n_captured_white_pieces = 0;
    s->n_captured_black_pieces = 0;
}

int init_graphics(UIState* s)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
    {
        printf("SDL couldnt initialize: %s\n", SDL_GetError());
        return -1;
    }

    s->window = SDL_CreateWindow("Madness Carl's Son", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (s->window == NULL)
    {
        printf("SDL couldnt initialize window: %s\n", SDL_GetError());
        return -1;
    }

    s->renderer = SDL_CreateRenderer(s->window, -1, SDL_RENDERER_SOFTWARE);

    int img_flags = IMG_INIT_PNG;
    if (!(IMG_Init(img_flags) & img_flags))
    {
        printf("SDL couldnt initialize! IMG_Error: %s\n", IMG_GetError());
        return -1;
    }

    return 0;
}

SDL_Texture* load_image(char* filename, UIState* s)
{
    SDL_Texture* target_surface = NULL;
    SDL_Surface* temp_surface = IMG_Load(filename);
    if (temp_surface == NULL)
    {
        printf("Cannot read image %s. %s\n", filename, IMG_GetError());
        return NULL;
    }
    target_surface = SDL_CreateTextureFromSurface(s->renderer, temp_surface);
    SDL_SetTextureBlendMode(target_surface, SDL_BLENDMODE_BLEND);
    SDL_FreeSurface(temp_surface);
    return target_surface;
}

int read_assets(UIState* s)
{
    s->texture_board            = load_image("assets/board-with-margins.png", s);
    s->texture_selected_square  = load_image("assets/selected-square.png", s);
    s->texture_check_square     = load_image("assets/check-square.png", s);
    s->texture_check_king       = load_image("assets/king-check.png", s);
    s->texture_legal_move       = load_image("assets/legal-move.png", s);
    s->texture_last_moves       = load_image("assets/last-move-squares.png", s);
    s->texture_pieces[B_ROOK]   = load_image("assets/black-rook.png", s);
    s->texture_pieces[B_PAWN]   = load_image("assets/black-pawn.png", s);
    s->texture_pieces[B_KING]   = load_image("assets/black-king.png", s);
    s->texture_pieces[B_QUEEN]  = load_image("assets/black-queen.png", s);
    s->texture_pieces[B_BISHOP] = load_image("assets/black-bishop.png", s);
    s->texture_pieces[B_KNIGHT] = load_image("assets/black-knight.png", s);
    s->texture_pieces[W_ROOK]   = load_image("assets/white-rook.png", s);
    s->texture_pieces[W_PAWN]   = load_image("assets/white-pawn.png", s);
    s->texture_pieces[W_KING]   = load_image("assets/white-king.png", s);
    s->texture_pieces[W_QUEEN]  = load_image("assets/white-queen.png", s);
    s->texture_pieces[W_BISHOP] = load_image("assets/white-bishop.png", s);
    s->texture_pieces[W_KNIGHT] = load_image("assets/white-knight.png", s);
    s->texture_white_win        = load_image("assets/check-mate-white-win.png", s);
    s->texture_black_win        = load_image("assets/check-mate-black-win.png", s);
    s->texture_stalemate        = load_image("assets/stalemate.png", s);
    s->texture_white_to_play    = load_image("assets/white-to-play.png", s);
    s->texture_black_to_play    = load_image("assets/black-to-play.png", s);
    s->texture_doge_overlay     = load_image("assets/doge-overlay-normal.png", s);
    s->texture_doge_king_check  = load_image("assets/doge-king-check.png", s);
    return 0;
}

int board_idx_to_graphics_y(int idx)
{
    return idx / 8;
}

int board_idx_to_graphics_x(int idx)
{
    return idx % 8;
}

void board_idx_to_piece_rect(int idx, SDL_Rect* srcrect)
{

    int rank = board_idx_to_graphics_y(idx);
    int file = board_idx_to_graphics_x(idx);

    srcrect->x = (file) * SQUARE_WIDTH + (SQUARE_WIDTH - PIECES_TEXTURE_WIDTH)/2;
    srcrect->y = TOP_BOTTOM_BARS_HEIGHT + (rank) * SQUARE_WIDTH + (SQUARE_WIDTH - PIECES_TEXTURE_HEIGHT)/2;
    srcrect->w = PIECES_TEXTURE_WIDTH;
    srcrect->h = PIECES_TEXTURE_HEIGHT;
}

void board_idx_to_square_rect(int idx, SDL_Rect* srcrect)
{

    int rank = board_idx_to_graphics_y(idx);
    int file = board_idx_to_graphics_x(idx);

    srcrect->x = (file) * SQUARE_WIDTH;
    srcrect->y = TOP_BOTTOM_BARS_HEIGHT + (rank) * SQUARE_WIDTH;
    srcrect->w = SQUARE_WIDTH;
    srcrect->h = SQUARE_WIDTH;
}

int pixel_coords_to_board_idx(int x, int y)
{
    int board_x = x / SQUARE_WIDTH;
    if (y < TOP_BOTTOM_BARS_HEIGHT)
        return -1;
    int board_y = (y - TOP_BOTTOM_BARS_HEIGHT) / SQUARE_WIDTH;
    if (board_x < 0 || board_x > 7 || board_y > 7 || board_y < 0)
        return -1;
    return board_x + 8*board_y;
}

void cleanup(game_state* s, UIState* ui_s)
{
    SDL_DestroyWindow(ui_s->window);
    ui_s->window = NULL;
    IMG_Quit();
    SDL_Quit();
}

void update_check_data(game_state* s, UIState* ui_s)
{

    int king_index;

    king_index = find_piece(s, (s->turn == WHITE)?W_KING:B_KING);
    if (s->turn == WHITE)
    {
        ui_s->white_check_status = which_pieces_check_king(s, king_index);
        ui_s->is_check_mate_white = is_check_mate(s);
    } else {
        ui_s->black_check_status = which_pieces_check_king(s, king_index);
        ui_s->is_check_mate_black = is_check_mate(s);
    }
    ui_s->stalemate = (!ui_s->white_check_status && !ui_s->black_check_status && (ui_s->is_check_mate_black || ui_s->is_check_mate_white));
    if (ui_s->stalemate)
    {
        ui_s->is_check_mate_black = 0;
        ui_s->is_check_mate_white = 0;
    }
}

char uci_move[] = {'a', 'a', 'a', 'a'};

void process_move(game_state* s, UIState* ui_s)
{
    int from = get_from_bits(ui_s->move);
    int to = get_to_bits(ui_s->move);

    if (s->squares[to] != BLANK)
    {
        int player = get_player(s->squares[to]);
        if (player == WHITE)
        {
            ui_s->captured_white_pieces[ui_s->n_captured_white_pieces] = s->squares[to];
            ui_s-> n_captured_white_pieces ++;
        } else {
            ui_s->captured_black_pieces[ui_s->n_captured_black_pieces] = s->squares[to];
            ui_s-> n_captured_black_pieces ++;
        }
    }

    *s = make_move_2(s, ui_s->move);
    update_check_data(s, ui_s);

    uci_move[0] = get_file_for_board_index(from);
    uci_move[1] = get_rank_for_board_index(from);
    uci_move[2] = get_file_for_board_index(to);
    uci_move[3] = get_rank_for_board_index(to);
    fprintf(stdout, "%s\n", uci_move);

    ui_s->last_to = ui_s->to;
    ui_s->last_from = ui_s->from;
    ui_s->to = -1;
    ui_s->from = -1;
    ui_s->counter++;

    //if (ui_s->counter > 9)
    //    SEARCH_DEPTH = 5;

}

void process_click(game_state* s, UIState* ui_s)
{
    if (ui_s->from == -1)
    {
        ui_s->from = pixel_coords_to_board_idx(ui_s->mouse_x, ui_s->mouse_y);
        if (ui_s->from == -1)
            return;
        if (!(get_player(s->squares[ui_s->from]) == s->turn) || s->squares[ui_s->from] == BLANK)
        {
            ui_s->from = -1;
            ui_s->to = -1;
        }
        else {
            ui_s->legal_moves = get_legal_destinations(s, ui_s->from);
        }
    } else if (ui_s->to == -1) {
        ui_s->to = pixel_coords_to_board_idx(ui_s->mouse_x,ui_s->mouse_y);
        if (ui_s->to == -1)
            return;
        Move move = 0;
        move = set_from_bits(move, ui_s->from);
        move = set_to_bits(move, ui_s->to);
        if (is_move_legal(ui_s->legal_moves, ui_s->to))
        {
            if (is_pawn(s->squares[ui_s->from]) &&
                board_index_to_coord_y(ui_s->from) == pawn_initial_ranks[get_opponent(s->turn)])
            {
                uint8_t promotion = 0;
                fprintf(stderr, "What do you want to promote your pawn to? \n");
                fprintf(stderr, "[0] QUEEN\n");
                fprintf(stderr, "[1] ROOK\n");
                fprintf(stderr, "[2] BISHOP\n");
                fprintf(stderr, "[3] KNIGHT\n");
                scanf(" %c", &promotion);
                promotion = promotion - 48;
                move = set_promotion_bits(move, promotion);
            }
            ui_s->move = move;
            process_move(s, ui_s);
        } else {
            ui_s->from = -1;
            ui_s->to = -1;
        }
    }
}

void process_event(game_state* s, UIState* ui_s){
    if (ui_s->event.type == SDL_QUIT)
        ui_s->stop_main_loop = 1;
    else if(ui_s->event.type == SDL_MOUSEBUTTONUP && ui_s->event.button.button == SDL_BUTTON_LEFT)
    {
        ui_s->mouse_x = ui_s->event.button.x;
        ui_s->mouse_y = ui_s->event.button.y;
        process_click(s, ui_s);
    } else if (ui_s->event.type == SDL_KEYUP && ui_s->event.key.keysym.sym == SDLK_w) {
        ui_s->is_doge_mode = 1 - ui_s->is_doge_mode;
    }
}

void render_possible_moves(game_state* s, UIState* ui_s)
{
    SDL_Rect rect;
    board_idx_to_square_rect(ui_s->from, &rect);
    SDL_RenderCopy(ui_s->renderer, ui_s->texture_selected_square, NULL, &rect);
    for (int z = 0; z < 64; z++)
    {
        if (get_nth_bit(ui_s->legal_moves, z))
        {
            board_idx_to_square_rect(z, &rect);
            SDL_RenderCopy(ui_s->renderer, ui_s->texture_legal_move, NULL, &rect);
        }
    }
}

void render_last_move(game_state* s, UIState* ui_s)
{

    SDL_Rect rect;
    board_idx_to_square_rect(ui_s->last_from, &rect);
    SDL_RenderCopy(ui_s->renderer, ui_s->texture_last_moves, NULL, &rect);
    board_idx_to_square_rect(ui_s->last_to, &rect);
    SDL_RenderCopy(ui_s->renderer, ui_s->texture_last_moves, NULL, &rect);
}

void render_check_status(game_state* s, UIState* ui_s, int player)
{
    SDL_Rect rect;

    int king_index = find_piece(s, player);
    board_idx_to_square_rect(king_index, &rect);
    uint64_t check_status = player==W_KING ? ui_s->white_check_status : ui_s->black_check_status;
    SDL_RenderCopy(ui_s->renderer, ui_s->texture_check_king, NULL, &rect);
    if (ui_s->is_doge_mode)
        SDL_RenderCopy(ui_s->renderer, ui_s->texture_doge_king_check, NULL, &rect);
    for (int i = 0; i < 64; i++)
    {
        if (get_nth_bit(check_status, i) == 0)
            continue;
        board_idx_to_square_rect(i, &rect);
        SDL_RenderCopy(ui_s->renderer, ui_s->texture_check_square, NULL, &rect);
    }
}

void render_checkmate(game_state* s, UIState* ui_s, int player)
{

    SDL_Rect rect;

    int king_index = find_piece(s, player);
    board_idx_to_piece_rect(king_index, &rect);
    uint64_t check_status = player==W_KING ? ui_s->white_check_status : ui_s->black_check_status;
    SDL_RenderCopy(ui_s->renderer, ui_s->texture_check_king, NULL, &rect);
    for (int i = 0; i < 64; i++)
    {
        if (get_nth_bit(check_status, i) == 0)
            continue;
        board_idx_to_piece_rect(i, &rect);
        SDL_RenderCopy(ui_s->renderer, ui_s->texture_check_square, NULL, &rect);
    }
    SDL_Texture* screen_texture = (player == W_KING) ? ui_s->texture_black_win : ui_s->texture_white_win;

    SDL_RenderCopy(ui_s->renderer, screen_texture, NULL, NULL);
}

void render_stalemate(game_state* s, UIState* ui_s)
{
    SDL_RenderCopy(ui_s->renderer, ui_s->texture_stalemate, NULL, NULL);
}

void render_turn(game_state* s, UIState* ui_s)
{
    SDL_Rect rect;
    int turn = s->turn;
    rect.x = (turn == WHITE) ? WHITE_TO_PLAY_X : BLACK_TO_PLAY_X;
    rect.y = (turn == WHITE) ? WHITE_TO_PLAY_Y : BLACK_TO_PLAY_Y;
    rect.h = TO_PLAY_HEIGHT;
    rect.w = TO_PLAY_WIDTH;
    SDL_Texture* texture_to_render = (turn == WHITE) ? ui_s->texture_white_to_play : ui_s->texture_black_to_play;
    SDL_RenderCopy(ui_s->renderer, texture_to_render, NULL, &rect);
}

void render_captured_pieces(game_state* s, UIState* ui_s)
{
    SDL_Rect rect;
    for (int i = 0; i < ui_s->n_captured_black_pieces; i++)
    {
        //printf("%d, %d pieces captured.\n", ui_s->n_captured_white_pieces, ui_s->n_captured_black_pieces);
        int piece = ui_s->captured_black_pieces[i];
        rect.x = CAPTURED_BLACK_PIECES_X + i * CAPTURED_BLACK_PIECES_DX;
        rect.y = CAPTURED_BLACK_PIECES_Y;
        rect.h = 60;
        rect.w = 60;
        SDL_RenderCopy(ui_s->renderer, ui_s->texture_pieces[piece], NULL, &rect);
        if (ui_s->is_doge_mode)
            SDL_RenderCopy(ui_s->renderer, ui_s->texture_doge_king_check, NULL, &rect);
    }
    for (int i = 0; i < ui_s->n_captured_white_pieces; i++)
    {
        int piece = ui_s->captured_white_pieces[i];
        rect.x = CAPTURED_WHITE_PIECES_X + i * CAPTURED_WHITE_PIECES_DX;
        rect.y = CAPTURED_WHITE_PIECES_Y;
        rect.h = 60;
        rect.w = 60;
        SDL_RenderCopy(ui_s->renderer, ui_s->texture_pieces[piece], NULL, &rect);
        if (ui_s->is_doge_mode)
            SDL_RenderCopy(ui_s->renderer, ui_s->texture_doge_king_check, NULL, &rect);
    }
}

void render_doge_overlays(game_state* s, UIState* ui_s)
{
    SDL_Rect rect;
    for (int i = 0; i < 64; i++)
    {
        if (s->squares[i] == BLANK)
            continue;
        board_idx_to_piece_rect(i, &rect);
        SDL_RenderCopy(ui_s->renderer, ui_s->texture_doge_overlay, NULL, &rect);
    }
}

void render_pieces(game_state* s, UIState* ui_s)
{
    SDL_Rect rect;
    for (int i = 0; i < 64; i++)
    {
        board_idx_to_piece_rect(i, &rect);
        SDL_RenderCopy(ui_s->renderer, ui_s->texture_pieces[s->squares[i]], NULL, &rect);
    }
}

void render_game(game_state* s, UIState* ui_s)
{

    SDL_RenderClear(ui_s->renderer);
    SDL_RenderCopy(ui_s->renderer, ui_s->texture_board, NULL, NULL);

    render_pieces(s, ui_s);
    render_turn(s, ui_s);
    render_captured_pieces(s, ui_s);

    if (ui_s->is_doge_mode)
        render_doge_overlays(s, ui_s);

    if (ui_s->from != -1 && ui_s->to == -1)
        render_possible_moves(s, ui_s);

    if (ui_s->last_from != -1 && ui_s->last_to != -1)
        render_last_move(s, ui_s);

    if (ui_s->white_check_status)
        render_check_status(s, ui_s, W_KING);

    if (ui_s->black_check_status)
        render_check_status(s, ui_s, B_KING);

    if (ui_s->stalemate)
        render_stalemate(s, ui_s);

    if (ui_s->is_check_mate_white)
        render_checkmate(s, ui_s, W_KING);

    if (ui_s->is_check_mate_black)
        render_checkmate(s, ui_s, B_KING);
}

void select_game_mode(UIState* ui_s)
{

    char game_mode_selection;
    fprintf(stderr, "Select game mode: \n");
    fprintf(stderr, "    1. Player vs Player[p] \n");
    fprintf(stderr, "    2. Player vs AI[a] \n");
    game_mode_selection = getc(stdin);
    if (game_mode_selection == 'p')
    {
        ui_s->player_black = HUMAN;
        ui_s->player_white = HUMAN;
    } else if (game_mode_selection == 'a') {
        fprintf(stderr, "Choose a side: \n");
        fprintf(stderr, "    1. White[w] \n");
        fprintf(stderr, "    2. Black[b] \n");
        scanf(" %c", &game_mode_selection);
        ui_s->player_white = game_mode_selection == 'w' ? HUMAN : AI;
        ui_s->player_black = game_mode_selection == 'w' ? AI : HUMAN;
    }
}

#endif // GUI_H_
