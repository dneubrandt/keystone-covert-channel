#ifndef _CHIPSUPPORT_H_
#define  _CHIPSUPPORT_H_

#include <stdint.h>

typedef uint64_t WORD;
typedef void volatile* ADDRESS;
typedef uint64_t TIMECOUNT;
typedef uint32_t INST;

#define RET_OPCODE      0x00008067  // opcode for "ret"

// L1D dimensions
#define D_WAYS 64
#define D_SETS 256 // change it back
#define D_LINE_SIZE 16 // bytes
#define L1D_SIZE (D_LINE_SIZE * D_WAYS * D_SETS)

#define L1D_WORD_COUNT (L1D_SIZE/sizeof(WORD))

// L1I dimensions
#define I_WAYS 4
#define I_SETS 256
#define I_LINE_SIZE 16 // in bytes
#define L1I_SIZE (I_LINE_SIZE * I_WAYS * I_SETS)

#define L1I_WORD_COUNT (L1I_SIZE/sizeof(INST))
#define L1I_ALIGNMENT (4*256*16)

typedef void sig_fun(void);

// A structure of the size of L1D
typedef volatile struct {
    WORD words[L1D_WORD_COUNT];
} __attribute__ ((aligned (L1D_SIZE)))
l1d_work_area;

// A structure of the size of L1I where we can easily write instructions
typedef volatile struct {
    INST returns[L1I_WORD_COUNT];
} __attribute__ ((aligned (L1I_ALIGNMENT)))
l1i_work_area;

#endif /* _CHIPSUPPORT_H_ */