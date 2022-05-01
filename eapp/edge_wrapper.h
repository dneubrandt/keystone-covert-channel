#ifndef _EDGE_WRAPPER_H_
#define _EDGE_WRAPPER_H_
#include "edge_call.h"

void edge_init();

unsigned long ocall_print_buffer(char* data);
void ocall_print_value(unsigned long val);
// L1I
void ocall_prime_cache();
void ocall_probe_cache(int set_index);

#endif /* _EDGE_WRAPPER_H_ */
