all: main.o
	gcc main.o -o browser
main.o: 
	gcc -std=c99 -Wall -o main.o -c main.c
