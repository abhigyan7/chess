#include <stdio.h>
#include <stdlib.h>
//storing strings of 1.possible letterings 2.possible numbers
char pieces[12] ="rnbqkpRNBQKP";
char numbers[8] ="12345678";
typedef char names[6];
//matrix of 12 strings containing the corresponding unicode of pieces
//matrix[0] contains unicode of black rook (i.e r in pieces string)
names matrix[12]={"\u265C","\u265E","\u265D","\u265B","\u265A","\u265F","\u2656","\u2658","\u2657","\u2655","\u2654","\u2659"};

//this function checks if a given character a is present in string b
//if present returns the corresponding index of a
//else returns 12
int check(char a,char b[]){
	int is_present =0;
    int position =0;
	for(int j=0;j<12;j++){
		if(pieces[j]==a){
			is_present = 1;
			break;
		}
        position++;
	}
	if(is_present==1){
        return position;
    }
    return 12;
}

//this function checks if a (character of number) is in b(strings of numbers)
int num_check(char a,char b[]){
    int i=1;
	for(int j=0;j<12;j++){
		if(b[j]==a){
			break;
		}
        i++;
	}
	return i;
}
//this function is for displaying the particular board
void display_board(char *pen){
    static char *i;
    i=pen;
	while(*i!='\0'){
        if(*i!='/'){
            int piece =check(*i,pieces);
            if(piece!=12){
                printf("%s ",matrix[piece]);
                i++;
            }
            else{
                int empty_squares =num_check(*i,numbers);
                while(empty_squares>0){
                    printf("_ ");
                    empty_squares--;
                }
                i++;
            }
        }
        else{
            printf("\n");
            i++;
        }
    }
}
int main(){
    char *str_of_pen;
    scanf("%s",str_of_pen);
	display_board(str_of_pen);
	return 0;
}
