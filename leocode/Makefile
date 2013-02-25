CC=gcc
CFLAGS=-c -Wall -std=c99 -D_FILE_OFFSET_BITS=64 -g
LDFLAGS=-pthread
SOURCES=util.c prefetch.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLES=prefetch

all: $(OBJECTS) $(EXECUTABLES)
	
random_read: random_read.o util.o time.o
	$(CC) $(LDFLAGS) time.o util.o random_read.o -o $@

fetchsize: fetchsize.o util.o time.o
	$(CC) $(LDFLAGS) time.o util.o fetchsize.o -o $@

timetest: timetest.o util.o
	$(CC) $(LDFLAGS) util.o timetest.o -o $@

prefetch: util.o prefetch.o
	$(CC) $(LDFLAGS) util.o prefetch.o -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
	rm $(EXECUTABLES)
