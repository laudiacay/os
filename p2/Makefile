CCBIN=/usr/bin/gcc
CCFAST=$(CCBIN) -Wall -Wextra -Werror -pedantic -g -std=gnu99 -lpthread -pthread
CCSLOW=$(CCFAST) -fsanitize=address -fno-omit-frame-pointer
CC=$(CCFAST)
SPIN=spin
COUNTER=counter
LIST=list
HASH=hash

# got help from http://codechunks.org/blog/2012/10/24/a-makefile-for-the-simplest-shared-library/

default: lib$(SPIN) lib$(COUNTER) lib$(LIST) lib$(HASH)

lib$(SPIN): lib$(SPIN).so

lib$(SPIN).so: $(SPIN).h $(SPIN).o
	$(CC) -shared -Wl,-soname,lib$(SPIN).so $(SPIN).h $(SPIN).o -o lib$(SPIN).so

$(SPIN).o: $(SPIN).h $(SPIN).c
	$(CCBIN) $(SPIN).c -c -fPIC

lib$(COUNTER): lib$(COUNTER).so

lib$(COUNTER).so: $(COUNTER).h $(COUNTER).o
	$(CC) -shared -Wl,-soname,lib$(COUNTER).so $(SPIN).h $(COUNTER).h $(COUNTER).o -o lib$(COUNTER).so -L -l:lib$(SPIN).so

$(COUNTER).o: $(COUNTER).h $(COUNTER).c
	$(CCBIN) $(COUNTER).c -c -fPIC

lib$(LIST): lib$(LIST).so

lib$(LIST).so: lib$(SPIN).so $(LIST).h $(LIST).o
	$(CC) -shared -Wl,-soname,lib$(LIST).so $(SPIN).h $(LIST).h $(LIST).o -o lib$(LIST).so -L -l:lib$(SPIN).so

$(LIST).o: $(LIST).h $(LIST).c
	$(CCBIN) $(LIST).c -c -fPIC

lib$(HASH): lib$(HASH).so

lib$(HASH).so: lib$(SPIN).so $(HASH).h $(HASH).o
	$(CC) -shared -Wl,-soname,lib$(HASH).so $(SPIN).h $(LIST).h $(HASH).h $(HASH).o -o lib$(HASH).so

$(HASH).o: $(HASH).h $(HASH).c
	$(CCBIN) $(HASH).c -c -fPIC

clean:
	$(RM) $(SPIN) $(COUNTER) $(LIST) $(HASH) myprog *.o *.so*
