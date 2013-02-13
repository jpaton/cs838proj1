#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>

#include "rdtsc.h"

#define USEC_IN_SEC (1000000)
#define NSEC_IN_SEC (1000000000)

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

int subtract_tp(struct timeval *tp1, struct timeval *tp2, struct timeval *result) {
	result->tv_sec = tp1->tv_sec - tp2->tv_sec;
	if (tp1->tv_usec < tp2->tv_usec) {
		result->tv_usec = USEC_IN_SEC - (tp2->tv_usec - tp1->tv_usec);
		result->tv_sec -= 1;
	} else {
		result->tv_usec = tp1->tv_usec - tp2->tv_usec;
	}
	return 0;
}

void *busy_loop(void *args) {
	while (true);
}

void print_time(struct timespec *tp) {
	printf("time:\t%lu s, %lu ns\n", tp->tv_sec, tp->tv_nsec);
}

void print_rdtsc(unsigned long long to) {
	printf("rdtsc:\t%llu\n", to);
}

int main(int argv, char **argc) {
	struct {
		unsigned long long start;
		unsigned long long end;
	} rdtsc_times;
	struct {
		struct timespec start;
		struct timespec end;
	} real_times;
	unsigned long long rdtsc_diff;
	struct timespec real_diff;
	pthread_t busy_looper;

	if (pthread_create(&busy_looper, NULL, busy_loop, NULL))
		perror("pthread_create");
	rdtsc_times.start = rdtsc_start();
	if (clock_gettime(CLOCK_HIGHRES, &real_times.start))
		perror("clock_gettime");
	sleep(10);
	rdtsc_times.end = rdtsc_end();
	if (clock_gettime(CLOCK_HIGHRES, &real_times.end))
		perror("clock_gettime");
	rdtsc_diff = rdtsc_times.end - rdtsc_times.start;
	subtract_timespec(&real_times.end, &real_times.start, &real_diff);

	print_time(&real_diff);
	print_rdtsc(rdtsc_diff);
	
	return 0;
}
