#pragma once

#include <cstddef>
#include <cstdint>

/* A linked-list node for a hashtable */
typedef struct HNode {
    HNode *next = NULL;
    uint64_t hval = 0; // hash value
} HNode;

/* Returns a pointer to the container of an HNode */
#define container_of(ptr, T, member) \
    ((T *)((char *)ptr - offsetof(T, member)))
