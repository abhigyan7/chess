#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "moves_generation.h"
int main()
{
    game_state state_1;
    int board[64];
    state_1.squares = board;

    printf("\n---------------\n");
    read_state(&state_1, test_fenstring_1);
    print_board(&state_1);

    printf("\n---------------\n");
    read_state(&state_1, test_fenstring_2);
    print_board(&state_1);

    printf("\n---------------\n");
    read_state(&state_1, test_fenstring_3);
    print_board(&state_1);
    printf("\n");
    move("a2","a4");
    move("b2","b7");
    
    return 0;

}
