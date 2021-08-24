#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "SDL2/SDL.h"
#include <SDL2/SDL_image.h>

#include "board.h"
#include "legal_moves.h"
#include <time.h>
#include <sys/time.h>

struct timeval start, stop;

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
        fprintf(stderr,"SDL couldnt initialize: %s\n", SDL_GetError());
        return -1;
    }

    window = SDL_CreateWindow("SDL tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (window == NULL)
    {
        fprintf(stderr,"SDL couldnt initialize window: %s\n", SDL_GetError());
        return -1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

    int img_flags = IMG_INIT_PNG;
    if (!(IMG_Init(img_flags) & img_flags))
    {
        fprintf(stderr,"SDL couldnt initialize! IMG_Error: %s\n", IMG_GetError());
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
        fprintf(stderr,"Cannot read image %s. %s\n", filename, IMG_GetError());
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
        fprintf(stderr,"tried to select square %d.\n", from);
        if (!(get_player(cur_state.squares[from]) == cur_state.turn) || cur_state.squares[from] == BLANK)
            from = -1;
        legal_movesss = legal_moves(&cur_state, from);
        fprintf(stderr,"Selected state %d.\n", from);
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
            fprintf(stderr,"Going to state %d.\n", to);
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

#define KNIGHT_MOB_VAL 0.875
#define BISHOP_MOB_VAL 1.149
#define ROOK_MOB_VAL 1.17
#define QUEEN_MOB_VAL 1.093

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

float minimax_eval(game_state *s, int depth)
{
    if (depth == 0)
        return eval_comprehensive(s, 0);

    float best_val;
    if (s->turn == WHITE)
    {
        best_val =  -1000000;
    } else {
        best_val = 1000000;
    }

    for (int i = 0; i < 64; i++)
    {
        if (get_player(s->squares[i]) == s->turn)
        {
            uint64_t all_moves_from_i = legal_moves(s, i);

            for (int j = 0; j < 64; j++)
            {
                if (get_nth_bit(all_moves_from_i, j))
                {
                    game_state new_state = make_move(*s, i, j);
                    float val_of_new_state = eval_function(&new_state, depth-1);
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
            uint64_t all_moves_from_i = legal_moves(s, i);

            for (int j = 0; j < 64; j++)
            {
                if (get_nth_bit(all_moves_from_i, j))
                {
                    game_state new_state = make_move(*s, i, j);
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
    fprintf(stderr,"Moving %s from %d to %d with a value of %.2f", chars_for_pieces[s->squares[best_from]], best_from, best_to, best_val);
    return ret;
}

char files[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
char ranks[] = {'1', '2', '3', '4', '5', '6', '7', '8'};
char get_rank_for_board_index(int idx)
{
    char ret;
    ret = ranks[board_index_to_coord_y(idx)];
    return ret;
}

char get_file_for_board_index(int idx)
{
    char ret;
    ret = files[board_index_to_coord_x(idx)];
    return ret;
}

int main(int argc, char *argv[])
{
    double secs = 0;

    int player= 0;
    printf("Enter turn, 0 for white, 1 for black: ");
    scanf(" %d", &player);

    player = player ? BLACK: WHITE;
    printf("Player is %d.\n", player);

    SDL_Rect rect;

    // Do stuff  here


    init_graphics();
    init_textures();
    srand(time(NULL));

    int counter = 0;

    while (!stop_main_loop)
    {
        while (SDL_PollEvent(&event))
        {
            process_event();
        }

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
        for (int i = 0; i < 64; i++)
        {
            rect = board_idx_to_piece_rect(i);
            SDL_RenderCopy(renderer, texture_pieces[cur_state.squares[i]], NULL, &rect);
        }

        SDL_RenderPresent(renderer);
        //gettimeofday(&start, NULL);


        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture_board, NULL, NULL);

        if (cur_state.turn != player)
        {
            if(choose_best_move(&cur_state, &from, &to)<0)
            {
                fprintf(stderr,"Check mate but with passion!!!\n");
                stop_main_loop = 1;
                continue;
            }
        }

        if (from != -1 && to != -1)
        {
            fprintf(stderr,"Move %d: ", counter);
            char uci_move[] = {'a', 'a', 'a', 'a'};
            uci_move[0] = get_file_for_board_index(from);
            uci_move[1] = get_rank_for_board_index(from);
            uci_move[2] = get_file_for_board_index(to);
            uci_move[3] = get_rank_for_board_index(to);
            fprintf(stdout, "%s\n", uci_move);
            cur_state = make_move(cur_state, from, to);

            counter ++;
            king_we_re_searching_for = (cur_state.turn==BLACK)? B_KING: W_KING;
            king_index = find_piece(&cur_state, king_we_re_searching_for);
            check_status = which_pieces_check_king(&cur_state, king_index);
            from = -1; to = -1;
        }

        if (check_status)
        {
            if (is_check_mate(&cur_state, king_index))
            {
                fprintf(stderr,"Check mate!!!\n");
                stop_main_loop = 1;
                continue;
            }
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
        //gettimeofday(&stop, NULL);
        //secs = (double)(stop.tv_usec - start.tv_usec) / 1000 + (double)(stop.tv_sec - start.tv_sec);
        //fprintf(stderr,"time taken %f\n",secs);
        int delay = (counter > 60) ? 3000 : 30;
        SDL_Delay(delay);
    }
    cleanup();
    return 0;
}
