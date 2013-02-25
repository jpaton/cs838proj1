CC=gcc
CFLAGS=-c -Wall -std=c99 -lpthread -D_FILE_OFFSET_BITS=64
LDFLAGS=
SOURCES=timetest.c random_read.c util.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLES=timetest random_read fetchsize

all: $(OBJECTS) $(EXECUTABLES)
	
random_read: random_read.o util.o time.o
	$(CC) $(LDFLAGS) time.o util.o random_read.o -o $@

fetchsize: fetchsize.o util.o time.o
	$(CC) $(LDFLAGS) time.o util.o fetchsize.o -o $@

timetest: timetest.o util.o
	$(CC) $(LDFLAGS) util.o timetest.o -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

copy:
	cp $(EXECUTABLES) /Volumes/DISK_IMG/

clean:
	rm *.o
	rm $(EXECUTABLES)
