# Variables
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -g -O3
CPROGRAMS = 412shell
OBJECTS = 412shell.o

# Targets and Dependencies
all: $(CPROGRAMS)

412shell: 412shell.o
	$(CC) $(CFLAGS) -o 412shell 412shell.o

412shell.o: 412shell.c
	$(CC) $(CFLAGS) -c 412shell.c

clean:
	rm -f $(OBJECTS) $(CPROGRAMS)

.PHONY: all clean
