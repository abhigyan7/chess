#ifndef _LEGAL_MOVES_H
#define _LEGAL_MOVES_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "board.h"
// int count(uint64_t number){
//     int count=1;
//     return count;
// }
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

int get_square_in_direction(int start_cell, int direction, int n){
    int start_cell_x = board_index_to_coord_x(start_cell);
    int start_cell_y = board_index_to_coord_y(start_cell);

    int dest_cell_x = start_cell_x+ n*vecs_for_directions[direction][0];
    int dest_cell_y = start_cell_y+n*vecs_for_directions[direction][1];

    if (dest_cell_x > 7 || dest_cell_x < 0 || dest_cell_y < 0 || dest_cell_y > 7)
        return -1;

    return coord_xy_to_board_index(dest_cell_x, dest_cell_y);
}

int get_knightsquare_in_direction(int start_cell, int direction){
    int start_cell_x = board_index_to_coord_x(start_cell);
    int start_cell_y = board_index_to_coord_y(start_cell);

    int dest_cell_x = start_cell_x+ vecs_for_knight[direction][0];
    int dest_cell_y = start_cell_y+vecs_for_knight[direction][1];

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

//checks linear and diagonal on the basis of direction
//blank spaces and  returns maximum possible square
int max_moves_sliding(game_state *s,int index,int direction){
    int max=1;
    int new_position=get_square_in_direction(index,direction,max);
    //if black's turn
    if(s->turn){
        while(new_position!=-1){
            if(s->squares[new_position]&8==0 && s->squares[new_position]!=BLANK){
                break;
            }
            else if(s->squares[new_position]==BLANK){
                new_position=get_square_in_direction(index,direction,max);
                if(new_position!=-1){
                    max++;
                }
                continue;
            }
            else{
                max--;
                break;
            }
        }
        return max-1;
    }else{
        while(new_position!=-1){
            if(s->squares[new_position]&8==8){
                break;
            }
            else if(s->squares[new_position]==BLANK){
                new_position=get_square_in_direction(index,direction,max);
                if(new_position!=-1){
                    max++;
                }
                
                continue;
            }
            else{
                max--;
                break;
            }  
        }
    }
    return max-1;
}
//DIR_TOP_LEFT, DIR_TOP_RIGHT, DIR_BOTTOM_LEFT, DIR_BOTTOM_RIGHT
uint64_t legal_move_knight(game_state *s,int index){
    uint64_t possible_moves=0x0;
    int position;
    int index_x = board_index_to_coord_x(index);
    int index_y = board_index_to_coord_y(index);
    if(s->turn){
        for(int i=0;i<8;i++){
            position= get_knightsquare_in_direction(index,i);
            if(position!=-1){
                possible_moves =(s->squares[position]&8==8 ||s->squares[position]==BLANK)? set_nth_bit_to(possible_moves, position,1):possible_moves;
            }
        }
    }
    else{
        for(int i=0;i<8;i++){
            position= get_knightsquare_in_direction(index,i);
            if(position!=-1){
                possible_moves =(s->squares[position]&8==8)? possible_moves:set_nth_bit_to(possible_moves, position,1);
            }
        }
        
    }
    return possible_moves;
}



uint64_t legal_move_pawn(game_state *s,int index){
    int *pos1= (int *)malloc(sizeof(int));
    int *pos2= (int *)malloc(sizeof(int));
    uint64_t possible_moves=0x0;
    int position;
    if(s->turn){
        //black's one step down
        position = get_square_in_direction(index,DIR_BOTTOM,1);
        if(s->squares[position]==BLANK){
            possible_moves= set_nth_bit_to(possible_moves, position,1);
             //black 's first two steps down
            if(index>=8 &&index<=15){
                if(s->squares[index+16]==BLANK){
                    possible_moves= set_nth_bit_to(possible_moves, index+16,1);
                    
                }
            }
        }
        //black's bottom left and bottom right
        *pos1 = get_square_in_direction(index,DIR_BOTTOM_LEFT,1);
        possible_moves = ((s->squares[*pos1])&8 ==8 ||s->squares[*pos1]==BLANK)? possible_moves:set_nth_bit_to(possible_moves, *pos1,1);
        *pos2 = get_square_in_direction(index,DIR_BOTTOM_RIGHT,1);
        possible_moves = ((s->squares[*pos2])&8 ==8 ||s->squares[*pos2]==BLANK)? possible_moves:set_nth_bit_to(possible_moves, *pos2,1);
    }
    else{
        //white's one step up
        position = get_square_in_direction(index,DIR_TOP,1);
        if(s->squares[position]==BLANK){
            possible_moves= set_nth_bit_to(possible_moves, position,1);
             //white's first two steps up
            if(index>=48 &&index<=55){
                if(s->squares[index-16]==BLANK){
                    possible_moves= set_nth_bit_to(possible_moves,index-16,1);
                }
                
            }
        }
        //white's top left and top right
        *pos1 = get_square_in_direction(index,DIR_TOP_LEFT,1);
        possible_moves =((s->squares[*pos1]&8) < 8) ? possible_moves:set_nth_bit_to(possible_moves,*pos1,1);
        *pos2 = get_square_in_direction(index,DIR_TOP_RIGHT,1);
        possible_moves =((s->squares[*pos2]&8) < 8) ? possible_moves:set_nth_bit_to(possible_moves,*pos2,1);
    }
    free(pos1);
    free(pos2);    
    return possible_moves;
}

uint64_t legal_move_king(game_state *s,int index){
    uint64_t possible_moves=0x0;
    int position;
    for(int dir =0;dir<8;dir++){
        position =get_square_in_direction(index,dir,1);
        possible_moves = ((position!=-1)&&(s->squares[position]==BLANK))? set_nth_bit_to(possible_moves, position,1):possible_moves;
    }
    return possible_moves;
}

//find opposite color queen or bishop or rook in the range
//the color is of king that is to be checked of checks from sliding pieces
uint64_t king_check_sliding(game_state *s,int index){
    int position;
    uint64_t possible_checks=0x0; 
    int max;
    if(s->turn){
        //checking top bottom left right
        for(int dir=0;dir<4;dir++){
            max=1;
            position =get_square_in_direction(index,dir,max);
            while(position!=-1 && s->squares[position] ==BLANK){
                max++;
                position =get_square_in_direction(index,dir,max);
                if(position!=-1 &&(s->squares[position]==W_QUEEN ||s->squares[position]==W_ROOK)){
                    possible_checks = set_nth_bit_to(possible_checks, position,1);
                    break;
                }
            }
            
        }
        //checking DIR_TOP_LEFT, DIR_TOP_RIGHT, DIR_BOTTOM_LEFT, DIR_BOTTOM_RIGHT
        for(int dir=4;dir<8;dir++){
            max=1;
            position =get_square_in_direction(index,dir,max);
            while(position!=-1 && s->squares[position] ==BLANK){
                max++;
                position =get_square_in_direction(index,dir,max);
                if(position!=-1 &&(s->squares[position]==W_QUEEN ||s->squares[position]==W_BISHOP)){
                    possible_checks = set_nth_bit_to(possible_checks, position,1);
                    break;
                }
            }
            
        }
    }
    else{//checking for white king
        //checking top bottom left right
        for(int dir=0;dir<4;dir++){
            max=1;
            position =get_square_in_direction(index,dir,max);
            while(position!=-1 && s->squares[position] ==BLANK){
                max++;
                position =get_square_in_direction(index,dir,max);
                if(position!=-1 &&(s->squares[position]==B_QUEEN ||s->squares[position]==B_ROOK)){
                    possible_checks = set_nth_bit_to(possible_checks, position,1);
                    break;
                }
            }
            
        }
        //checking DIR_TOP_LEFT, DIR_TOP_RIGHT, DIR_BOTTOM_LEFT, DIR_BOTTOM_RIGHT
        for(int dir=4;dir<8;dir++){
            max=1;
            position =get_square_in_direction(index,dir,max);
            while(position!=-1 && s->squares[position] ==BLANK){
                max++;
                position =get_square_in_direction(index,dir,max);
                if(position!=-1 &&(s->squares[position]==B_QUEEN ||s->squares[position]==B_BISHOP)){
                    possible_checks = set_nth_bit_to(possible_checks, position,1);
                    break;
                }
            }
            
        }
    }
    
    return possible_checks;
}

uint64_t king_in_check(game_state *s,int king_index){
    int position;
    uint64_t flag=0x0;
    uint64_t flag_sliding=0x0;
    //FIRST CHECKING FOR BLACK KING
    if(s->turn){
        //check l-SHAPE FOR KNIGHT
        for(int i=0;i<8;i++){
            position = get_knightsquare_in_direction(king_index,i);
            if(position!=-1){
                flag = (s->squares[position]==W_KNIGHT)?set_nth_bit_to(flag, position,1):flag;
            }
        }
        //check 1 step left bottom and right bottom diagnols for white pawns
        for(int i=6;i<=7;i++){
            position =get_square_in_direction(king_index,i,1);
            if(position!=-1){
                flag = (s->squares[position]==W_PAWN)? set_nth_bit_to(flag, position,1):flag;
            }
        }
        flag_sliding =king_check_sliding(s,king_index);
        flag =flag|flag_sliding;
    }
    else{
        //check l
        for(int i=0;i<8;i++){
            position = get_knightsquare_in_direction(king_index,i);
            if(position!=-1){
                flag = (s->squares[position]==B_KNIGHT)?set_nth_bit_to(flag, position,1):flag;
            }
        }
        //check 1 step left top and right top diagnols for white pawns
        for(int i=4;i<=5;i++){
            position =get_square_in_direction(king_index,i,1);
            if(position!=-1){
                flag = (s->squares[position]==B_PAWN)? set_nth_bit_to(flag, position,1):flag;
            }
        }
        flag_sliding =king_check_sliding(s,king_index);
        flag =flag|flag_sliding;
    }
    return flag;
}

uint64_t legal_moves(game_state *s,int index){
    uint64_t possible_moves =0x0;
    if(s->squares[index]!=BLANK){
        switch(s->squares[index]){
            case W_ROOK:
                possible_moves =legal_move_rook(s,index);
                break;
            case W_KNIGHT:
                possible_moves =legal_move_knight(s,index);
                break;
            case W_BISHOP:
                possible_moves =legal_move_bishop(s,index);
                break;
            case W_KING:
                possible_moves =legal_move_king(s,index);
                break;
            case W_QUEEN:
                possible_moves =legal_move_queen(s,index);
                break;
            case W_PAWN:
                possible_moves =legal_move_pawn(s,index);
                break;
            case B_ROOK:
                possible_moves =legal_move_rook(s,index);
                break;
            case B_KNIGHT:
                possible_moves =legal_move_knight(s,index);
                break;
            case B_BISHOP:
                possible_moves =legal_move_bishop(s,index);
                break;
            case B_KING:
                possible_moves =legal_move_king(s,index);
                break;
            case B_QUEEN:
                possible_moves =legal_move_queen(s,index);
                break;
            case B_PAWN:
                possible_moves =legal_move_pawn(s,index);
                break;
        }
    }
    return possible_moves;
}




#endif
