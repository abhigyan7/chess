#ifndef _LEGAL_MOVES_H
#define _LEGAL_MOVES_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "bitutils.h"
#include "board.h"


enum DIRECTIONS {
    DIR_TOP, DIR_BOTTOM, DIR_LEFT, DIR_RIGHT,
    DIR_TOP_LEFT, DIR_TOP_RIGHT, DIR_BOTTOM_LEFT, DIR_BOTTOM_RIGHT
};

// all the translations for a knight's movements
const int knight_translations[][2] = {{-1,2},{-2,1},{1,2},{2,1},{-1,-2},{-2,-1},{1,-2},{2,-1}};

// the direction vectors for the directions in DIRECTIONS
// the indeces for this array are the values in DIRECTIONS
const int direction_vectors[][2] = {{0,1},{0,-1},{-1,0},{1,0},{-1,1},{1,1},{-1,-1},{1,-1}};

// the vectors for a normal pawn move to a blank square
// the indeces for this array are 0 for WHITE and 1 for BLACK
int pawn_move_vectors   [] = { DIR_TOP, DIR_BOTTOM};

// the initial vertical coordinates for the pawns for each side
// the indeces for this array are 0 for WHITE and 1 for BLACK
int pawn_initial_ranks  [] = { 1, 6};

// the direction vectors for pawns to capture opponent pieces
// the indeces for this array are 0 for WHITE and 1 for BLACK
// and then 0 and 1 for the two directions
int pawn_capture_vectors[][2] = { {DIR_TOP_LEFT, DIR_TOP_RIGHT}, {DIR_BOTTOM_LEFT, DIR_BOTTOM_RIGHT}};

int board_index_to_coord_x(int index){
    // returns the horizontal co-ordinate for the square at index
    // 0 is left, with the positive axes going right
    return index %8;
}

int board_index_to_coord_y(int index){
    // returns the vertical co-ordinate for the square at index
    // 0 is bottom, with the positive axes going up
    return (63-index)/8;
}

int coord_xy_to_board_index(int x, int y){
    // returns the index for the square at (x,y)
    // 0 is top left
    return (7-y)*8+x;
}

void print_moves(uint64_t moves)
{
    // prints the moves set in `moves` as an 8x8 grid with
    // 0 representing can't move and 1 representing can move
    // to the corresponding square in board

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            printf("%d", get_nth_bit(moves, i*8+j));
        }
        printf("\n");
    }
}

int get_square_in_direction(int start_cell, int direction, int n){

    // returns the index of the square that is n cells away from
    // start_cell in `direction`
    //
    // returns -1 if a wall is hit

    int start_cell_x = board_index_to_coord_x(start_cell);
    int start_cell_y = board_index_to_coord_y(start_cell);

    int dest_cell_x = start_cell_x+ n*direction_vectors[direction][0];
    int dest_cell_y = start_cell_y+n*direction_vectors[direction][1];

    if (dest_cell_x > 7 || dest_cell_x < 0 || dest_cell_y < 0 || dest_cell_y > 7)
        return -1;

    return coord_xy_to_board_index(dest_cell_x, dest_cell_y);
}

int get_square_for_knight_vector(int start_cell, int knight_vector){

    // returns the index of the square that a knight at start_cell
    // would end up if it moves according to knight_vector
    //
    // knight vector is an index to knight_translations
    //
    // returns -1 if a wall is hit

    int start_cell_x = board_index_to_coord_x(start_cell);
    int start_cell_y = board_index_to_coord_y(start_cell);

    int dest_cell_x = start_cell_x+ knight_translations[knight_vector][0];
    int dest_cell_y = start_cell_y+ knight_translations[knight_vector][1];

    if (dest_cell_x > 7 || dest_cell_x < 0 || dest_cell_y < 0 || dest_cell_y > 7)
        return -1;

    return coord_xy_to_board_index(dest_cell_x, dest_cell_y);
}

