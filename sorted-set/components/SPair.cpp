#include <stdlib.h>
#include <cstring>

#include "SPair.hpp"
#include "../../utils/hash_utils.hpp"

SPair *spair_new(const char *name, uint32_t len, double score) {
    SPair *pair = (SPair *) malloc(sizeof(SPair) + len);
    pair->map_node.hval = str_hash(name, len);
    pair->score = score;
    pair->len = len;
    memcpy(&pair->name, name, len);
    return pair;
}

void spair_del(SPair *pair) {
    free(pair);
}
