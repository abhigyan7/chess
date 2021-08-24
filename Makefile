##
# Chess
#
# @file
# @version 0.1

main:
	gcc main.c -o main.out -lSDL2 -lSDL2_image -g -std=c11

run: main
	./main.out

# end
