#include "eapp_trojan.h"
#include "chipsupport.h"

l1d_work_area area1;

int entries = 256;

void initialise_benchmark() {}

volatile WORD touch_l1d_add(ADDRESS address) {
    return  *((WORD volatile*)address);
}

void encode_secret(int i) {
    for (int k = 0; k < 8; k++){
        for (unsigned int ways = 0; ways < D_WAYS; ways++) {
          touch_l1d_add((void *) ((WORD)(&area1) + i*D_LINE_SIZE + (ways * D_SETS * D_LINE_SIZE) ) );
        }
    }
}

int get_number_of_entries() {
    return entries;
}