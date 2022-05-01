#include "host-spy.h"
#include "chipsupport.h"
#include <cstdio>

btb_work_area area;

// save the opcode for easy rewritings
uint32_t zero_jumps[BTB_ENTRIES * JUMP_ALIGNMENT];
uint32_t one_jumps[BTB_ENTRIES * JUMP_ALIGNMENT];

void __attribute__ ((noinline)) instructions_fence(void) {
    asm("fence.i" : : : "memory");
}

unsigned int encode_jump_offset(unsigned int offset) {
    unsigned int imm = offset & 0xFF000;
    imm |= (offset << 11) & 0x80000000;
    imm |= (offset << 20) & 0x7FE00000;
    imm |= (offset << 9) & 0x00100000;
    return imm;
}

volatile inline uint64_t read_time() {
    uint64_t rv;
    __asm__ volatile ("rdcycle %0": "=r" (rv) ::);
    return rv;
}

// ret_one and ret_zero content is not important, but they are two distinct jump destinations.
__attribute__ ((noinline)) volatile WORD ret_zero() {
    return 0;
}

__attribute__ ((noinline)) volatile WORD ret_one() {
    return 1;
}

// the training gadget is an array of "j [ret_zero]" (size = BTB_ENTRIES), terminating with a ret
volatile void initalise_benchmark() {
   
    //write ret at the end
    area.entries[BTB_ENTRIES * JUMP_ALIGNMENT] = RET_OPCODE;
    
    WORD ret_zero_add = ((WORD) ret_zero);
    WORD ret_one_add = ((WORD) ret_one);

    for(WORD i = 0; i < BTB_ENTRIES; i++) {
         volatile INST* jadd = &(area.entries[i * JUMP_ALIGNMENT]);
        zero_jumps[i * JUMP_ALIGNMENT] = J_OPCODE | encode_jump_offset(ret_zero_add - (WORD)jadd);
        one_jumps[i * JUMP_ALIGNMENT]  = J_OPCODE | encode_jump_offset(ret_one_add - (WORD)jadd);

        // *jadd = zero_jumps[i]; // not necessary here
    }

    //1 - rewrite "j ret_zero" everywhere
    for(WORD i = 0; i < BTB_ENTRIES; i++) {
        INST* callsite = (INST*) &(area.entries[i * JUMP_ALIGNMENT]);
        *callsite = zero_jumps[i * JUMP_ALIGNMENT];
        for (int j = 1; j < JUMP_ALIGNMENT; j++) {
            area.entries[i*JUMP_ALIGNMENT+j] = NOP_OPCODE;
        }
    }
}

void prime_cache() {
   int passes = 1;

    //1 - rewrite "j ret_zero" everywhere
    for(WORD i = 0; i < BTB_ENTRIES; i++) {
        INST* callsite = (INST*) &(area.entries[i * JUMP_ALIGNMENT]);
        *callsite = zero_jumps[i * JUMP_ALIGNMENT];
        for (int j = 1; j < JUMP_ALIGNMENT; j++) {
            area.entries[i*JUMP_ALIGNMENT+j] = NOP_OPCODE;
        }
    }

//     // 2 - train. Jump to jump in training gadget.
//     for(WORD p = 0; p < passes; p++) {

//         for(WORD i = 0; i < BTB_ENTRIES; i++) {
//             sig_jump* j = (sig_jump*)  &(area.entries[i * JUMP_ALIGNMENT]); // don't need
//             asm volatile (".align 7");
//             asm volatile("auipc ra, 0;"
//                  "addi ra, ra, 10;"
//                  "jalr x0, %0;"::"r"(&(area.entries[i * JUMP_ALIGNMENT]))); // j();
//         }

//     }
}

//Alignement is required for precise time measurement: we do not want the fetch to interfere.
__attribute__ ((aligned (I_LINE_SIZE))) __attribute__ ((noinline)) TIMECOUNT poke_one_btb(WORD i) {
    // volatile uint32_t* address = (uint32_t*)((uint32_t)training_btb) + (i << 2);
    volatile INST* address = &(area.entries[i * JUMP_ALIGNMENT]);

    //1 - rewrite "j ret_one"
    *address = one_jumps[i * JUMP_ALIGNMENT];
    instructions_fence();

    

    //2 - measure time
    sig_jump* j = (sig_jump*) address;
    asm volatile (".align 7");
    TIMECOUNT start = read_time();
    asm volatile("auipc ra, 0;"
                 "addi ra, ra, 10;"
                 "jalr x0, %0;"::"r"(address)); // j();
    TIMECOUNT end = read_time();
        
    //3 - rewrite "j ret_zero"
    *address = zero_jumps[i * JUMP_ALIGNMENT];
    instructions_fence();

    return (end - start);
}

void probe_cache(int set_index) {
  TIMECOUNT result = poke_one_btb(set_index);
  printf("%lu\n", result);
  return ;
}