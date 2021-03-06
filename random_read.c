/**
 * random_read.c
 *
 * Seeks to answer the question: what is the ideal buffer size for random file reads? Does this by using reads of varying sizes, timing how long each read takes.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>
#include <time.h>
#include "util.h"
#include "rdtsc.h"

#define READ_SIZE_STEP (128 * 1024)
#define MIN_READ_SIZE READ_SIZE_STEP
#define MAX_READ_SIZE (256 * 128 * 1024)
//#define MAX_READ_SIZE 1024
#define NUM_TRIALS 1000
#define FETCH_SIZE (128 * 1024)

double drand48(void);
void srand48(long);

struct offset_tracker {
    unsigned long long offsets[NUM_TRIALS];
    int len_offsets;
};

void print_usage(char *cmd) {
    fprintf(stderr, "Usage: %s <big file for testing> <setup file 1> [<setup file 2> ...]\n", cmd);
}

void init_offset_tracker(struct offset_tracker *tracker) {
    tracker->len_offsets = 0;
}

bool already_used(struct offset_tracker *tracker, unsigned long long offset, int read_size) {
    for (int i = 0; i < tracker->len_offsets; i++) {
        if (abs(tracker->offsets[i] - offset) <= MAX(read_size, FETCH_SIZE))
            return true;
    }
    return false;
}

void register_offset(struct offset_tracker *tracker, unsigned long offset) {
    tracker->offsets[tracker->len_offsets++] = offset;
}

unsigned long long random_offset(size_t file_size, int buf_size, struct offset_tracker *tracker, int read_size) {
    unsigned long long offset;
    while(already_used(tracker, (offset = drand48() * ((unsigned long long) file_size - read_size - 1)), read_size));
    register_offset(tracker, offset);
    return offset;
}

int main(int argc, char **argv) {
    char **setup_filenames, *test_filename, *buffer;
    struct stat f_stat;
    int fildes; 
    ssize_t bytes_read;
    unsigned long long file_size;
    unsigned long long start, end;
    struct offset_tracker tracker;

    if (argc < 3) {
        print_usage(argv[0]);
        return 0;
    }

    srand48(time(NULL));

    setup_filenames = &argv[2];
    test_filename = argv[1];

    EXIT_ON_FAIL(stat(test_filename, &f_stat), "stat");
    file_size = f_stat.st_size;
    fprintf(stderr, "file size: %llu\n", file_size);

    for (
            int read_size = MIN_READ_SIZE; 
            read_size <= MAX_READ_SIZE; 
            read_size += READ_SIZE_STEP
    ) {
        buffer = malloc(read_size);
        //setup_system(argc - 2, setup_filenames); // put system into known state
        fprintf(stderr, "system set up\n");
        init_offset_tracker(&tracker);
        EXIT_ON_FAIL((fildes = open(test_filename, O_RDONLY)) == -1, "open");
        for (int trial = 0; trial < NUM_TRIALS; trial++) {
            unsigned long long offset = random_offset(file_size, read_size, &tracker, read_size);
            start = rdtsc();
            bytes_read = pread(fildes, buffer, read_size, offset);
            end = rdtsc();
            EXIT_ON_FAIL(bytes_read == -1 && errno != EIO, "pread");
            if (bytes_read != read_size) {
                trial--;
                continue;
            }
            printf("%d,%d,%llu,%llu\n", trial, read_size, offset, end - start);
            fflush(stdout);
        }
        EXIT_ON_FAIL(close(fildes), "close");
        free(buffer);
    }

    return 0;
}
