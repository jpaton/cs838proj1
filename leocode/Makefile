CC=gcc
CFLAGS=-c -Wall -D_FILE_OFFSET_BITS=64
LDFLAGS=-pthread
SOURCES=util.c prefetch.c idealbuffersize.c filecachesize.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLES=prefetch idealbuffersize filecachesize

all: $(OBJECTS) $(EXECUTABLES)

dirs:
	mkdir -p $(EXECUTABLES)
	
idealbuffersize: util.o idealbuffersize.o dirs
	$(CC) $(LDFLAGS) util.o idealbuffersize.o -o $@/$@

filecachesize: util.o filecachesize.o dirs
	$(CC) $(LDFLAGS) util.o filecachesize.o -o $@/$@

prefetch: util.o prefetch.o dirs
	$(CC) $(LDFLAGS) util.o prefetch.o -o $@/$@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
	rm $(EXECUTABLES)
