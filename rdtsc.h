#include <stdbool.h>
/**
 * This code taken from the Intel document I found online.
 * http://www.intel.com/content/dam/www/public/us/en/documents/white-papers/ia-32-ia-64-benchmark-code-execution-paper.pdf
 **/
static __inline__ unsigned long long rdtsc(void)
{
	unsigned hi, lo;
	__asm__ __volatile__ (
      "rdtscp\n\t"
      : "=a"(lo), "=d"(hi));
	return ( (unsigned long long)lo) | ( ((unsigned long long)hi)<<32 );
}

static __inline__ unsigned cpuid_features(void) 
{ 
  unsigned features;
  __asm__ __volatile__ (
      "movl $0x0, %%eax\n\t"
      "cpuid\n\t"
      : "=a" (features)
      :
      : "%ebx", "%ecx", "%edx");
  return features;
}

static __inline__ bool invariant_tsc(void)
{
  unsigned ret;
  __asm__ __volatile__ (
      "movl $0x80000007, %%eax\n\t"
      "cpuid\n\t"
      "movl %%edx, %0\n\t"
      : "=d"(ret)
      :
      : "%eax"
      );
  return ret & 1 << 8;
}
