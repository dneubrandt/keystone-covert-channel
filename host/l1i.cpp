#include "host-spy.h"
#include "chipsupport.h"
#include <cstdio>

l1i_work_area l1i_area;

void instructions_fence(void) {
    asm("fence.i" : : : "memory");
}

volatile inline uint64_t read_time() {
    uint64_t rv;
    __asm__ volatile ("rdcycle %0": "=r" (rv) ::);
    return rv;
}

void initialise_benchmark() {
  for (WORD w = 0; w < I_WAYS; w++) {
    for(WORD s = 0; s < I_SETS; s++) {
        l1i_area.returns[s*4 + w*I_SETS*4] = RET_OPCODE;
    }
  }
  instructions_fence();
}

void prime_cache() {
    // execute rets in helper data structure
    for (int k = 0; k < 8; ++k) {
      for (WORD w = 0; w < I_WAYS; w++) {
        for(WORD s = 0; s < I_SETS; s++) {
          ((sig_fun *)&(l1i_area.returns[s*4 + w*I_SETS*4]))();//convert address to function pointer, and call it
        }
      }
    }
}

//Alignement is required for precise time measurement: we do not want the fetch to interfere.
__attribute__ ((aligned (I_LINE_SIZE))) __attribute__ ((noinline)) volatile TIMECOUNT poke_l1i_add(sig_fun* address) {
    TIMECOUNT start = read_time();
    address();
    TIMECOUNT end = read_time();
    return (end - start);
}

void probe_cache(int set_index) {
  TIMECOUNT result = poke_l1i_add((sig_fun *) &(l1i_area.returns[set_index * 4]));
  printf("%llu\n", result);
  return ;
}