#include "os.h"

#define MASK9b (0x1ffULL)
#define MASK12b (0xfffULL)
#define MASK_INVALID_BIT (~1ULL)

int page_walk(uint64_t pt, uint64_t vpn, uint64_t **pteResalt) {
    uint64_t *root = phys_to_virt(pt);
    uint64_t index, pte;
    int i;

    for (i = 4; i >= 0; --i) {
        index = vpn & (MASK9b << (9 * i));
        pte = root[index];
        if ((pte & 1) == 0) {
            --i;
            break;
        }
        if (i > 0) {
            pte = pte & MASK12b;
            root = phys_to_virt(pte);
        }
    }
    *pteResalt = &root[index];
    return 5 - i - 1;
}

void page_table_update(uint64_t pt, uint64_t vpn, uint64_t ppn) {
    uint64_t *ptePtr;
    uint64_t pte;
    int walk_steps = page_walk(pt, vpn, &ptePtr);

    if (ppn == NO_MAPPING) {
        // if (walk_steps < 5) return; // Not exist - no further actions
        *ptePtr = (*ptePtr & MASK_INVALID_BIT); // Invalidate the valid bit
        return;
    }

    for (int i = walk_steps; i <= 5; i++) {
        pte = (i < 5 ? alloc_page_frame() : ppn) << 12;
        *ptePtr = pte | 1; // Valid bit to 1
        if (i < 5)
            ptePtr = phys_to_virt(pte);
    }
}

uint64_t page_table_query(uint64_t pt, uint64_t vpn) {
    uint64_t *ptePtr;
    int walk_steps = page_walk(pt, vpn, &ptePtr);

    return walk_steps == 5 ? *ptePtr >> 12 : NO_MAPPING;
}