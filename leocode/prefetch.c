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
#include <linux/fs.h>
#include <sys/ioctl.h>
#include <math.h>

unsigned long getrandom(unsigned long max){
	return 1 + (max * (rand() / (RAND_MAX + 1.0)));
}

void flushcache(){
	sync();
	/*
	   pid_t pid;
	   int status;
	   fork();
	   if(pid == 0){
	//child
	exec("echo 3
	}else{
	//parent
	wait(&status);
	if(!WIFEXITED(status)){
	perror("Child did not exit properly\n");
	}
	}
	int fd = open("/proc/sys/vm/drop_caches",
	close(fd);
	 */
	sleep(1);
	int fd = open("/dev/sda5",O_RDONLY);
	if (fd < 0) {
		perror ("open of blk device for flush of read cache\n");
		exit(1);
	}
	int ret = ioctl(fd, BLKFLSBUF);
	if(ret){
		perror("read cache flush failed\n");
		exit(1);
	}
	close(fd);
	sleep(2);
	//printf("flushed cache\n");
}

void run_experiment(FILE* file,char* buffer, unsigned long buffer_size,unsigned long max_offset){
	rewind(file);
	int ret;
	long file_offset = 0;
	unsigned long start,end,elapsed_ticks;
	char* file_name = (char*)malloc(20);
	sprintf(file_name,"%lu.txt",buffer_size);
	FILE* output_file = fopen(file_name,"w");
	fprintf(output_file,"Iteration\tTime\n");
	for(int i=1;i<=1000;i++)
	{
		start = getticks();
		ret = fread(buffer,buffer_size,1,file);
		if(ret != 0)
		{
			perror("read failed\n");
			exit(1);
		}
		end = getticks();
		elapsed_ticks = elapsed(end,start);
		fprintf(output_file,"%d\t%f\n",i,elapsed_ticks);
	}
	fclose(output_file);
}

int main(){

	unsigned long max_offset=671088640; // No: of 16 Bytes in 10 GB
	unsigned long buffer_size=16; // Initial buffer size

	FILE* file = fopen("10GigFile","r");

	if(file == NULL)
	{
		perror("file could not be opened for reading\n");
		exit(1);
	}
	char* buffer;
	for(;buffer_size <= 131072;buffer_size += 16,max_offset/=16){
		buffer = (char*) malloc(buffer_size);
		run_experiment(file,buffer,buffer_size,max_offset);
		free(buffer);
		flushcache();
		srand(time(NULL)%INT_MAX);
	}
}
