#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "util.h"

#define BUF_SIZE 4092

/**
 * Put the system into a known state by reading a file into memory
 **/
void setup_system(char *filename) {
  char buffer[BUF_SIZE];
  int fildes;
  struct stat f_stat;

  EXIT_ON_FAIL((fildes = open(filename, O_RDONLY)) == -1, "open");

  EXIT_ON_FAIL(fstat(fildes, &f_stat), "fstat");

  /* read in the whole file to bring it into cache */
  int bytes_read = 0;
  while (bytes_read < f_stat.st_size) {
    ssize_t read_size = read(fildes, buffer, BUF_SIZE);
    EXIT_ON_FAIL(read_size == -1, "read");
    bytes_read += read_size;
    buffer[0] = buffer[1] + buffer[2];
  }

  EXIT_ON_FAIL(close(fildes), "close");
}
