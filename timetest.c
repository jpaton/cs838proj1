/**
 * timetest
 *
 * Tests the resolution, precision, and accuracy of CLOCK_HIGHRES against CLOCK_REALTIME
 **/

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>

int clock_gettime(clockid_t, struct timespec *tp);
int clock_getres(clockid_t, struct timespec *tp);

#define USEC_IN_SEC (1000000)
#define NSEC_IN_SEC (1000000000)
#define NUM_TRIALS 1
#define MAX_SLEEP_TIME 10 
#define MIN_SLEEP_TIME 0
#define SLEEP_TIME_STEP 5

/**
 * A list of all the clocks to test with clock_gettime
 **/
const clockid_t clocks[] = {CLOCK_REALTIME, CLOCK_MONOTONIC};
#define NUM_CLOCKS 2 /* size of the clocks array */

int subtract_timespec(struct timespec *ts1, struct timespec *ts2, struct timespec *result) {
    result->tv_sec = ts1->tv_sec - ts2->tv_sec;
    if (ts1->tv_nsec < ts2->tv_nsec) {
        result->tv_nsec = NSEC_IN_SEC - (ts2->tv_nsec - ts1->tv_nsec);
        result->tv_sec -= 1;
    } else {
        result->tv_nsec = ts1->tv_nsec - ts2->tv_nsec;
    }
    return 0;
}

void *busy_loop(void *args) {
    while (true);
    return NULL;
}

void print_time(struct timespec *tp) {
    printf("%lu s,%lu ns\n", tp->tv_sec, tp->tv_nsec);
}

void print_resolutions() {
    struct timespec tp;
    for (int clock_num = 0; clock_num < NUM_CLOCKS; clock_num++) {
        if (clock_getres(clocks[clock_num], &tp))
            perror("clock_getres");
        fprintf(stderr, "clock res for %d: %lu s, %lu ns\n", clocks[clock_num], tp.tv_sec, tp.tv_nsec);
    }
}

int main(int argv, char **argc) {
    struct {
        struct timespec start;
        struct timespec end;
    } highres_times;
    struct timespec highres_diff;
    pthread_t busy_looper;

    print_resolutions();

    if (pthread_create(&busy_looper, NULL, busy_loop, NULL))
        perror("pthread_create");
    else
        fprintf(stderr, "extra thread running...\n");

    for (int clock_num = 0; clock_num < NUM_CLOCKS; clock_num++) {
        for (int sleep_time = 0; sleep_time <= MAX_SLEEP_TIME; sleep_time += SLEEP_TIME_STEP) {
            for (int trial = 0; trial < NUM_TRIALS; trial++) {
                if (clock_gettime(clocks[clock_num], &highres_times.start))
                    perror("clock_gettime");
                sleep(sleep_time);
                if (clock_gettime(clocks[clock_num], &highres_times.end))
                    perror("clock_gettime");
                subtract_timespec(&highres_times.end, &highres_times.start, &highres_diff);
                printf("%d,%d,%d,", trial, sleep_time, clocks[clock_num]);
                print_time(&highres_diff);
            }
        }
    }

    return 0;
}
