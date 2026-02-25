CC = gcc
CFLAGS = -fsanitize=address -Wall -Wextra -g

all: sort

sort: sort.c
	$(CC) $(CFLAGS) -o sort sort.c

clean:
	rm -f sort
