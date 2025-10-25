#pragma once

#include <cstddef>
#include <cstdint>

/* A linked-list node for a hashtable */
struct HNode {
    HNode *next = NULL;
    uint64_t hval = 0; // hash value
};
