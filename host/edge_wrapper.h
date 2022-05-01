#ifndef _EDGE_WRAPPER_H_
#define _EDGE_WRAPPER_H_

#include <edge_call.h>
#include "keystone.h"

int edge_init(Keystone::Enclave* enclave);

void print_buffer_wrapper(void* buffer);
unsigned long print_buffer(char* str);

void print_value_wrapper(void* buffer);
void print_value(unsigned long val);

// L1I
void prime_cache_wrapper(void* buffer);
void prime_cache();

void probe_cache_wrapper(void* buffer);
void probe_cache(int set_index);

#endif /* _EDGE_WRAPPER_H_ */
