CCBIN=/usr/bin/gcc
CC=$(CCBIN) -Wall -Wextra -pedantic -g -fsanitize=address

default: mysh

mysh: lib/*.h mysh.c parsing.c utils.c list.c
	$(CC) -O3 -o mysh  mysh.c parsing.c utils.c list.c

list: lib/*.h list.c list_test.c
	$(CC) -O3 -o list  list.c list_test.c

.PHONY: clean
clean:
	rm -f *.o mysh
