CC=gcc
CFLAGS=-c -Wall -std=c99 -lpthread
LDFLAGS=-mt
SOURCES=timetest.c random_read.c util.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLES=timetest random_read

all: $(OBJECTS) $(EXECUTABLES)
	
random_read: random_read.o util.o time.o
	$(CC) $(LDFLAGS) time.o util.o random_read.o -o $@

timetest: timetest.o util.o
	$(CC) $(LDFLAGS) util.o timetest.o -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
	rm $(EXECUTABLES)
