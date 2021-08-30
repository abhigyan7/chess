#ifndef _LEGAL_MOVES_H
#define _LEGAL_MOVES_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "bitutils.h"
#include "board.h"

enum DIRECTIONS {
    DIR_TOP, DIR_BOTTOM, DIR_LEFT, DIR_RIGHT,
    DIR_TOP_LEFT, DIR_TOP_RIGHT, DIR_BOTTOM_LEFT, DIR_BOTTOM_RIGHT
};

char promotion_pieces[][4]  = {
{W_QUEEN, W_ROOK, W_BISHOP, W_KNIGHT},
{B_QUEEN, B_ROOK, B_BISHOP, B_KNIGHT}};
// all the translations for a knight's movements
const int knight_translations[][2] = {{-1,2},{-2,1},{1,2},{2,1},{-1,-2},{-2,-1},{1,-2},{2,-1}};

// places the kings can go to for castling
const int king_translations_castle[][2] = {{C1, G1},{C8, G8}};

// places where the corresponding rooks end up at
const int rook_translations_castle_fr[][2] = {{A1, H1}, {A8, H8}};
const int rook_translations_castle_to[][2] = {{D1, F1}, {D8, F8}};

// the direction vectors for the directions in DIRECTIONS
// the indeces for this array are the values in DIRECTIONS
const int direction_vectors[][2] = {{0,1},{0,-1},{-1,0},{1,0},{-1,1},{1,1},{-1,-1},{1,-1}};

// the vectors for a normal pawn move to a blank square
// the indeces for this array are 0 for WHITE and 1 for BLACK
int pawn_move_vectors   [] = { DIR_TOP, DIR_BOTTOM};

// the initial vertical coordinates for the pawns for each side
// the indeces for this array are 0 for WHITE and 1 for BLACK
int pawn_initial_ranks  [] = { 1, 6};

// the final vertical coordinates that pawns on each side can reach
// the indeces for this array are 0 for WHITE and 1 for BLACK
// at these final coordinates, we can promote the pawn
int pawn_final_ranks [] = { 7, 0 };

// the direction vectors for pawns to capture opponent pieces
// the indeces for this array are 0 for WHITE and 1 for BLACK
// and then 0 and 1 for the two directions
int pawn_capture_vectors[][2] = { {DIR_TOP_LEFT, DIR_TOP_RIGHT}, {DIR_BOTTOM_LEFT, DIR_BOTTOM_RIGHT}};

typedef uint16_t Move;
const uint16_t TO_BITS   = 0b0000000000111111;
const uint16_t FROM_BITS = 0b0000111111000000;
const uint16_t PROM_BITS = 0b1111000000000000;

enum PROMOTIONS {
PROMOTE_TO_QUEEN,
PROMOTE_TO_ROOK,
PROMOTE_TO_BISHOP,
PROMOTE_TO_KNIGHT
};

uint16_t promotion_flags[] = {0b0001, 0b0010, 0b0100, 0b1000};

Move set_promotion_bits(Move in, enum PROMOTIONS promote_to)
{
    in &= ~PROM_BITS;
    in |= (promotion_flags[promote_to] & 0b1111) << 12;
    return in;
}

int get_promotion_bits(uint32_t in)
{
    return (in >> 12) & 0b1111;
}

uint32_t set_from_bits(uint32_t in, uint32_t from)
{
    in &= ~FROM_BITS;
    in |= (from & TO_BITS) << 6;
    return in;
}

uint32_t set_to_bits(uint32_t in, uint32_t to)
{
    in &= ~TO_BITS;
    in |= (to & TO_BITS);
    return in;
}

int get_from_bits(uint32_t in)
{
    return (in >> 6) & 0b111111;
}

