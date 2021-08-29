##
# Chess
#
# @file
# @version 0.1

main:
	gcc main.c -o main.out -lSDL2 -lSDL2_image -g -std=c11

mainoptim:
	gcc main.c -o main.out -lSDL2 -lSDL2_image -O3

runop: mainoptim
	./main.out

run: main
	./main.out

# end
