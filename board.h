#ifndef BOARD_H_
#define BOARD_H_

#include <stdio.h>

typedef struct game_state game_state;
struct game_state
{
    /*
     * game_state stores all the required state variables that define
     * the state of the game at any point in time
     *
     * It stores:
     *   1. squares
     *        a pointer to an array of 64 ints, each integer one of the
     *        values from the enum PIECES
     *   2. turn
     *        an int, one of the values from the enum TURNS
     *
     * Things we might store in the future
     *   1. Check status
     *        Which kings are in check, which pieces check the opponent's king
     *   2. Castle status
     *        Which directions are available for castling for both players
     *   3. en passant status etc
     */
    int* squares;
    int turn;
};

enum PIECES {
    W_ROOK, W_KNIGHT, W_BISHOP, W_KING, W_QUEEN, W_PAWN,
    B_ROOK, B_KNIGHT, B_BISHOP, B_KING, B_QUEEN, B_PAWN,
    BLANK
};

enum TURNS {WHITE, BLACK};

int board_starting_config[] = {
    B_ROOK, B_KNIGHT, B_BISHOP, B_QUEEN, B_KING, B_BISHOP, B_KNIGHT, B_ROOK,
    B_PAWN, B_PAWN,   B_PAWN,   B_PAWN,  B_PAWN, B_PAWN,   B_PAWN,   B_PAWN,
    BLANK,  BLANK,    BLANK,    BLANK,   BLANK,  BLANK,    BLANK,    BLANK,
    BLANK,  BLANK,    BLANK,    BLANK,   BLANK,  BLANK,    BLANK,    BLANK,
    BLANK,  BLANK,    BLANK,    BLANK,   BLANK,  BLANK,    BLANK,    BLANK,
    BLANK,  BLANK,    BLANK,    BLANK,   BLANK,  BLANK,    BLANK,    BLANK,
    W_PAWN, W_PAWN,   W_PAWN,   W_PAWN,  W_PAWN, W_PAWN,   W_PAWN,   W_PAWN,
    W_ROOK, W_KNIGHT, W_BISHOP, W_QUEEN, W_KING, W_BISHOP, W_KNIGHT, W_ROOK
};

// use this when you need the starting state
game_state starting_state = {board_starting_config, WHITE};


enum PLACES {
/*
 * This enum enables us to do something like this
 *
 *   if (game_state.squares[E8] == B_KING) <Black's king is at home>
 *
 * very idiomatic, not sure how useful it might be in practice though
 */
    A8, B8, C8, D8, E8, F8, G8, H8,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A1, B1, C1, D1, E1, F1, G1, H1,
};

/*
 * Some complete FEN strings for testing
 */
char* test_fenstring_1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
char* test_fenstring_2 = "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2";
char* test_fenstring_3 = "r3kbnr/pp1Nppp1/n1p5/1B5p/3Pp3/2P5/PP3PPP/RNB1K2R w KQkq - 0 10";

/*
 * Universal code points for chess pieces
 * These should work on most platforms irrespective
 * of how they handle unicode
 * The indeces for this array match with the values from
 * the pieces enum, so you can do
 *   printf("chars_for_pieces[B_KING]")
 * to print a black king
 */
const char* chars_for_pieces[] = {
    "\u2656", // white rook
    "\u2658", // white knight
    "\u2657", // white bishop
    "\u2654", // white king
    "\u2655", // white queen
    "\u2659", // white pawn
    "\u265C", // black rook
    "\u265E", // black knight
    "\u265D", // black bishop
    "\u265A", // black king
    "\u265B", // black queen
    "\u265F", // black pawn
    "\u2000"  // a blank space
};

void print_board(const game_state* state)
/*
 * Print just the board configuration, no bells and whistles
 * like borders, turns and captured pieces
 */
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            printf("%s ", chars_for_pieces[state->squares[i*8+j]]);
        }
        printf("\n");
    }
}

int read_state(game_state* state, char* fen_string)
{
    /*
     * Read the game state from a FEN string
     * assuming the string is valid and well-formed
     *
     * returns 1 if the board was parsed successfully
     *        -1 if the parse failed because of an unknown piece type
     */
    int board_index = 0; // which square is being filled on the board
    char token; // which character in the FEN string is being processed
    int string_index = 0; // where in the FEN string we are

    int blank_spaces;
    int piece_type;

    for(;board_index < 64;string_index++)
    {
        token = fen_string[string_index];
        if (token <= '8' && token >= '1')
            {
            // blank spaces
            blank_spaces = token - 48;
            for (int j = 0; j < blank_spaces; j++)
            {
                state->squares[board_index] = BLANK;
                board_index ++;
            }
        } else if (token >= 65 && token <= 122)
        {
            // chess piece
            switch(token)
            {
                case 'r': { piece_type = B_ROOK; break; }
                case 'n': { piece_type = B_KNIGHT; break; }
                case 'b': { piece_type = B_BISHOP; break; }
                case 'k': { piece_type = B_KING; break; }
                case 'q': { piece_type = B_QUEEN; break; }
                case 'p': { piece_type = B_PAWN; break; }
                case 'R': { piece_type = W_ROOK; break; }
                case 'N': { piece_type = W_KNIGHT; break; }
                case 'B': { piece_type = W_BISHOP; break; }
                case 'K': { piece_type = W_KING; break; }
                case 'Q': { piece_type = W_QUEEN; break; }
                case 'P': { piece_type = W_PAWN; break; }
                default: { return -1; }
            }
        state->squares[board_index] = piece_type;
        board_index ++;
        }
    }
    return 1;
}

#endif // BOARD_H_
