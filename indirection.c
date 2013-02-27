#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <time.h>
#include "util.h"
#include "rdtsc.h"

#define BLOCK_SIZE (128 * 1<<10) // ZFS default block size is apparently 128k
#define FILE_SIZE (1<<30) // in bytes
#define NUM_TRIALS (100)

void fill_buffer(char *buffer) {
    for (int i = 0; i < BLOCK_SIZE; i++) 
        buffer[i] = drand48() * 256;
}

int main(int argc, char **argv) {
    char *filename = argv[1];
    int fildes;
    unsigned long long start, end;
    char buffer[BLOCK_SIZE];
    ssize_t written;

    srand48(time(NULL));

    for (int trial = 0; trial < NUM_TRIALS; trial++) {
        EXIT_ON_FAIL((fildes = open(filename, O_WRONLY | O_CREAT | O_TRUNC)) == -1, "open");
        unsigned long long bytes_written = 0;
        while (bytes_written < FILE_SIZE) {
            fill_buffer(buffer);
            start = rdtsc();
            written = write(fildes, buffer, BLOCK_SIZE);
            fsync(fildes);
            end = rdtsc();
            if (written == -1) {
                if (errno == EIO) 
                    continue;
                else
                    EXIT_ON_FAIL(true, "write");
            } else if (written < BLOCK_SIZE) {
                fprintf(stderr, "written < BLOCK_SIZE\n");
                exit(-1);
            }
            printf("%d,%llu,%llu\n", trial, bytes_written, end - start);
            bytes_written += BLOCK_SIZE;
        }
        EXIT_ON_FAIL(close(fildes), "close");
    }
    return 0;
}
