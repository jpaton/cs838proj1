#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdint.h>
#include <cycle.h>

inline uint64_t rdtsc() {
uint32_t lo, hi;
__asm__ __volatile__ (
		"xorl %%eax, %%eax\n"
		"cpuid\n"
		"rdtsc\n"
		: "=a" (lo), "=d" (hi)
		:
		: "%ebx", "%ecx");
return (uint64_t)hi << 32 | lo;
}

int main()
{
	uint64_t start,end,sum=0,start1,end1;
	int i = 0;
	int iterations = 10;
	int max_wait_time = 2; // seconds
	int wait_time;
	int repetitions = 0;

	for(;i<iterations;i++){
		wait_time = i%max_wait_time + 1;
		start= rdtsc();
		start1=getticks();
		int time = sleep(wait_time);
		end = rdtsc();
		end1=getticks();
		if( time == 0 ){
			repetitions++;
			uint64_t count_per_second = (end-start)/wait_time;
			double diff = elapsed(end1,start1);
			sum += count_per_second;
			printf("%d. psec %llu diff %e \n",repetitions, count_per_second,diff);
		}
	}
	printf("%d repetitions\n",repetitions);
	printf("Counts per second  %llu\n",sum/repetitions);
	return 0;
}
