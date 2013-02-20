#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/processor.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include "util.h"

#define BUF_SIZE (1<<10)

/**
 * Put the system into a known state by reading a file into memory
 **/
void setup_system(char **filenames) {
  char buffer[BUF_SIZE];
  int fildes;

  //EXIT_ON_FAIL(fstat(fildes, &f_stat), "fstat");

  /* read in the whole file to bring it into cache */
  for (int i = 0; i < 4; i++) {
      EXIT_ON_FAIL((fildes = open(filenames[i], O_RDONLY)) == -1, "open");
      while (true) {
        ssize_t read_size = read(fildes, buffer, BUF_SIZE);
        EXIT_ON_FAIL(read_size == -1, "read");
        buffer[0] = buffer[1] + buffer[2]; //make sure nothing is optimized out or some BS
        if (read_size == 0) break;
      }
      EXIT_ON_FAIL(close(fildes), "close");
  }
}

int get_clock_frequency(void) {
    processorid_t cpuid = getcpuid();
    processor_info_t info;
    EXIT_ON_FAIL(processor_info(cpuid, &info), "processor_info");
    return info.pi_clock;
}
