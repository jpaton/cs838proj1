/**
 * This code taken from the Intel document I found online.
 * http://www.intel.com/content/dam/www/public/us/en/documents/white-papers/ia-32-ia-64-benchmark-code-execution-paper.pdf
 **/
static __inline__ unsigned long long rdtsc_start(void)
{
	unsigned hi, lo;
	__asm__ __volatile__ (
      "cpuid\n\t"
      "rdtsc\n\t" 
      "movl %%edx, %0\n\t"
      "movl %%eax, %1\n\t" : "=r"(lo), "=r"(hi)
      : 
      : "%edx", "%eax", "memory");
	return ( (unsigned long long)lo) | ( ((unsigned long long)hi)<<32 );
}

static __inline__ unsigned long long rdtsc_end(void)
{
	unsigned hi, lo;
	__asm__ __volatile__ (
      "rdtscp\n\t"
      "cpuid\n\t"
      "movl %%edx, %0\n\t"
      "movl %%eax, %1\n\t" : "=r"(lo), "=r"(hi)
      :
      : "%edx", "%eax", "memory");
	return ( (unsigned long long)lo) | ( ((unsigned long long)hi)<<32 );
}
