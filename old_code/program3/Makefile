CC = gcc
CFLAGS = -fsanitize=address -Wall -Wextra -g

all: mismatch

mismatch: mismatch.c
	$(CC) $(CFLAGS) -o mismatch mismatch.c

clean:
	rm -f mismatch
