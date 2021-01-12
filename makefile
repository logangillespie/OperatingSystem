CC=gcc 
CFLAGS=-I. -ansi -pedantic -Wall -std=c99

.PHONY: compile clean run
compile: parser.x

parser.x: parser.o
	$(CC) $(CFLAGS) -o parser.x  parser.o

parser.o: parser.c
	$(CC) $(CFLAGS) -o parser.o -c parser.c
	
Clean: 
	rm -f*.o* .x
run: compile 
	./parser.x
