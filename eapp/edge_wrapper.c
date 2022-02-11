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

