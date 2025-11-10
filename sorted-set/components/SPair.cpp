#include <stdlib.h>
#include <cstring>

#include "SPair.hpp"
#include "../../utils/hash_utils.hpp"

SPair *spair_new(const char *name, uint32_t len, double score) {
    SPair *pair = (SPair *) malloc(sizeof(SPair) + len);
    // explicitly initialize all members because memory allocated by malloc is not initialized
    pair->map_node = HNode(); 
    pair->map_node.hval = str_hash(name, len);
    pair->tree_node = AVLNode(); 
    pair->score = score;
    pair->len = len;
    memcpy(&pair->name, name, len);
    return pair;
}

void spair_del(SPair *pair) {
    free(pair);
}
