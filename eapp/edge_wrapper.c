#include "eapp_utils.h"
#include "string.h"
#include "syscall.h"
#include "edge_wrapper.h"
#include "edge_defines.h"

void edge_init(){
  /* Nothing for now, will probably register buffers/callsites
     later */
}

unsigned long ocall_print_buffer(char* data){

  unsigned long retval;
  ocall(OCALL_PRINT_BUFFER, data, strlen(data)+1, &retval ,sizeof(unsigned long));

  return retval;
}

void ocall_print_value(unsigned long val){

  unsigned long val_ = val;
  ocall(OCALL_PRINT_VALUE, &val_, sizeof(unsigned long), 0, 0);

  return;
}

// L1I ---------------------------------------

// this is just to give back execution to host
void ocall_prime_cache() {
  ocall(OCALL_PRIME_CACHE, NULL, 0, 0, 0);
  return;
}

void ocall_probe_cache(int set_index) {
  int set_index_ = set_index; // why do I need this?
  ocall(OCALL_PROBE_CACHE, &set_index_, sizeof(int), 0, 0);
  return;
}