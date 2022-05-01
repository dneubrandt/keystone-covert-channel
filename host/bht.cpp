#include "host-spy.h"
#include "chipsupport.h"
#include <cstdio>

bht_work_area area2;
volatile WORD* dummy;

volatile inline uint64_t read_time() {
    uint64_t rv;
    __asm__ volatile ("rdcycle %0": "=r" (rv) ::);
    return rv;
}

unsigned int encode_branch_offset(unsigned int offset) {
    offset <<= 2;
    unsigned int final = (offset >> 4) & 0x80; // 1 bit mask
    final |= (offset << 7) & 0xF00; // 4 bit mask
    final |= (offset << 20) & 0x7E000000; // 6 bit mask
    final |= (offset << 21) & 0x80000000; // 1 bit mask
    return final;
}

// the gadget is an array of "blt a0, a1, [gadget_end]" (size = BHT_ENTRIES), terminating with a ret
// if we execute one instruction, it will be the only branch to be taken if the condition is met, or all successive branches will be taken if not met.
void initialise_benchmark() {
    //write ret at the end
    area2.entries[BHT_ENTRIES] = RET_OPCODE;
    uint32_t off0_val = BLT01_OPCODE;

    for(uint32_t i = 0; i < BHT_ENTRIES; i++) {
        uint32_t offset = &area2.entries[BHT_ENTRIES] - &area2.entries[i];
        // offset <<= 2;
        
        area2.entries[i] = off0_val | encode_branch_offset(offset);
    }
}

//assume gadget has been written
//init bht with "not taken" condition: "blt a0=2, a1=1, [end]"
void prime_cache() {   
    WORD nb_passes = 1 << BHT_COUNTER_BITS; 
    sig_br* start_branch = (sig_br*)(&area2.entries[0]);
    //take the branches
    for(WORD i = 0; i < nb_passes; i++) {
        start_branch(2, 1);
        // return here after all branches from the training gadget have been taken.
        // we do not iterate on the bht entries since all branches instructions in the gadget will be taken successively (condition not met)
    }
}

//Alignement is required for precise time measurement: we do not want the fetch to interfere.
__attribute__ ((aligned (I_LINE_SIZE))) __attribute__ ((noinline)) TIMECOUNT poke_taken_bht(WORD i) {
    sig_br* touch_branch = (sig_br*) (&area2.entries[i]);
    dummy = ((WORD*)touch_branch); // write it somewhere to trigger the addresses computation before the rdtime
    TIMECOUNT start = read_time();
    touch_branch(1, 2);
    TIMECOUNT end = read_time();
    return (end - start);
}

void probe_cache(int set_index) {
  TIMECOUNT result = poke_taken_bht(set_index);
  printf("%lu\n", result);
  return ;
}