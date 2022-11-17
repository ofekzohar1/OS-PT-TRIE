#include "os.h"

#define MASK9b (0x1ffUL)
#define MASK12b (0xfffUL)
#define MASK_INVALID_BIT (~1UL)

/* Perform page walk until get to a leaf (not necessarily in the 5th level).
 * Save pointer to the leaf inside pteResalt.
 * Return - # walk steps or leaf depth (starting from 1), 6 := walk complete.
 * */
int page_walk(uint64_t pt, uint64_t vpn, uint64_t **pteResalt) {
    uint64_t *root = phys_to_virt(pt << 12);
    uint64_t index, pte;
    int i;

    for (i = 4; i >= 0; --i) {
        index = (vpn >> (9 * i)) & MASK9b; // Get 9 bit offset from vpn
        pte = root[index];
        if ((pte & 1) == 0) // Bit valid = 0
            break;
        if (i > 0) {
            pte = pte & (~MASK12b); // zero the first 12 bits -> PA
            root = phys_to_virt(pte); // Get next node in trie
        }
    }
    *pteResalt = &root[index]; // Save pointer to the leaf
    return 5 - i; // walk steps = 5 - i
}

/* Update vpn map to ppn. if ppn = NO_MAPPING, make vpn invalid.
 *
 * */
void page_table_update(uint64_t pt, uint64_t vpn, uint64_t ppn) {
    uint64_t *ptePtr;
    uint64_t pte, index;

    int walk_steps = page_walk(pt, vpn, &ptePtr);

    if (ppn == NO_MAPPING) {
        *ptePtr = (*ptePtr & MASK_INVALID_BIT); // Invalidate valid bit, if steps < 6 it's already invalid
        return;
    }

    index = 0; // First ptePtr point to a leaf
    walk_steps = walk_steps == 6 ? 5 : walk_steps; // If walk complete, behave like last level
    for (int i = walk_steps; i <= 5; i++) {
        // Alloc new nodes, for the last level update pte to desired ppn
        pte = (i < 5 ? alloc_page_frame() : ppn) << 12;
        ptePtr[index] = pte | 1; // Valid bit to 1
        if (i < 5) {
            index = (vpn >> (9 * (5 - i - 1))) & MASK9b; // Get 9 bit offset from vpn
            ptePtr = phys_to_virt(pte);
        }
    }
}

/* Return the ppn mapped to vpn. If none exist, return NO_MAPPING */
uint64_t page_table_query(uint64_t pt, uint64_t vpn) {
    uint64_t *ptePtr;
    int walk_steps = page_walk(pt, vpn, &ptePtr);

    return walk_steps == 6 ? *ptePtr >> 12 : NO_MAPPING; // If walk complete return leaf's value slr 12 bits == ppn
}