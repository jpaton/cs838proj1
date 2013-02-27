#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#if defined(__sun) && defined(__SVR4)
#include <sys/processor.h>
#endif
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include "util.h"

#define BUF_SIZE (1<<20)

/**
 * Put the system into a known state by reading a file into memory
 **/
void setup_system(int num_files, char **filenames) {
  char *buffer;
  int fildes;

  //EXIT_ON_FAIL(fstat(fildes, &f_stat), "fstat");
  
  buffer = malloc(BUF_SIZE);
  EXIT_ON_FAIL(buffer == NULL, "malloc");

  /* read in the whole file to bring it into cache */
  for (int i = 0; i < num_files; i++) {
      EXIT_ON_FAIL((fildes = open(filenames[i], O_RDONLY)) == -1, "open");
      while (true) {
        ssize_t read_size = read(fildes, buffer, BUF_SIZE);
        if (read_size == -1) {
            if (errno == EIO)
                perror("read");
            else
                EXIT_ON_FAIL(true, "read");
        }
        buffer[0] = buffer[1] + buffer[2]; //make sure nothing is optimized out or some BS
        if (read_size == 0) break;
      }
      EXIT_ON_FAIL(close(fildes), "close");
  }
  free(buffer);
}

#if defined(__sun) && defined(__SVR4)
long get_clock_frequency(void) {
    processorid_t cpuid = getcpuid();
    processor_info_t info;
    EXIT_ON_FAIL(processor_info(cpuid, &info), "processor_info");
    return info.pi_clock;
}
#else
long get_clock_frequency(void) {
    return 3400l;
}
#endif
