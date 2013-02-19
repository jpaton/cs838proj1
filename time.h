#ifndef _TIME_H
#define _TIME_H

#define USEC_IN_SEC (1000000)
#define NSEC_IN_SEC (1000000000)

#include <sys/time.h>
#include <time.h>

extern int clock_gettime(clockid_t, struct timespec *);

extern int subtract_timespec(struct timespec *, struct timespec *, struct timespec *);

extern void print_time(struct timespec *);

#endif
