/**
 * random_read.c
 *
 * Seeks to answer the question: what is the ideal buffer size for random file reads? Does this by using reads of varying sizes, timing how long each read takes.
 *
 */

#include <stdio.h>
#include "util.h"

void print_usage(char *cmd) {
    fprintf(stderr, "Usage: %s <big file 1> <big file 2>\n", cmd);
}

int main(int argc, char **argv) {
    char *bigfilename1, *bigfilename2;

    if (argc != 3) {
        print_usage(argv[0]);
        return 0;
    }

    bigfilename1 = argv[1];
    bigfilename2 = argv[2];

    setup_system(bigfilename1);

    return 0;
}
