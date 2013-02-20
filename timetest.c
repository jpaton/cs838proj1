/**
 * timetest
 *
 * Tests the resolution, precision, and accuracy of CLOCK_HIGHRES against CLOCK_REALTIME
 **/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/processor.h>
#include "rdtsc.h"
#include "util.h"

#ifndef __APPLE__
int clock_gettime(clockid_t, struct timespec *tp);
int clock_getres(clockid_t, struct timespec *tp);
#endif

#define USEC_IN_SEC (1000000)
#define NSEC_IN_SEC (1000000000)
#define NUM_TRIALS 10
#define MAX_SLEEP_TIME 60
#define MIN_SLEEP_TIME 0
#define SLEEP_TIME_STEP 5

/**
 * A list of all the clocks to test with clock_gettime
 **/
#ifndef __APPLE__
const clockid_t clocks[] = {CLOCK_REALTIME, CLOCK_MONOTONIC};
#define NUM_CLOCKS 2 /* size of the clocks array */
#endif

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

struct busy_loop_arg {
    bool done;
    pthread_mutex_t exiting_mutex;
    pthread_cond_t exiting;
};

void *busy_loop(void *_args) {
    struct busy_loop_arg *args = (struct busy_loop_arg *) _args;
    while (!args->done);
    fprintf(stderr, "busy loop exiting...\n");
    pthread_exit(0);
}

void print_time(struct timespec *tp) {
    printf("%lu s,%lu ns", tp->tv_sec, tp->tv_nsec);
}

#ifndef __APPLE__
void print_resolutions() {
    struct timespec tp;
    for (int clock_num = 0; clock_num < NUM_CLOCKS; clock_num++) {
        if (clock_getres(clocks[clock_num], &tp))
            perror("clock_getres");
        fprintf(stderr, "clock res for %d: %lu s, %lu ns\n", clocks[clock_num], tp.tv_sec, tp.tv_nsec);
    }
}
#endif

#ifndef __APPLE__
void run_trials(char *extra) {
    struct {
        struct timespec start;
        struct timespec end;
    } highres_times;
    struct timespec highres_diff;

    for (int clock_num = 0; clock_num < NUM_CLOCKS; clock_num++) {
        for (int sleep_time = MIN_SLEEP_TIME; sleep_time <= MAX_SLEEP_TIME; sleep_time += SLEEP_TIME_STEP) {
            for (int trial = 0; trial < NUM_TRIALS; trial++) {
                if (clock_gettime(clocks[clock_num], &highres_times.start))
                    perror("clock_gettime");
                sleep(sleep_time);
                if (clock_gettime(clocks[clock_num], &highres_times.end))
                    perror("clock_gettime");
                subtract_timespec(&highres_times.end, &highres_times.start, &highres_diff);
                printf("%d,%d,%d,", trial, sleep_time, clocks[clock_num]);
                print_time(&highres_diff);
                printf(",%s\n", extra);
            }
        }
    }
}
#endif

void run_rdtsc_trials(char *extra, unsigned long cpu_freq) {
    unsigned long long start, end;
    double diff, time;

    for (int sleep_time = MIN_SLEEP_TIME; sleep_time <= MAX_SLEEP_TIME; sleep_time += SLEEP_TIME_STEP) {
        for (int trial = 0; trial < NUM_TRIALS; trial++) {
            start = rdtsc();
            sleep(sleep_time);
            end = rdtsc();
            diff = (double) (end - start);
            time = diff / cpu_freq;
            printf("%d,%d,%s,%f", trial, sleep_time, "rdtsc", time);
            printf(",%s\n", extra);
            fflush(stdout);
        }
    }
}

int main(int argc, char **argv) {
    pthread_t busy_looper;
    struct busy_loop_arg *args;
    unsigned long cpu_freq;

#ifndef __APPLE__
    print_resolutions();
#endif

    cpu_freq = get_clock_frequency() * MHZ_TO_HZ;
    fprintf(stderr, "Using CPU frequency of %lu\n", cpu_freq);

    args = malloc(sizeof(struct busy_loop_arg));
    pthread_cond_init(&args->exiting, NULL);
    /**
     * pthread with process contention
     **/
    args->done = false;
    if (pthread_create(&busy_looper, NULL, busy_loop, args))
        perror("pthread_create");
    else
        fprintf(stderr, "extra thread running...\n");

    run_rdtsc_trials("pthread", cpu_freq);

    args->done = true;
    pthread_join(busy_looper, NULL);

    /**
     * no pthread
     **/
    run_rdtsc_trials("no_pthread", cpu_freq);

    return 0;
}
