#include "app/eapp_utils.h"
#include "string.h"
#include "syscall.h"
#include "malloc.h"
#include "edge_wrapper.h"

void EAPP_ENTRY eapp_entry(){

  edge_init();

  ocall_print_buffer("Ciao bella!\n");

  EAPP_RETURN(0);
}
