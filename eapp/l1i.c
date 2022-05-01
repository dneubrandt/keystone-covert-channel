#include "eapp_trojan.h"
#include "chipsupport.h"

l1i_work_area l1i_area;

int entries = 256;

void instructions_fence(void) {
    asm("fence.i" : : : "memory");
}

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

// touch a cache line in L1I cache (in all the ways)
// do multiple rounds of eviction to be sure
void encode_secret(int i) {
  for (int k = 0; k < 8; ++k) {
    for (WORD w = 0; w < I_WAYS; ++w) {
      touch_l1i_add((void *) &(l1i_area.returns[i*4 + w*I_SETS*4]) );
    }
  }
}

int get_number_of_entries() {
    return entries;
}