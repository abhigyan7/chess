#ifndef _LEGAL_MOVES_H
#define _LEGAL_MOVES_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "board.h"

uint64_t set_nth_bit_to(uint64_t integer, int n, int val)
{
    integer ^= (-val ^ integer) & (1UL << n);
    return integer;
}

int get_nth_bit(uint64_t integer, int n)
{
    return (integer >> n) & 1U;
}

enum DIRECTIONS{
    DIR_TOP, DIR_BOTTOM, DIR_LEFT, DIR_RIGHT,
    DIR_TOP_LEFT, DIR_TOP_RIGHT, DIR_BOTTOM_LEFT, DIR_BOTTOM_RIGHT
    };

const int vecs_for_knight[][2] = {{-1,2},{-2,1},{1,2},{2,1},{-1,-2},{-2,-1},{1,-2},{2,-1}};
const int vecs_for_directions[][2] = {{0,1},{0,-1},{-1,0},{1,0},{-1,1},{1,1},{-1,-1},{1,-1}};

int board_index_to_coord_x(int index){
    return index %8;
}

int board_index_to_coord_y(int index){
    return (63-index)/8;
}

int coord_xy_to_board_index(int x, int y){
    return (7-y)*8+x;
}

void print_moves(uint64_t moves)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            printf("%d", get_nth_bit(moves, i*8+j));
        }
        printf("\n");
    }
}

int popcount(uint64_t in)
{
    int ret;

    for (ret = 0; in; ret++)
    {
      in &= in - 1; // clear the least significant bit set
    }
    return ret;
}

int get_square_in_direction(int start_cell, int direction, int n){
    int start_cell_x = board_index_to_coord_x(start_cell);
    int start_cell_y = board_index_to_coord_y(start_cell);

    int dest_cell_x = start_cell_x+ n*vecs_for_directions[direction][0];
    int dest_cell_y = start_cell_y+n*vecs_for_directions[direction][1];

    if (dest_cell_x > 7 || dest_cell_x < 0 || dest_cell_y < 0 || dest_cell_y > 7)
        return -1;

    return coord_xy_to_board_index(dest_cell_x, dest_cell_y);
}

int get_square_for_knight_vector(int start_cell, int knight_vector){
    int start_cell_x = board_index_to_coord_x(start_cell);
    int start_cell_y = board_index_to_coord_y(start_cell);

    int dest_cell_x = start_cell_x+ vecs_for_knight[knight_vector][0];
    int dest_cell_y = start_cell_y+ vecs_for_knight[knight_vector][1];

    if (dest_cell_x > 7 || dest_cell_x < 0 || dest_cell_y < 0 || dest_cell_y > 7)
        return -1;

    return coord_xy_to_board_index(dest_cell_x, dest_cell_y);
}
//checking legal positions for king
int neighbor_empty(game_state *s,int index,int direction){
    int new_postion =get_square_in_direction(index,direction,1);
    if(new_postion != -1 && s->squares[new_postion]==BLANK){
        return 1;
    }
    return 0;
}

uint64_t fill_legal_squares_in_direction(game_state *s, int direction, int index)
{
    uint64_t ret = 0;
    int n = 1;
    while (1) {
        int position = get_square_in_direction(index, direction, n);
        n++;
        if (position == -1)
            break;
        else if (are_two_pieces_same_player(s->squares[position], s->squares[index]))
            break;
        else if (is_blank(s->squares[position]))
            ret = set_nth_bit_to(ret, position, 1);
        else if (are_two_pieces_different_player(s->squares[position], s->squares[index]))
        {
            ret = set_nth_bit_to(ret, position, 1);
            break;
        }
    }
    return ret;
}

//this function gives a 64 bit number where in each bit 1:it can move 0:it can't go for queen
uint64_t legal_move_queen(game_state *s,int index){
    uint64_t possible_moves=0x0;
    for (int dir = DIR_TOP; dir <= DIR_BOTTOM_RIGHT; dir++)
    {
        uint64_t ret = fill_legal_squares_in_direction(s, dir, index);
        possible_moves = ret | possible_moves;
    }
    return possible_moves;
}


uint64_t legal_move_bishop(game_state *s,int index){
    uint64_t possible_moves=0x0;
    for (int dir = DIR_TOP_LEFT; dir <= DIR_BOTTOM_RIGHT; dir++)
    {
        uint64_t ret = fill_legal_squares_in_direction(s, dir, index);
        possible_moves = ret | possible_moves;
    }
    return possible_moves;
}

uint64_t legal_move_rook(game_state *s,int index){
    uint64_t possible_moves=0x0;
    for (int dir = DIR_TOP; dir <= DIR_RIGHT; dir++)
    {
        uint64_t ret = fill_legal_squares_in_direction(s, dir, index);
        possible_moves = ret | possible_moves;
    }
    return possible_moves;
}