uint64_t fill_legal_squares_in_direction(game_state *s, int direction, int index)
{
    // returns 64-bits which show which of the places are legally reachable
    // by a piece at square `index` in the direction `direction`
    uint64_t ret = 0;
    int n = 1;
    while (1) {
        int position = get_square_in_direction(index, direction, n);
        n++;
        if (position == -1) // we've reached the wall
            break;
        else if (are_two_pieces_same_player(s->squares[position], s->squares[index]))
            // can't capture own player
            break;
        else if (is_blank(s->squares[position]))
            // can go to an empty square
            ret = set_nth_bit_to(ret, position, 1);
        else if (are_two_pieces_different_player(s->squares[position], s->squares[index]))
        {
            // can capture an enemy piece
            ret = set_nth_bit_to(ret, position, 1);
            // but can't go any beyond
            break;
        }
    }
    return ret;
}

uint64_t legal_move_queen(game_state *s,int index){

    // returns the legal moves for a queen at square `index`
    // as a 64-bit integer

    uint64_t possible_moves=0x0;
    for (int dir = DIR_TOP; dir <= DIR_BOTTOM_RIGHT; dir++)
    {
        uint64_t ret = fill_legal_squares_in_direction(s, dir, index);
        possible_moves = ret | possible_moves;
    }
    return possible_moves;
}

uint64_t legal_move_bishop(game_state *s,int index){

    // returns the legal moves for a bishop at square `index`
    // as a 64-bit integer

    uint64_t possible_moves=0x0;
    for (int dir = DIR_TOP_LEFT; dir <= DIR_BOTTOM_RIGHT; dir++)
    {
        uint64_t ret = fill_legal_squares_in_direction(s, dir, index);
        possible_moves = ret | possible_moves;
    }
    return possible_moves;
}

uint64_t legal_move_rook(game_state *s,int index){

    // returns the legal moves for a rook at square `index`
    // as a 64-bit integer

    uint64_t possible_moves=0x0;
    for (int dir = DIR_TOP; dir <= DIR_RIGHT; dir++)
    {
        uint64_t ret = fill_legal_squares_in_direction(s, dir, index);
        possible_moves = ret | possible_moves;
    }
    return possible_moves;
}

uint64_t legal_move_knight(game_state *s,int index){

    // returns the legal moves for a knight at square `index`
    // as a 64-bit integer

    uint64_t possible_moves=0x0;
    int position;
    for(int knight_vector = 0; knight_vector < 8; knight_vector++){
        position= get_square_for_knight_vector(index, knight_vector);
        if (position == -1)
            continue;
        if (are_two_pieces_same_player(s->squares[position], s->squares[index]))
            continue;
        if (is_blank(s->squares[position]))
        {
            possible_moves |= set_nth_bit_to(possible_moves, position, 1);
        }
        if (are_two_pieces_different_player(s->squares[position], s->squares[index]))
            possible_moves |= set_nth_bit_to(possible_moves, position, 1);
        }
    return possible_moves;
}

uint64_t legal_move_pawn(game_state *s,int index){

    // returns the legal moves for a pawn at square `index`
    // as a 64-bit integer

    uint64_t possible_moves=0x0;
    int position;
    position = get_square_in_direction(index, pawn_move_vectors[s->turn], 1);
    if (position != -1)
        if (is_blank(s->squares[position]))
        {
            // can go to an empty place
            possible_moves |= set_nth_bit_to(possible_moves, position, 1);

            position = get_square_in_direction(index, pawn_move_vectors[s->turn], 2);
            if (position != -1)
                if (    is_blank(s->squares[position])
                        && board_index_to_coord_y(index) == pawn_initial_ranks[s->turn]
                )
                    // can go two steps too if its the pawn's first move
                {
                    possible_moves |= set_nth_bit_to(possible_moves, position, 1);
                }
        }
    for (int direction = 0; direction < 2; direction++)
    {
        // for pawn captures
        position = get_square_in_direction(index, pawn_capture_vectors[s->turn][direction], 1);
        if (position == -1)
            continue;
        if (is_blank(s->squares[position]))
            continue;
        if (are_two_pieces_different_player(s->squares[position], s->squares[index]))
        {
            possible_moves |= set_nth_bit_to(possible_moves, position, 1);
        }
    }
    return possible_moves;
}

