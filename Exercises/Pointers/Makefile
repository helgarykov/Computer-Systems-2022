CC=gcc
CFLAGS=-std=c11 -Wall -Wextra -pedantic -g -fsanitize=address

all: hexabyte int2ascii foo

hexabyte: hexabyte.c
		gcc hexabyte.c -o hexabyte $(CFLAGS)

int2ascii: int2ascii.c
		gcc int2ascii.c -o int2ascii $(CFLAGS)

foo: foo.c
		gcc foo.c -o foo $(CFLAGS)
clean:
		rm -f hexabyte int2ascii foo