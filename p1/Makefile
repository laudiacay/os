CCBIN=/usr/bin/gcc
CC=$(CCBIN) -Wall -Wextra -Werror -pedantic -g -fsanitize=address -fno-omit-frame-pointer
CCFAST=$(CCBIN) -Wall -Wextra -Werror -pedantic -g

default: mysh

mysh: lib/*.h mysh.c parsing.c utils.c list.c run.c builtins.c
	$(CC) -O3 -o mysh  mysh.c parsing.c utils.c list.c run.c builtins.c

myshfast: lib/*.h mysh.c parsing.c utils.c list.c run.c builtins.c
	$(CCFAST) -O3 -o mysh  mysh.c parsing.c utils.c list.c run.c builtins.c

list: lib/*.h list.c list_test.c
	$(CC) -O3 -o list  list.c list_test.c

.PHONY: clean
clean:
	rm -f *.o mysh list
