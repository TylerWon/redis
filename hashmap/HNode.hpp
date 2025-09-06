#include <cstdint>
#include <cstring>

typedef struct HNode {
    HNode *next = NULL;
    uint64_t hval = 0; // hash value
} HNode;
