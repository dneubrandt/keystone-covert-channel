#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include "keystone.h"
#include "edge_wrapper.h"
#include "chipsupport.h"
// #include "encl_message.h"

/* We hardcode these for demo purposes. */
const char* enc_path = "eapp-trojan.riscv";
const char* runtime_path = "eyrie-rt";

volatile inline uint64_t read_time() {
    uint64_t rv;
    __asm__ volatile ("rdcycle %0": "=r" (rv) ::);
    return rv;
}

unsigned long print_buffer(char* str){
  printf("[SE] %s",str);
  return strlen(str);
}

void print_value(unsigned long val){
  printf("timestamp: %lu\n",val);
  return;
}

void instructions_fence(void) {
    asm("fence.i" : : : "memory");
}

l1i_work_area l1i_area;
// l1i_work_area l1i_area_2;

void initialise_benchmark() {
  for (WORD w = 0; w < I_WAYS; w++) {
    for(WORD s = 0; s < I_SETS; s++) {
        l1i_area.returns[s*4 + w*I_SETS*4] = RET_OPCODE;
        // l1i_area_2.returns[s*4 + w*I_SETS*4] = RET_OPCODE;
    }
  }
  instructions_fence();
}

void prime_cache() {
    // execute rets in helper data structure
    // TODO? what about the other ways?
    for (int k = 0; k < 8; ++k) {
      for (WORD w = 0; w < I_WAYS; w++) {
        for(WORD s = 0; s < I_SETS; s++) {
          ((sig_fun *)&(l1i_area.returns[s*4 + w*I_SETS*4]))();//convert address to function pointer, and call it
        }
      }
    }
    
    // printf("end of prime cache\n");
}

//Alignement is required for precise time measurement: we do not want the fetch to interfere.
__attribute__ ((aligned (I_LINE_SIZE))) __attribute__ ((noinline)) volatile TIMECOUNT poke_l1i_add(sig_fun* address) {
    // address(); // cache instruction
    // evict instruction from cache (no TLB flush) using area2
    // for (int repetition = 0; repetition < 8; repetition++) {
    //   for (WORD w = 0; w < I_WAYS; ++w) {
    //       ((sig_fun *)&(l1i_area_2.returns[set_index*4 + w*I_SETS*4]))();//convert address to function pointer, and call it
    //   }
    // }

    // Access sg on the same page so we don't measure a TLB miss
    // for (int i = 0; i < L1I_WORD_COUNT; i += 1024) {
    //   ((sig_fun *) &(l1i_area.returns[i]))();
    // }
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

int main(int argc, char** argv)
{
  // before starting enclace init it's helper data structure for the benchmarks
  initialise_benchmark();

  Keystone::Enclave enclave;
  Keystone::Params params;

  if(enclave.init(enc_path, runtime_path, params) != Keystone::Error::Success){
    printf("HOST: Unable to start enclave\n");
    exit(-1);
  }

  edge_init(&enclave);

  Keystone::Error rval = enclave.run();
  printf("rval: %i\n",rval);

  return 0;
}
