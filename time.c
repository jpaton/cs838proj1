#include <stdio.h>

#include "time.h"

extern int subtract_timespec(struct timespec *ts1, struct timespec *ts2, struct timespec *result) {
    result->tv_sec = ts1->tv_sec - ts2->tv_sec;
    if (ts1->tv_nsec < ts2->tv_nsec) {
        result->tv_nsec = NSEC_IN_SEC - (ts2->tv_nsec - ts1->tv_nsec);
        result->tv_sec -= 1;
    } else {
        result->tv_nsec = ts1->tv_nsec - ts2->tv_nsec;
    }
    return 0;
}

extern void print_time(struct timespec *tp) {
    printf("%lu s,%lu ns", tp->tv_sec, tp->tv_nsec);
}
