CFLAGS = -std=gnu18 -Wall -g -c

all: main.o browser.o
	gcc main.o browser.o -o browser
main.o: main.c main.h
	gcc $(CFLAGS) -o main.o main.c
browser.o: browser.c browser.h
	gcc $(CFLAGS) -o browser.o browser.c
clean:
	rm *.o .*~ *~ .*.*un* browser
