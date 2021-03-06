CC=gcc
CFLAGS=-c -Wall -std=c99 -D_FILE_OFFSET_BITS=64
LDFLAGS=-pthread
SOURCES=timetest.c util.c random_read.c cache_size.c indirection.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLES=timetest fetchsize random_read cache_size indirection

all: $(OBJECTS) $(EXECUTABLES)
	
random_read: random_read.o util.o time.o
	$(CC) $(LDFLAGS) time.o util.o random_read.o -o $@

fetchsize: fetchsize.o util.o time.o
	$(CC) $(LDFLAGS) time.o util.o fetchsize.o -o $@

cache_size: cache_size.o util.o
	$(CC) $(LDFLAGS) util.o cache_size.o -o $@

indirection: indirection.o util.o
	$(CC) $(LDFLAGS) util.o indirection.o -o $@

timetest: timetest.o util.o
	$(CC) $(LDFLAGS) util.o timetest.o -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

copy:
	cp $(EXECUTABLES) /Volumes/DISK_IMG/

clean:
	rm *.o
	rm $(EXECUTABLES)
