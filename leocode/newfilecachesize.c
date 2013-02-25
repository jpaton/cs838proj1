#include <stdio.h>
#include <limits.h>
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
	return 1 + (unsigned long)(max * (rand() / (RAND_MAX + 1.0)));
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

void run_experiment(int fd,char* buffer, unsigned long buffer_size,unsigned long max_offset);

int main(){
	unsigned long max_offset;
	unsigned long buffer_size=1;
	int fd = open("10GigFile",O_RDONLY|O_LARGEFILE);

	if(fd == -1)
	{
		perror("file could not be opened for reading\n");
		exit(1);
	}
	char* buffer;
	flushcache();
	max_offset = (10240*1024/buffer_size)-1;
	buffer = (char*) malloc(buffer_size*1024);
	run_experiment(fd,buffer,buffer_size*1024,max_offset);
	free(buffer);
	flushcache();
	srand(time(NULL)%INT_MAX);
}

void run_experiment(int fd,char* buffer, unsigned long buffer_size,unsigned long max_offset){
	int ret;
	char * errmessage = (char*)malloc(100);
	FILE* file = fdopen(fd,"r");
	if(file == NULL)
	{
		perror("file descriptor cannot be open\n");
		exit(1);
	}
	rewind(file);
	long file_offset = 0,random_offset,previous_offset=-99999;
	unsigned long start,end;
	double elapsed_ticks;
	struct result{
		long offset;
		double time;
	};
	double min_time = 0;
	long i=0;
	while(true){
		i++;
		ret = fread(buffer,buffer_size,1,file);
		if(ret < 1)
		{
			sprintf(errmessage,"read failed bytes actual read %d expected %d \n\0",ret,1);
			perror(errmessage);
			break;
		}
		if(i==3048576 || i==5145728 || i==8291456  ){	
			long temp_offset = ftell(file);
			char filename[20];
			sprintf(filename,"cachesize%ld.txt\0",i);
			FILE* output_file = fopen(filename,"w");
			for(int j=1;j<(i-1248576)/200;j+=200){
				ret = fseek(file,-(j*1024),SEEK_CUR);
				if(ret < 0){
					perror("could not seek\n");
					exit(1);
				}
				start = getticks();
				ret = fread(buffer,buffer_size,1,file);
				end = getticks();
				if(ret < 1)
				{
					sprintf(errmessage,"read failed bytes actual read %d expected %d \n\0",ret,1);
					perror(errmessage);
					break;
				}
				elapsed_ticks = elapsed(end,start);
				fprintf(output_file,"%d %f\n",j,elapsed_ticks);
			}
			ret = fseek(file,temp_offset,SEEK_SET);
			if(ret < 0 )
			{
				perror("could not seek\n");
				exit(1);
			}
			fclose(output_file);
		}
		/*
		   if( min_time != 0 &&  elapsed_ticks-mintime > 1000)
		   {
		   fprintf(output_file,"Index %d Possibly replaced from cache old %f new %f\n",j+1,results[random_i].time,elapsed_ticks);
		   break;
		   }*/
	}
}
