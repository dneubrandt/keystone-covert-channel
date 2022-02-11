#ifndef _EDGE_WRAPPER_H_
#define _EDGE_WRAPPER_H_

#include <edge_call.h>
#include "keystone.h"

int edge_init(Keystone::Enclave* enclave);

void print_buffer_wrapper(void* buffer);
unsigned long print_buffer(char* str);

#endif /* _EDGE_WRAPPER_H_ */
