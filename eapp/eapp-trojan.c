#include "app/eapp_utils.h"
#include "string.h"
#include "syscall.h"
#include "malloc.h"
#include "edge_wrapper.h"
#include "chipsupport.h"

#define REPETITION_FACTOR 10

l1i_work_area l1i_area;
// l1i_work_area padding;
// l1i_work_area padding1;
// l1i_work_area padding2;
// l1i_work_area padding3;
// l1i_work_area padding4;
// l1i_work_area padding5;
// l1i_work_area padding6;

int entries = 256;

// volatile inline int read_time() {
//     int rv;
//     __asm__ volatile ("rdcycle %0": "=r" (rv) ::);
//     return rv;
// }

void instructions_fence(void) {
    asm("fence.i" : : : "memory");
}

// only inits the first way?
void initialise_benchmark() {
  for (WORD w = 0; w < I_WAYS; w++) {
    for(WORD s = 0; s < I_SETS; s++) {
        l1i_area.returns[s*4 + w*I_SETS*4] = RET_OPCODE;
    }
  }
  instructions_fence();
}

void touch_l1i_add(sig_fun* address) {
    address(); //expect a return instruction at address
}

void EAPP_ENTRY eapp_entry(){

  edge_init();

  ocall_print_buffer("Ciao bella!!!\n");

  // Before the start of the benchmark init its helper data structure
  initialise_benchmark();

  for (unsigned int r = 0; r < REPETITION_FACTOR; r++) {
    for (unsigned int o = 0; o < entries; o++) {
      for (unsigned int i = 0; i < entries; i++) {

        // ocall to spy to prime the cache
        ocall_prime_cache();

        // touch a cache line in L1I cache (in all the ways)
        // do multiple rounds of eviction to be sure
        for (int k = 0; k < 8; ++k) {
          for (WORD w = 0; w < I_WAYS; ++w) {
            touch_l1i_add((void *) &(l1i_area.returns[i*4 + w*I_SETS*4]) );
          }
        }

        // // ocalll to spy to probe the cache (oth cache line access), it also prints the result
        ocall_probe_cache(o);
      }
    }
  }

  EAPP_RETURN(0);
}
