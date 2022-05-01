#include "host-spy.h"
#include "chipsupport.h"
#include <cstdio>

l1d_work_area l1d_area2;

inline uint64_t read_time() {
    uint64_t rv;
    __asm__ volatile ("rdcycle %0": "=r" (rv) ::);
    return rv;
}

void initialise_benchmark() {}

volatile WORD touch_l1d_add(ADDRESS address) {
    return  *((WORD volatile*)address);
}

void prime_cache() {
  #ifdef TIMESTAMPS
  start_prime = read_time();
  #endif
   // to prepare trojan, we must touch memory someplaces never touched by trojan as to fill the cache
  for (int k = 0; k < 1; k++) {
    for (WORD w = 0; w < D_WAYS; w++) {
      for (WORD s = 0; s < D_SETS; s++) {
          touch_l1d_add((void*)((WORD)(&l1d_area2) + D_LINE_SIZE * (s + w*D_SETS)));
      }
    }
  }
  #ifdef TIMESTAMPS
  end_prime = read_time();
  #endif
}

//Alignement is required for precise time measurement: we do not want the fetch to interfere.
__attribute__ ((aligned (I_LINE_SIZE))) __attribute__ ((noinline)) volatile TIMECOUNT poke_l1d_add(ADDRESS address) {
    // touch_l1d_add(address); // it'll be caches in data cache and TLB
    // Access sg on the same page so we don't measure a TLB miss. The first entries on every page will be cache hits too
    // without TLB misses, and the rest is cache hit/miss without TLB miss.
    // touch_l1d_add((void*)&(l1d_area2.words[0]));
    TIMECOUNT start = read_time();
    touch_l1d_add(address); // time how much does it take from L1 cache
    TIMECOUNT end = read_time();
    return (end - start);
}

void probe_cache(int set_index) {
  #ifdef TIMESTAMPS
  start_probe = read_time();
  #endif
  TIMECOUNT result = poke_l1d_add((void *) ((WORD)(&l1d_area2) + set_index*D_LINE_SIZE) ); // try to poke in every way?
  printf("%lu\n", result);
  // cycles[counter++] = result;
  #ifdef TIMESTAMPS
  end_probe = read_time();
  #endif
  return ;
}