uint64_t legal_move_king(game_state *s,int index){

    // returns the legal moves for a king at square `index`
    // as a 64-bit integer

    uint64_t possible_moves=0x0;
    int position;
    for(int dir =0;dir<8;dir++){
        position =get_square_in_direction(index,dir,1);
        if (position == -1)
            continue;
        if (are_two_pieces_same_player(s->squares[index], s->squares[position]))
            continue;
        if (is_blank(s->squares[position]))
            possible_moves |= set_nth_bit_to(possible_moves, position, 1);
        else if (are_two_pieces_different_player(s->squares[index], s->squares[position]))
            possible_moves |= set_nth_bit_to(possible_moves, position, 1);
    }
    return possible_moves;
}

int get_square_at_end_of_direction(game_state *s, int direction, int index)
{
    // returns the square that is reachable and the furthest along `direction`
    // starting at square `index`

    int n = 1;
    int ret;
    while (1)
    {
        ret = get_square_in_direction(index, direction, n);
        if (ret == -1)
            break;
        if (!is_blank(s->squares[ret]))
            break;
        n++;
    }
    return ret;
}

int is_king_in_check(game_state *s, int king_index)
{
    // returns a 1 if the king at king_index is in check

    int player = get_player(s->squares[king_index]);

    int nearest_piece_in_direction, idx;

    for (int dir = DIR_TOP; dir <= DIR_RIGHT; dir++)
    {
        idx = get_square_at_end_of_direction(s, dir, king_index);
        if (idx == -1)
            continue;
        nearest_piece_in_direction = s->squares[idx];
        if (nearest_piece_in_direction == BLANK)
            continue;
        if (get_player(nearest_piece_in_direction) == player)
            continue;
        if (is_queen(nearest_piece_in_direction) || is_rook(nearest_piece_in_direction))
            return 1;
    }

    for (int dir = DIR_TOP_LEFT; dir <= DIR_BOTTOM_RIGHT; dir++)
    {
        idx = get_square_at_end_of_direction(s, dir, king_index);
        if (idx == -1)
            continue;
        nearest_piece_in_direction = s->squares[idx];
        if (nearest_piece_in_direction == BLANK)
            continue;
        if (get_player(nearest_piece_in_direction) == player)
            continue;
        if (is_queen(nearest_piece_in_direction) || is_bishop(nearest_piece_in_direction))
            return 1;
    }

    for (int vec = 0; vec < 8; vec++)
    {
        idx = get_square_for_knight_vector(king_index, vec);
        if (idx == -1)
            continue;
        nearest_piece_in_direction = s->squares[idx];
        if (nearest_piece_in_direction == BLANK)
            continue;
        if (get_player(nearest_piece_in_direction) == player)
            continue;
        if (is_knight(nearest_piece_in_direction))
            return 1;
    }

    for (int i = 0; i < 2; i++)
    {
        idx = get_square_in_direction(king_index, pawn_capture_vectors[player == BLACK][i], 1);
        if (idx == -1)
            continue;
        nearest_piece_in_direction = s->squares[idx];
        if (nearest_piece_in_direction == BLANK)
            continue;
        if (get_player(nearest_piece_in_direction) == player)
            continue;
        if (is_pawn(nearest_piece_in_direction))
            return 1;
    }
    return 0;
}

