#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <time.h>
#include "rdtsc.h"
#include "util.h"

#define BLOCK_SIZE (512)
#define BUF_SIZE (512)
#define MAX_INITIAL_READ_SIZE (1<<14)
#define MIN_INITIAL_READ_SIZE BLOCK_SIZE
#define MAX_PREFETCH_HYPOTHESIS (1024)  // in blocks
#define MIN_PREFETCH_HYPOTHESIS (1)
#define NUM_TRIALS 10

void print_usage(char *cmd) {
    fprintf(stderr, "Usage: %s <big file for testing> <setup file 1> [<setup file 2> ...]\n", cmd);
}

int main(int argc, char **argv) {
    char **setup_filenames, *test_filename, *buffer;
    struct stat f_stat;
    int fildes;
    unsigned long cpu_freq;
    size_t file_size;
    unsigned long long start, end;
    double time;

    if (argc < 3) {
        print_usage(argv[0]);
        return -1;
    }

    setup_filenames = &argv[2];
    test_filename = argv[1];

    EXIT_ON_FAIL(stat(test_filename, &f_stat), "stat");
    file_size = f_stat.st_size;

    cpu_freq = get_clock_frequency() * MHZ_TO_HZ;

    for (int trial = 0; trial < NUM_TRIALS; trial++) {
        for (
                int initial_read_size = MIN_INITIAL_READ_SIZE; 
                initial_read_size <= MAX_INITIAL_READ_SIZE; 
                initial_read_size += BLOCK_SIZE
        ) {
            setup_system(argc - 2, setup_filenames);
            for (
                int prefetch_hypothesis = MIN_PREFETCH_HYPOTHESIS;
                prefetch_hypothesis <= MAX_PREFETCH_HYPOTHESIS;
                prefetch_hypothesis++
            ) {
                buffer = malloc(MAX(initial_read_size, prefetch_hypothesis));
                EXIT_ON_FAIL((fildes = open(test_filename, O_RDONLY)) == -1, "open");
                EXIT_ON_FAIL(read(fildes, buffer, initial_read_size) < 0, "read");
                start = rdtsc();
                int bytes_read = pread(fildes, buffer, BLOCK_SIZE, prefetch_hypothesis);
                end = rdtsc();
                EXIT_ON_FAIL(bytes_read == -1, "pread");
                time = ((double)(end - start)) / cpu_freq;
                printf("%d,%d,%d,%f", trial, initial_read_size, prefetch_hypothesis, time);
                printf("\n");
                free(buffer);
                EXIT_ON_FAIL(close(fildes), "close");
            }
        }
    }
}
