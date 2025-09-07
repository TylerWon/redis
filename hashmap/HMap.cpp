#include <stdlib.h>
#include <assert.h>

#include "HMap.hpp"

HMap::HTable::HTable(size_t n) {
    assert(n > 0 && ((n - 1) & n) == 0);

    table = (HNode **) calloc(n, sizeof(HNode *)); 
    num_slots = n;
    num_keys = 0;
    mask = n - 1;
}

void HMap::HTable::insert(HNode *node) {
    uint64_t slot = node->hval & mask;
    HNode *head = table[slot];
    node->next = head;
    table[slot] = node;
    num_keys++;
}

HNode **HMap::HTable::lookup(HNode *key, bool (*eq)(HNode *, HNode *)) {
    uint64_t slot = key->hval & mask;
    HNode **from = &table[slot];
    for (HNode *curr = *from; curr != NULL; from = &curr->next) {
        if (key->hval == curr->hval && eq(key, curr)) {
            return from;
        }
    }
    return NULL;
}

HNode *HMap::HTable::detach(HNode **from) {
    HNode *node = *from;
    *from = node->next;
    num_keys--;
    return node;
}

HMap::HMap() {
    newer = new HTable(8);
    older = NULL;
    migrate_pos = 0;
}

void HMap::insert(HNode *node) {
    newer->insert(node);

    if (older == NULL && newer->num_keys >= newer->num_slots * MAX_LOAD_FACTOR) {
        resize();
    }

    migrate_keys();
}

HNode *HMap::lookup(HNode *key, bool (*eq)(HNode *, HNode *)) {
    migrate_keys();

    HNode **from = newer->lookup(key, eq);
    if (from == NULL && older != NULL) {
        from = older->lookup(key, eq);
    }

    return from != NULL ? *from : NULL;
}

HNode *HMap::remove(HNode *key, bool (*eq)(HNode *, HNode *)) {
    migrate_keys();

    HNode **from;
    if ((from = newer->lookup(key, eq)) != NULL) {
        return newer->detach(from);
    }

    if (older != NULL && (from = older->lookup(key, eq)) != NULL) {
        return older->detach(from);
    }

    return NULL;
}

void HMap::migrate_keys() {
    if (older == NULL) {
        return;
    }

    uint64_t keys_migrated = 0;
    while (keys_migrated < NUM_KEYS_TO_MIGRATE && older->num_keys > 0) {
        HNode **from = &older->table[migrate_pos];
        if (*from == NULL) {
            migrate_pos++;
            continue;
        }
        newer->insert(older->detach(from));
        keys_migrated++;
    }

    if (older->num_keys == 0) {
        free(older->table);
        delete older;
        older = NULL;
    }
}

void HMap::resize() {
    older = newer;
    newer = new HTable(2 * older->num_slots);
    migrate_pos = 0;
}
