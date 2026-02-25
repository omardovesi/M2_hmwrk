CC = gcc
CFLAGS = -fsanitize=address -Wall -Wextra -g

all: unique

unique: unique.c
	$(CC) $(CFLAGS) -o unique unique.c

clean:
	rm -f unique
