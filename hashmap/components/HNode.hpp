#pragma once

#include <cstddef>
#include <cstdint>

/* A node in a HTable */
struct HNode {
    HNode *next = NULL;
    uint64_t hval = 0; // hash value
};
