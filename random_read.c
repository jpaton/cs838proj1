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
#include "util.h"

#define READ_SIZE_STEP 1024
#define MIN_READ_SIZE READ_SIZE_STEP
//#define MAX_READ_SIZE 1<<20
#define MAX_READ_SIZE 1024
#define NUM_TRIALS 10

void print_usage(char *cmd) {
    fprintf(stderr, "Usage: %s <big file 1> <big file 2>\n", cmd);
}

int random_offset(size_t file_size, int buf_size) {
    return rand() % (file_size / buf_size);
}

int main(int argc, char **argv) {
    char *setup_filename, *test_filename, *buffer;
    struct stat f_stat;
    int fildes; 
    ssize_t bytes_read;
    size_t file_size;

    if (argc != 3) {
        print_usage(argv[0]);
        return 0;
    }

    setup_filename = argv[1];
    test_filename = argv[2];

    EXIT_ON_FAIL(stat(test_filename, &f_stat), "stat");
    file_size = f_stat.st_size;

    for (
            int read_size = MIN_READ_SIZE; 
            read_size <= MAX_READ_SIZE; 
            read_size += READ_SIZE_STEP
    ) {
        buffer = malloc(read_size);
        for (int trial = 0; trial < NUM_TRIALS; trial++) {
            setup_system(setup_filename); // put system into known state
            EXIT_ON_FAIL((fildes = open(test_filename, O_RDONLY)) == -1, "open");
            bytes_read = pread(fildes, buffer, read_size, random_offset(file_size, read_size));
            EXIT_ON_FAIL(bytes_read == -1, "pread");
            EXIT_ON_FAIL(bytes_read != read_size, "read too small");
            EXIT_ON_FAIL(close(fildes), "close");
        }
        free(buffer);
    }

    return 0;
}
