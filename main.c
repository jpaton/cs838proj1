#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdbool.h>

#include "rdtsc.h"

#define USEC_IN_SEC (1000000)

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

int main(int argv, char **argc) {
	for (int i = 0; i < 10; i++) {
		unsigned long long time = rdtsc();
		printf("%llu\n", time);
		sleep(1);
	}
	
	return 0;
}
