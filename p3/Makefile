CCBIN = gcc
CCFAST=$(CCBIN) -Wall -Wextra -Werror -pedantic -g -std=gnu99 -lpthread -pthread
CCSLOW=$(CCFAST) -fsanitize=address -fno-omit-frame-pointer
CC=$(CCFAST)
SERVEROBJECTS=fsops.o directory.o inode.o dblock.o bitmap.o

default: all

all: server libmfs.so

# this generates the target executables
server: server.o udp.o $(SERVEROBJECTS) libmfs.so
	$(CC) -o server server.o udp.o $(SERVEROBJECTS)

libmfs.so: mfs.h mfs.o
	$(CC) -shared -Wl,-soname,libmfs.so mfs.h mfs.o -o libmfs.so

mfs.o: mfs.h mfs.c
	$(CC) mfs.c -c -fPIC

# this is a generic rule for .o files
%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f *.o *.so* server testclient *.pyc *.fs
