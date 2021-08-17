#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "board.h"
#include "moves_generation.h"
#include "legal_moves.h"
int main(){
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

    print_board_state(&starting_state);
    long int count=0;
    for(int i=48;i<64;i++){
        uint64_t legal= legal_moves(&starting_state,i);
        for(int j=0;j<64;j++){
            if(get_nth_bit(legal, j)==1){
            printf("%d ",j);
            count++;
            }
        }
    }

    printf("\nCount: %ld",count);
    return 0;

}
