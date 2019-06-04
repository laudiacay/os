CCBIN = gcc
CCFAST=$(CCBIN) -Wall -Wextra -Werror -pedantic -g -std=gnu99 -lpthread -pthread
CCSLOW=$(CCFAST) -fsanitize=address -fno-omit-frame-pointer
CC=$(CCFAST)

default: all

all: server libmfs.so

# this generates the target executables
server: server.o udp.o
	$(CC) -o server server.o udp.o

#client: client.o udp.o
#	$(CC) -o client client.o udp.o

libmfs.so: mfs.h mfs.o
	$(CC) -shared -Wl,-soname,libmfs.so mfs.h mfs.o -o libmfs.so

mfs.o: mfs.h mfs.c
	$(CCBIN) mfs.c -c -fPIC

# this is a generic rule for .o files
%.o: %.c
	$(CCBIN) -c $< -o $@

clean:
	rm -f *.o *.so* server
