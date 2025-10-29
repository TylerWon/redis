#include <cstring>

#include "SortedSet.hpp"
#include "../utils/intrusive_data_structure_utils.hpp"
#include "../utils/hash_utils.hpp"


/**
 * Callback which checks if a LookupSPair and SPair in an HMap are equal.
 * 
 * @param node1 The HNode contained by the LookupSPair.
 * @param node2 The HNode contained by the SPair.
 * 
 * @return  True if the pairs are equal.
 *          False if not. 
 */
bool are_pairs_equal(HNode *node1, HNode *node2) {
    LookupSPair *pair1 = container_of(node1, LookupSPair, node);
    SPair *pair2 = container_of(node2, SPair, map_node);
    if (pair1->len != pair2->len) {
        return false;
    }
    return 0 == memcmp(pair1->name, pair2->name, pair1->len);
}

/**
 * Callback which compares two SPairs in an AVLTree.
 * 
 * @param node1 The AVLNode contained by the first pair.
 * @param node2 The AVLNode contained by the second pair.
 * 
 * @return  < 0 if first pair < second pair
 *          > 0 if first pair > second pair
 *          0 if the two are equal
 */
int32_t compare_pairs(AVLNode *node1, AVLNode *node2) {
    SPair *pair1 = container_of(node1, SPair, tree_node);
    SPair *pair2 = container_of(node2, SPair, tree_node);
    if (pair1->score != pair2->score) {
        return pair1->score - pair2->score;
    }
    int res = strncmp(pair1->name, pair2->name, std::min(pair1->len, pair2->len));
    return res != 0 ? res : pair1->len - pair2->len;
}

bool SortedSet::insert(const char *name, uint32_t len, double score) {
    SPair *pair = lookup(name, len);
    if (pair != NULL) {
        update(pair, score);
        return false;
    }
    pair = spair_new(name, len, score);
    map.insert(&pair->map_node);
    tree.insert(&pair->tree_node, compare_pairs);
    return true;
}

SPair *SortedSet::lookup(const char *name, uint32_t len) {
    LookupSPair lookup_pair;
    lookup_pair.node.hval = str_hash(name, len);
    lookup_pair.name = name;
    lookup_pair.len = len;
    HNode *map_node = map.lookup(&lookup_pair.node, are_pairs_equal);
    return map_node != NULL ? container_of(map_node, SPair, map_node) : NULL;
}

void SortedSet::remove(const char *name, uint32_t len) {
    LookupSPair lookup_pair;
    lookup_pair.node.hval = str_hash(name, len);
    lookup_pair.name = name;
    lookup_pair.len = len;
    HNode *map_node = map.remove(&lookup_pair.node, are_pairs_equal);
    SPair *pair = container_of(map_node, SPair, map_node);
    tree.remove(&pair->tree_node);
    spair_del(pair);
}

void SortedSet::update(SPair *pair, double score) {
    // detach pair from AVLTree
    tree.root = tree.remove(&pair->tree_node);

    // re-insert to fix order
    pair->tree_node = AVLNode(); // reset node data
    pair->score = score;
    tree.insert(&pair->tree_node, compare_pairs);
}
