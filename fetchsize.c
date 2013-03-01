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
#define MAX_PREFETCH_HYPOTHESIS (1024 * 2)  // in blocks
#define MIN_PREFETCH_HYPOTHESIS (1)
#define NUM_TRIALS (1000)
#define INITIAL_READ_SIZE (512)

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

    buffer = malloc(INITIAL_READ_SIZE);
    for (int trial = 0; trial < NUM_TRIALS; trial++) {
        setup_system(argc - 2, setup_filenames);
        EXIT_ON_FAIL((fildes = open(test_filename, O_RDONLY)) == -1, "open");
        EXIT_ON_FAIL(read(fildes, buffer, INITIAL_READ_SIZE) < 0, "read");
        for (
            int prefetch_hypothesis = MIN_PREFETCH_HYPOTHESIS;
            prefetch_hypothesis <= MAX_PREFETCH_HYPOTHESIS;
            prefetch_hypothesis++
        ) {
            start = rdtsc();
            int bytes_read = pread(fildes, buffer, 1, prefetch_hypothesis * BLOCK_SIZE);
            end = rdtsc();
            EXIT_ON_FAIL(bytes_read == -1, "pread");
            time = ((double)(end - start)) / cpu_freq;
            printf("%d,%d,%d,%f", trial, INITIAL_READ_SIZE, prefetch_hypothesis, time);
            printf("\n");
            fflush(stdout);
        }
        EXIT_ON_FAIL(close(fildes), "close");
    }
    free(buffer);
}
