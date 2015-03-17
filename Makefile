CC = cc
CFLAGS=-c -Wall

all: main

main: one.o
	$(CC) one.o -o one

one.o: one.c
	$(CC) $(CFLAGS) one.c

clean:
	rm *o one