uint64_t legal_move_knight(game_state *s,int index){
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

int pawn_move_vectors   [] = { DIR_TOP, DIR_BOTTOM};
int pawn_initial_ranks  [] = { 1, 6};
int pawn_capture_vectors[][2] = { {DIR_TOP_LEFT, DIR_TOP_RIGHT}, {DIR_BOTTOM_LEFT, DIR_BOTTOM_RIGHT}};

uint64_t legal_move_pawn(game_state *s,int index){
    uint64_t possible_moves=0x0;
    int position;
    position = get_square_in_direction(index, pawn_move_vectors[s->turn], 1);
    if (position != -1)
        if (is_blank(s->squares[position]))
        {
            possible_moves |= set_nth_bit_to(possible_moves, position, 1);

            position = get_square_in_direction(index, pawn_move_vectors[s->turn], 2);
            if (position != -1)
                if (is_blank(s->squares[position]) && board_index_to_coord_y(index) == pawn_initial_ranks[s->turn])
                {
                    possible_moves |= set_nth_bit_to(possible_moves, position, 1);
                }
        }
    for (int direction = 0; direction < 2; direction++)
    {
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

int is_knight(int p)
{
    return (p == B_KNIGHT || p == W_KNIGHT);
}

int is_rook(int p)
{
    return (p == B_ROOK || p == W_ROOK);
}

int is_bishop(int p)
{
    return (p == B_BISHOP || p == W_BISHOP);
}

int is_king(int p)
{
    return (p == B_KING || p == W_KING);
}

int is_queen(int p)
{
    return (p == B_QUEEN || p == W_QUEEN);
}

int is_pawn(int p)
{
    return (p == B_PAWN || p == W_PAWN);
}

int get_square_at_end_of_direction(game_state *s, int direction, int index)
{
    int n = 1;
    int ret;
    do {
        ret = get_square_in_direction(index, direction, n);
        n++;
    } while (ret != -1 && is_blank(s->squares[ret]));
    return ret;
}

int is_king_in_check(game_state *s, int king_index)
{
    int player = get_player(s->squares[king_index]);

    int nearest_piece_in_direction, idx;

    for (int dir = DIR_TOP; dir <= DIR_RIGHT; dir++)
    {
        nearest_piece_in_direction = get_square_at_end_of_direction(s, dir, king_index);
        if (nearest_piece_in_direction == BLANK)
            continue;
        if (get_player(nearest_piece_in_direction) == player)
            continue;
        if (is_queen(nearest_piece_in_direction) || is_rook(nearest_piece_in_direction))
            return 1;
    }

    for (int dir = DIR_TOP_LEFT; dir <= DIR_BOTTOM_RIGHT; dir++)
    {
        nearest_piece_in_direction = get_square_at_end_of_direction(s, dir, king_index);
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
        nearest_piece_in_direction = s->squares[idx];
        if (nearest_piece_in_direction == BLANK)
            continue;
        if (get_player(nearest_piece_in_direction) == player)
            continue;
        if (is_knight(nearest_piece_in_direction))
            return 1;
    }

    return 0;
}

uint64_t which_pieces_threathen_king(game_state *s, int king_index)
{
    int player = get_player(s->squares[king_index]);

    uint64_t ret = 0;

    int nearest_piece_in_direction, idx;

    for (int dir = DIR_TOP; dir <= DIR_RIGHT; dir++)
    {
        nearest_piece_in_direction = get_square_at_end_of_direction(s, dir, king_index);
        if (nearest_piece_in_direction == BLANK)
            continue;
        if (get_player(nearest_piece_in_direction) == player)
            continue;
        if (is_queen(nearest_piece_in_direction) || is_rook(nearest_piece_in_direction))
            ret = ret | set_nth_bit_to(ret, nearest_piece_in_direction, 1);
    }

    for (int dir = DIR_TOP_LEFT; dir <= DIR_BOTTOM_RIGHT; dir++)
    {
        nearest_piece_in_direction = get_square_at_end_of_direction(s, dir, king_index);
        if (nearest_piece_in_direction == BLANK)
            continue;
        if (get_player(nearest_piece_in_direction) == player)
            continue;
        if (is_queen(nearest_piece_in_direction) || is_bishop(nearest_piece_in_direction))
            ret = ret | set_nth_bit_to(ret, nearest_piece_in_direction, 1);
    }

    for (int vec = 0; vec < 8; vec++)
    {
        idx = get_square_for_knight_vector(king_index, vec);
        nearest_piece_in_direction = s->squares[idx];
        if (nearest_piece_in_direction == BLANK)
            continue;
        if (get_player(nearest_piece_in_direction) == player)
            continue;
        if (is_knight(nearest_piece_in_direction))
            ret = ret | set_nth_bit_to(ret, nearest_piece_in_direction, 1);
    }

    for (int i = 0; i < 2; i++)
    {
        idx = get_square_in_direction(king_index, pawn_capture_vectors[player == BLACK][i], 1);
        nearest_piece_in_direction = s->squares[idx];
        if (nearest_piece_in_direction == BLANK)
            continue;
        if (get_player(nearest_piece_in_direction) == player)
            continue;
        if (is_knight(nearest_piece_in_direction))
            ret = ret | set_nth_bit_to(ret, nearest_piece_in_direction, 1);
    }
    return ret;
}

uint64_t legal_moves(game_state *s,int index){
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
    return possible_moves;
}

#endif
