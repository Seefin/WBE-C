all: main.o browser.o
	gcc main.o browser.o -o browser
main.o: 
	gcc -std=c99 -Wall -o main.o -c main.c
browser.o:
	gcc -std=c99 -Wall -o browser.o -c browser.c
clean:
	rm *.o *.h~ *.c~ browser
