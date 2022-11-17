#include "os.h"

#define MASK9b 0x1ffULL
#define MASK12b 0xfffULL

void page_table_update(uint64_t pt, uint64_t vpn, uint64_t ppn) {
    char *root = phys_to_virt(pt);

}

uint64_t page_table_query(uint64_t pt, uint64_t vpn) {
    uint64_t *root = phys_to_virt(pt);
    uint64_t index;
    uint64_t tempVa;

    for (int i = 4; i >= 0; --i) {
        index = vpn & (MASK9b << (9 * i));
        tempVa = root[index];
        if ((tempVa & 1) == 0)
            return NO_MAPPING;
        tempVa = tempVa & (MASK12b);
        if (i > 0)
            root = phys_to_virt(tempVa);
    }
    return tempVa >> 12;
}