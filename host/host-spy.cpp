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
#include "host-spy.h"

/* We hardcode these for demo purposes. */
const char* enc_path = "eapp-trojan.riscv";
const char* runtime_path = "eyrie-rt";

unsigned long print_buffer(char* str){
  printf("[SE] %s",str);
  return strlen(str);
}

void print_value(unsigned long val){
  printf("timestamp: %lu\n",val);
  return;
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
