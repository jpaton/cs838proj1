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
#include "rdtsc.h"
#include "util.h"

#define USEC_IN_SEC (1000000)
#define NSEC_IN_SEC (1000000000)
#define NUM_TRIALS 10
#define MAX_SLEEP_TIME 60
#define MIN_SLEEP_TIME 0
#define SLEEP_TIME_STEP 5

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

    if (!invariant_tsc()) 
        fprintf(stderr, "WARNING: TSC is not invariant!!!\n");
    else
        fprintf(stderr, "TSC is invariant.\n");

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
