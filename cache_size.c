#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "util.h"
#include "rdtsc.h"

#define BUF_SIZE (1024*1024) // use 1mb buffer
#define MIN_CACHE_SIZE (1024*1024)
#define MAX_CACHE_SIZE (16ll * 1<<30) // that is, all of RAM
//#define MAX_CACHE_SIZE (1 * 1<<30)
#define CACHE_SIZE_STEP (64 * 1024 * 1024) // 64 MB
#define NUM_TRIALS (10)

double drand48(void);
void srand48(long);

void get_into_cache(int fildes, unsigned long long size) {
    char buffer[BUF_SIZE];
    unsigned long long read_so_far = 0; // how many bytes have been read
    while (read_so_far < size) {
        int to_read = MIN(BUF_SIZE, size - read_so_far);
        int bytes_read = read(fildes, buffer, to_read);
        if (bytes_read == -1) {
            if (errno == EIO) 
                continue;
            else
                EXIT_ON_FAIL(true, "read");
        }
        read_so_far += bytes_read;
    }
}

int main(int argc, char **argv) {
    char *filename = argv[1];
    int fildes;
    unsigned long long start, end;

    srand48(time(NULL));

    EXIT_ON_FAIL((fildes = open(filename, O_RDONLY)) == -1, "open");
    for (unsigned long long cache_size_hypo = MIN_CACHE_SIZE;
            cache_size_hypo <= MAX_CACHE_SIZE;
            cache_size_hypo += CACHE_SIZE_STEP) {
        fprintf(stderr, "testing %llu\n", cache_size_hypo);
        lseek(fildes, 0, SEEK_SET);
        get_into_cache(fildes, cache_size_hypo);
        for (int trial = 0; trial < NUM_TRIALS; trial++) {
            lseek(fildes, 0, SEEK_SET);
            start = rdtsc();
            get_into_cache(fildes, cache_size_hypo);
            end = rdtsc();
            printf("%d,%llu,%llu\n", trial, cache_size_hypo, end - start);
            fflush(stdout);
        }
    }
    close(fildes);
}
