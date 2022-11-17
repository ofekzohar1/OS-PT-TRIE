#include "os.h"

#define MASK9b (0x1ffULL)
#define MASK12b (0xfffULL)
#define MASK_INVALID_BIT (~1ULL)

uint64_t *page_walk(uint64_t pt, uint64_t vpn) {
    uint64_t *root = phys_to_virt(pt);
    uint64_t index, pte;

    for (int i = 4; i >= 0; --i) {
        index = vpn & (MASK9b << (9 * i));
        pte = root[index];
        if ((pte & 1) == 0)
            return NULL;
        if (i > 0) {
            pte = pte & MASK12b;
            root = phys_to_virt(pte);
        }
    }
    return &root[index];
}

void page_table_update(uint64_t pt, uint64_t vpn, uint64_t ppn) {
    uint64_t *ptePointer = page_walk(pt, vpn);
    uint64_t pte;

    if (ptePointer == NULL) return;
    if (ppn == NO_MAPPING) {
        *ptePointer = (*ptePointer & MASK_INVALID_BIT);
    } else {
        pte = (ppn << 12) | 1;
        *ptePointer = pte;
    }
}

uint64_t page_table_query(uint64_t pt, uint64_t vpn) {
    uint64_t *ptePointer = page_walk(pt, vpn);

    return ptePointer == NULL ? NO_MAPPING : *ptePointer >> 12;
}