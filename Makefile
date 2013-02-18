CC=gcc
CFLAGS=-c -Wall -std=c99 -lpthread
LDFLAGS=-mt
SOURCES=timetest.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLES=timetest

all: $(SOURCES) $(EXECUTABLES)
	
timetest: timetest.o
	$(CC) $(LDFLAGS) timetest.o -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
	rm $(EXECUTABLES)
