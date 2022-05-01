#ifndef _CHIPSUPPORT_H_
#define  _CHIPSUPPORT_H_

#include <stdint.h>

typedef uint64_t WORD;
typedef void volatile* ADDRESS;
typedef uint64_t TIMECOUNT;
typedef uint32_t INST;

#define RET_OPCODE      0x00008067  // opcode for "ret"
#define BLT01_OPCODE    0xB54063    // opcode for "blt a0, a1, 0"
#define J_OPCODE        0x6F        // opcode for "j 0" (direct jump)
#define NOP_OPCODE      0x13        // opcode for "nop"

// L1D dimensions
#define D_WAYS 64
#define D_SETS 256 // change it back
#define D_LINE_SIZE 16 // bytes
#define L1D_SIZE (D_LINE_SIZE * D_WAYS * D_SETS)

#define L1D_WORD_COUNT (L1D_SIZE/sizeof(WORD))

// A structure of the size of L1D
typedef volatile struct {
    WORD words[L1D_WORD_COUNT];
} __attribute__ ((aligned (L1D_SIZE)))
l1d_work_area;


// L1I dimensions
#define I_WAYS 4
#define I_SETS 256
#define I_LINE_SIZE 16 // in bytes
#define L1I_SIZE (I_LINE_SIZE * I_WAYS * I_SETS)

#define L1I_WORD_COUNT (L1I_SIZE/sizeof(INST))
#define L1I_ALIGNMENT (4*256*16)

// A structure of the size of L1I where we can easily write instructions
typedef volatile struct {
    INST returns[L1I_WORD_COUNT];
} __attribute__ ((aligned (L1I_ALIGNMENT)))
l1i_work_area;

typedef void sig_fun(void);

// BHT dimensions
#define BHT_ENTRIES 64
#define BHT_COUNTER_BITS    2 //to evaluate the number of passes for training

// A structure of the size the number of bht entries + 1 (for last return instruction) where we can easily write instructions
typedef volatile struct {
    INST entries[BHT_ENTRIES+1];
} __attribute__ ((aligned (256))) //TODO: I get a bug with smaller alignments // aligment in bytes
bht_work_area;

typedef void sig_br(WORD rs1, WORD rs2);

// BTB dimensions
#define BTB_ENTRIES 16

#define JUMP_ALIGNMENT 1  // HALF WORD size (4 bytes) alignment

typedef volatile void sig_jump();

// A structure of the size the number of btb entries + 1 (for last return instruction) where we can easily write instructions
typedef volatile struct {
    INST entries[(BTB_ENTRIES * JUMP_ALIGNMENT)+1];
} __attribute__ ((aligned (256))) //TODO: I get a bug with smaller alignments
btb_work_area;

#endif /* _CHIPSUPPORT_H_ */