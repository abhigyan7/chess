##
# Chess
#
# @file
# @version 0.1

main:
	gcc main.c -o main.out -lSDL2 -lSDL2_image -lm -g -std=c11

mainoptim:
	gcc main.c -o main.out -lSDL2 -lSDL2_image -lm -O3

runop: mainoptim
	./main.out

run: main
	./main.out

# end
