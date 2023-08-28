CC=gcc
CFLAGS=-std=c11 -Wall -Werror -Wextra -pedantic -g

foo: Exe_1.c
	$(CC) -o foo Exe_1.c $(CFLAGS)


repeatme: repeatme.c
	$(CC) -o repeatme repeatme.c $(CFLAGS)


repeatme: noAs.c
	$(CC) -o noAs noAs.c $(CFLAGS)


sum: sum.c
	$(CC) -o sum sum.c $(CFLAGS)