uint64_t which_pieces_check_king(game_state *s, int king_index)
{
    // returns the pieces that check the king at `king_index`

    int player = get_player(s->squares[king_index]);

    int nearest_piece_in_direction, idx;
    uint64_t ret = 0;

    for (int dir = DIR_TOP; dir <= DIR_RIGHT; dir++)
    {
        idx = get_square_at_end_of_direction(s, dir, king_index);
        if (idx == -1)
            continue;
        nearest_piece_in_direction = s->squares[idx];
        if (nearest_piece_in_direction == BLANK)
            continue;
        if (get_player(nearest_piece_in_direction) == player)
            continue;
        if (is_queen(nearest_piece_in_direction) || is_rook(nearest_piece_in_direction))
            ret = ret | set_nth_bit_to(ret, idx, 1);
    }

    for (int dir = DIR_TOP_LEFT; dir <= DIR_BOTTOM_RIGHT; dir++)
    {
        idx = get_square_at_end_of_direction(s, dir, king_index);
        if (idx == -1)
            continue;
        nearest_piece_in_direction = s->squares[idx];
        if (nearest_piece_in_direction == BLANK)
            continue;
        if (get_player(nearest_piece_in_direction) == player)
            continue;
        if (is_queen(nearest_piece_in_direction) || is_bishop(nearest_piece_in_direction))
            ret = ret | set_nth_bit_to(ret, idx, 1);
    }

    for (int vec = 0; vec < 8; vec++)
    {
        idx = get_square_for_knight_vector(king_index, vec);
        if (idx == -1)
            continue;
        nearest_piece_in_direction = s->squares[idx];
        if (nearest_piece_in_direction == BLANK)
            continue;
        if (get_player(nearest_piece_in_direction) == player)
            continue;
        if (is_knight(nearest_piece_in_direction))
            ret = ret | set_nth_bit_to(ret, idx, 1);
    }

    for (int i = 0; i < 2; i++)
    {
        idx = get_square_in_direction(king_index, pawn_capture_vectors[player == BLACK][i], 1);
        if (idx == -1)
            continue;
        nearest_piece_in_direction = s->squares[idx];
        if (nearest_piece_in_direction == BLANK)
            continue;
        if (get_player(nearest_piece_in_direction) == player)
            continue;
        if (is_pawn(nearest_piece_in_direction))
            ret = ret | set_nth_bit_to(ret, idx, 1);
    }
    return ret;
}

game_state make_move(game_state s, int from, int to)
{
    // executes a move and returns the resulting game_state

    game_state ret = s;
    ret.turn = get_opponent(s.turn);
    ret.squares[to] = s.squares[from];
    ret.squares[from] = BLANK;
    return ret;
}

int find_piece(game_state* s, int piece)
{

    // finds the piece `piece` in the board
    // and returns its location's index

    int index = -1;
    for (int i = 0; i < 64; i++)
    {
        if (s->squares[i] == piece)
        {
            index = i;
            break;
        }

    }
    return index;
}

uint64_t ensure_moves_are_legal(game_state* s, int index, uint64_t moves)
{
    // make sure none of the moves cause the player's own king
    // to be in check

    uint64_t ret = moves;

    int king_index = -1;
    int king_we_re_searching_for = (s->turn==BLACK)? B_KING: W_KING;
    king_index = find_piece(s, king_we_re_searching_for);

    for (int j = 0; j < 64; j++)
    {
        if (get_nth_bit(moves, j) == 1)
        {
            game_state new_state = make_move(*s, index, j);
            king_index = find_piece(&new_state, king_we_re_searching_for);
            if (is_king_in_check(&new_state, king_index))
            {
                ret = set_nth_bit_to(ret, j, 0);
            }
        }
    }
    return ret;
}

uint64_t legal_moves(game_state *s,int index)
{
    // returns all the legal moves that a piece at index can make

    uint64_t possible_moves =0x0;
    if(s->squares[index]!=BLANK){
        switch(s->squares[index]){
            case W_ROOK:
            case B_ROOK:
                possible_moves =legal_move_rook(s,index);
                break;
            case W_KNIGHT:
            case B_KNIGHT:
                possible_moves =legal_move_knight(s,index);
                break;
            case W_BISHOP:
            case B_BISHOP:
                possible_moves =legal_move_bishop(s,index);
                break;
            case W_KING:
            case B_KING:
                possible_moves =legal_move_king(s,index);
                break;
            case W_QUEEN:
            case B_QUEEN:
                possible_moves =legal_move_queen(s,index);
                break;
            case W_PAWN:
            case B_PAWN:
                possible_moves =legal_move_pawn(s,index);
                break;
        }
    }
    return ensure_moves_are_legal(s, index, possible_moves);
}

int is_check_mate(game_state* s, int king_index)
{
    int player = get_player(s->squares[king_index]);
    int opponent = get_opponent(player);

    for (int i = 0; i < 64; i++)
    {
        if (s->squares[i] == BLANK)
            continue;
        if (get_player(s->squares[i]) == opponent)
            continue;
        uint64_t moves = legal_moves(s, i);
        if (moves)
            return 0;
    }
    return 1;
}

#endif
