#include "app/eapp_utils.h"
#include "string.h"
#include "syscall.h"
#include "malloc.h"
#include "edge_wrapper.h"
#include "chipsupport.h"
#include "eapp_trojan.h"

#define REPETITION_FACTOR 10

void EAPP_ENTRY eapp_entry(){

  edge_init();

  ocall_print_buffer("Ciao bella!!!\n");

  // Before the start of the benchmark init its helper data structure
  initialise_benchmark();

  for (unsigned int r = 0; r < REPETITION_FACTOR; r++) {
    for (unsigned int o = 0; o < get_number_of_entries(); o++) {
      for (unsigned int i = 0; i < get_number_of_entries(); i++) {

        // ocall to spy to prime the cache
        ocall_prime_cache();

        encode_secret(i);

        // // ocalll to spy to probe the cache (oth cache line access), it also prints the result
        ocall_probe_cache(o);
      }
    }
  }

  EAPP_RETURN(0);
}