int get_to_bits(uint32_t in)
{
    return in & 0b111111;
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

int get_last_square_in_direction(game_state* s, int direction, int index)
{
    int ret = 0;
    int n = 1;
    while (1) {
        int position = get_square_in_direction(index, direction, n);
        n++;
        if (position == -1)
            break;
        ret = position;
        if (!(s->squares[position] == BLANK))
            break;
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

int legal_move_pawn_expand_promotions(game_state* s, int index, int dest, Move* moves,int n_moves)
{
    for (int i = PROMOTE_TO_QUEEN; i <= PROMOTE_TO_KNIGHT; i++)
    {
        moves[n_moves] = 0;
        moves[n_moves] = set_from_bits(moves[n_moves], index);
        moves[n_moves] = set_to_bits(moves[n_moves], dest);
        moves[n_moves] = set_promotion_bits(moves[n_moves], i);
        n_moves++;
    }
    return n_moves;
}

uint64_t legal_move_pawn_enpassant(game_state* s, int index)
{
    uint64_t possible_moves = 0x0;
    int position;
    for (int i = 0; i < 2; i++)
    {
        position = get_square_in_direction(index, pawn_capture_vectors[s->turn][i], 1);
        if (position == -1)
            continue;
        if (s->en_passant == position)
            return set_nth_bit_to(possible_moves, position, 1);
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

uint8_t get_castle_status_for_player(uint8_t castle_status, int player)
{
    uint8_t shift_amount = (player==WHITE) ? 0:2;
    return (castle_status >> shift_amount) & 0b11;
}

uint64_t legal_move_king_castle(game_state* s, int index)
{
    uint64_t possible_moves = 0x0;
    int player = s->turn;
    char own_rook = (s->turn == WHITE) ? W_ROOK : B_ROOK;

    // queenside castle
    if (s->squares[get_last_square_in_direction(s, DIR_LEFT, index)] == own_rook)
    {
        uint8_t castle_status_for_player = get_castle_status_for_player(s->castles_possible, player);
        if(get_nth_bit(castle_status_for_player, 0))
        {
            possible_moves = set_nth_bit_to(possible_moves, king_translations_castle[player==BLACK][0], 1);
        }
    }

    // kingside castle
    if (s->squares[get_last_square_in_direction(s, DIR_RIGHT, index)] == own_rook)
    {
        uint8_t castle_status_for_player = get_castle_status_for_player(s->castles_possible, player);
        if(get_nth_bit(castle_status_for_player, 1))
        {
            possible_moves = set_nth_bit_to(possible_moves, king_translations_castle[player==BLACK][1], 1);
        }
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

    for (int i = DIR_TOP; i <= DIR_BOTTOM_RIGHT; i++)
    {
        idx = get_square_in_direction(king_index, i, 1);
        if (idx == -1)
            continue;
        nearest_piece_in_direction = s->squares[idx];
        if (nearest_piece_in_direction == BLANK)
            continue;
        if (get_player(nearest_piece_in_direction) == player)
            continue;
        if (is_king(nearest_piece_in_direction))
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
    for (int i = DIR_TOP; i <= DIR_BOTTOM_RIGHT; i++)
    {
        idx = get_square_in_direction(king_index, i, 1);
        if (idx == -1)
            continue;
        nearest_piece_in_direction = s->squares[idx];
        if (nearest_piece_in_direction == BLANK)
            continue;
        if (get_player(nearest_piece_in_direction) == player)
            continue;
        if (is_king(nearest_piece_in_direction))
            ret |= set_nth_bit_to(ret, idx, 1);
    }
    return ret;
}

void set_flags_new_state(game_state* new)
{
    new->white_pieces = 0;
    new->black_pieces = 0;
    for (int i = 0; i < 64; i++)
    {
        if (get_player(new->squares[i]) == BLACK)
            new->black_pieces = set_nth_bit_to(new->black_pieces, i, 1);
        if (get_player(new->squares[i]) == WHITE)
            new->white_pieces = set_nth_bit_to(new->white_pieces, i, 1);
    }
}

game_state make_move_2(game_state* s, Move m)
{
    // executes a move and returns the resulting game_state

    game_state ret = *s;
    ret.turn = get_opponent(s->turn);
    int from = get_from_bits(m);
    int to = get_to_bits(m);
    int promotions = get_promotion_bits(m);

    if (is_pawn(s->squares[from]) && to == s->en_passant)
    {
        int captured_pawn_index = get_square_in_direction(to, pawn_move_vectors[get_opponent(s->turn)], 1);
        ret.squares[captured_pawn_index] = BLANK;
        ret.en_passant = -1;
    }
    if (is_pawn(s->squares[from]) && pawn_initial_ranks[get_player(s->squares[from])] == board_index_to_coord_y(from) &&
        (abs(board_index_to_coord_y(from) - board_index_to_coord_y(to)) == 2))
    {
        ret.en_passant = get_square_in_direction(from, pawn_move_vectors[s->turn], 1);
    } else {
        ret.en_passant = -1;
    }

    if (is_king(s->squares[from]))
    {
        uint8_t castles_possible = get_castle_status_for_player(s->castles_possible, s->turn);
        char own_rook = (s->turn == WHITE) ? W_ROOK : B_ROOK;
        if (to == king_translations_castle[s->turn==BLACK][0]
            && get_nth_bit(castles_possible, 0)
            && s->squares[get_last_square_in_direction(s, DIR_LEFT, from)] == own_rook)
        {
            ret.squares[rook_translations_castle_to[s->turn==BLACK][0]] = ret.squares[rook_translations_castle_fr[s->turn==BLACK][0]];
            ret.squares[rook_translations_castle_fr[s->turn==BLACK][0]] = BLANK;
        }
        if (to == king_translations_castle[s->turn==BLACK][1]
            && get_nth_bit(castles_possible, 1)
            && s->squares[get_last_square_in_direction(s, DIR_RIGHT, from)] == own_rook)
        {
            ret.squares[rook_translations_castle_to[s->turn==BLACK][1]] = ret.squares[rook_translations_castle_fr[s->turn==BLACK][1]];
            ret.squares[rook_translations_castle_fr[s->turn==BLACK][1]] = BLANK;
        }
        uint8_t reset_mask = (s->turn == WHITE) ? 0b1100 : 0b0011;
        ret.castles_possible = s->castles_possible & reset_mask;
    }
    if (is_rook(s->squares[from]))
    {
        uint8_t reset_mask;
        if (from == rook_translations_castle_fr[s->turn==BLACK][0])
        {
            reset_mask = (s->turn==WHITE) ? 0b1110 : 0b1011;
        } else if (from == rook_translations_castle_fr[s->turn==BLACK][1]) {

            reset_mask = (s->turn==WHITE) ? 0b1101 : 0b0111;
        }
        ret.castles_possible = ret.castles_possible & reset_mask;
    }
    ret.squares[to] = s->squares[from];
    ret.squares[from] = BLANK;
    if (promotions)
    {
        char what_to_promote_to = LOG2(promotions);

        what_to_promote_to = promotion_pieces[s->turn==BLACK][(int)what_to_promote_to];

        ret.squares[to] = what_to_promote_to;
    }
    set_flags_new_state(&ret);
    return ret;
}
int find_piece(game_state* s, int piece)
{

    // finds the piece `piece` in the board
    // and returns its location's index

    char player = get_player(piece);
    uint64_t search_area = (player == WHITE) ? s->white_pieces : s->black_pieces;
    int index;
    while(search_area > 0)
    {
        index = pop_next_index(&search_area);
        if (s->squares[index] == piece)
            return index;
    }
    return -1;
}

int ensure_moves_are_legal(game_state* s, Move move[], int n_moves)
{
    // make sure none of the moves cause the player's own king
    // to be in check

    int king_index = -1;

    Move legal_moves[n_moves];
    int n_legal_moves = 0;
    for (int i = 0; i < n_moves; i++)
    {
        game_state new_state = make_move_2(s, move[i]);
        int king_we_re_searching_for = (s->turn==WHITE)? W_KING: B_KING;
        king_index = find_piece(&new_state, king_we_re_searching_for);
        if (!is_king_in_check(&new_state, king_index))
        {
            legal_moves[n_legal_moves] = move[i];
            n_legal_moves++;
        }
    }

    memcpy(move, legal_moves, n_legal_moves*sizeof(Move));

    return n_legal_moves;
}

uint64_t pseudo_legal_moves(game_state *s,int index)
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
                possible_moves |= legal_move_king_castle(s, index);
                break;
            case W_QUEEN:
            case B_QUEEN:
                possible_moves =legal_move_queen(s,index);
                break;
            case W_PAWN:
            case B_PAWN:
                possible_moves =legal_move_pawn(s,index);
                possible_moves |= legal_move_pawn_enpassant(s, index);
                break;
        }
    }
    return possible_moves;
}

int get_legal_moves_from_one_square_as_move_array(game_state* s, int index, Move moves[], int counter)
{
    uint64_t legal_moves_bitfield = pseudo_legal_moves(s, index);
    int j;
    while(legal_moves_bitfield)
    {
        j = pop_next_index(&legal_moves_bitfield);
        if (is_pawn(s->squares[index]) && board_index_to_coord_y(index) == pawn_initial_ranks[get_opponent(s->turn)])
        {
            counter = legal_move_pawn_expand_promotions(s, index, j, moves, counter);
        } else
        {
            moves[counter] = 0;
            moves[counter] = set_from_bits(moves[counter], index);
            moves[counter] = set_to_bits(moves[counter], j);
            counter++;
        }
    }
    return counter;
}

int get_legal_moves_as_move_array(game_state* s, Move moves[])
{
    int counter = 0, i;
    uint64_t search_area = (s->turn == WHITE)? s->white_pieces : s->black_pieces;
    while (search_area > 0)
    {
        i = pop_next_index(&search_area);
        counter = get_legal_moves_from_one_square_as_move_array(s, i, moves, counter);
    }
    counter = ensure_moves_are_legal(s, moves, counter);
    return counter;
}

uint64_t get_legal_destinations(game_state* s, int index)
{
    Move moves[28];
    int n_moves = get_legal_moves_from_one_square_as_move_array(s, index, moves, 0);
    n_moves = ensure_moves_are_legal(s, moves, n_moves);
    uint64_t ret = 0;
    for (int i = 0; i < n_moves; i++)
    {
        int to_bits = get_to_bits(moves[i]);
        ret = set_nth_bit_to(ret, to_bits, 1);
    }
    return ret;
}

int is_check_mate(game_state* s)
{
    Move moves[256];
    int n_moves = get_legal_moves_as_move_array(s, moves);
    if (n_moves == 0)
        return 1;
    return 0;
}

int is_move_legal(uint64_t possible_moves, int to)
{
    return get_nth_bit(possible_moves, to);
}

#endif
