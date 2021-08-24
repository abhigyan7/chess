#include <stdio.h>
#include <stdint.h>

#include "board.h"
#include "legal_moves.h"

int firstSetBit(uint64_t number)
{
    return number & -number;
}
int test(uint64_t x)
{
    return x & ~(x-1);
}
int find_piece_1(game_state* s, int piece)
{
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
float eval_random()
{
    float x = (float)rand()/(float)(RAND_MAX/1000);
    return x;
}
int main(int argc, char *argv[])
{
    //char* test_fen = "rnbqkbnr/ppp2ppp/8/4p3/4P3/8/PPP2PPP/RNBQKBNR w KQkq - 0 4";
    //char* test_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
//    char* test_fens[] = {
//    "rnb1kbnr/pp1q1ppp/2pP4/8/8/4Q3/PPP1PPPP/RNB1KBNR b KQkq - 1 5",
//    "rnbk1bnr/pp1q1ppp/2pP4/6Q1/8/8/PPP1PPPP/RNB1KBNR b KQ - 3 6",
//    "rnbqkbnr/5ppp/p2P4/1N4Q1/P7/4R3/1PP1PPPP/2B1KBNR b K - 1 14",
//    "1nbq1bnr/r2k1ppp/3P4/p1N3Q1/P7/4R3/1PP1PPPP/2B1KBNR b K - 3 17",
//    "rnbqkbnr/ppp2ppp/8/4p3/4P3/8/PPP2PPP/RNBQKBNR w KQkq - 0 4",
//    "1nbq1bnr/r4ppp/2kP4/pP4Q1/P7/3NR3/2P1PPPP/2B1KBNR b K - 0 20",
//    "3b4/1p1n4/p2P4/P4Pp1/2P2k2/8/2B3K1/4R3 b - - 6 55",
//};
    char* test_fens[] = {"3b4/1p1n4/p2P4/P4Pp1/2P2k2/8/2B3K1/4R3 b - - 6 55"};


    int n = 1;

    game_state s;
    for (int i = 0; i < n; i++)
    {
        read_state(&s, test_fens[i]);

        //int king = (s.turn == BLACK)? B_KING : W_KING;
        int king = B_KING;
        printf("Piece: %d\n", king);
        print_board_state(&s);
        int k_idx = find_piece(&s, king);
        int result = is_king_in_check(&s, k_idx);
        uint64_t legalmoves = legal_moves(&s, k_idx);
        uint64_t checks = which_pieces_check_king(&s, k_idx);
        print_moves(legalmoves);
        printf("\n***************\n");
        print_moves(checks);
        printf("Check : %d.\n", result);
        printf("Index: %d.\n***************\n", k_idx);

    }
    for (int z = 0; z < 10; z++)
        printf("%f   ", eval_random());
}
