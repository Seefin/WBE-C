CFLAGS = -std=gnu18 -Wall -g -c
CC=gcc
LDFLAGS = -L/usr/local/ssl/lib
LDLIBS= -lssl -lcrypto

all: main.o browser.o
	gcc main.o browser.o -o browser
ssl: ssl_client.c
	$(CC) $(CFLAGS) ssl_client.c
main.o: main.c main.h
	gcc $(CFLAGS) -o main.o main.c
browser.o: browser.c browser.h
	gcc $(CFLAGS) -o browser.o browser.c
clean:
	$(RM) *.o .*~ *~ .*.*un* browser test
test: tests/test.sh main.o browser.o
	tests/test.sh
testfile: test.c browser.h
	gcc $(CFLAGS) -o test.o test.c
	gcc test.o -o test
tree: browser.o main.o
	$(CC) -fdump-tree-graph main.o browser.o
