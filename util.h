#ifndef _UTIL_H
#define _UTIL_H

#define EXIT_ON_FAIL(val, msg) if (val) { perror(msg); exit(-1); }

void setup_system(char *);

#endif
