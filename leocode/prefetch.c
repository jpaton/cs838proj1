#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "cycle.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <math.h>
#include "util.h"

unsigned long getrandom(unsigned long max){
	return 1 + (max * (rand() / (RAND_MAX + 1.0)));
}

void run_experiment(FILE* file,char* buffer, unsigned long buffer_size,unsigned long max_offset){
	rewind(file);
	int ret, i;
	unsigned long start,end,elapsed_ticks;
	printf("Buffer_size,Iteration\tTime\n");
	for(i=1;i<=1000;i++)
	{
		start = getticks();
		ret = fread(buffer,buffer_size,1,file);
		end = getticks();
		if(ret == 0)
		{
			perror("read failed\n");
			exit(1);
		}
		elapsed_ticks = elapsed(end,start);
		printf("%lu,%d\t%lu\n",buffer_size,i,elapsed_ticks);
	}
}

int main(int argc, char **argv){

	unsigned long max_offset=671088640 / 5; // No: of 16 Bytes in 10 GB
	unsigned long buffer_size=16; // Initial buffer size

	FILE* file = fopen(argv[1],"r");

	if(file == NULL)
	{
		perror("file could not be opened for reading\n");
		exit(1);
	}
	char* buffer;
	for(;buffer_size <= 131072;buffer_size += 16,max_offset/=16){
		flushcache(argc - 2, &argv[2]);
		buffer = (char*) malloc(buffer_size);
		run_experiment(file,buffer,buffer_size,max_offset);
		free(buffer);
		srand(time(NULL)%INT_MAX);
	}
    return 0;
}
