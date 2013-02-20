#ifndef _UTIL_H
#define _UTIL_H

#define EXIT_ON_FAIL(val, msg) if (val) { perror(msg); exit(-1); }
#define MHZ_TO_HZ 1000000
#define MAX(x, y) (x > y ? x : y)

void setup_system(int, char **);

int get_clock_frequency(void);

#endif
