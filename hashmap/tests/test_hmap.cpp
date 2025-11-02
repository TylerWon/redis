#define TEST_MODE

#include <assert.h>

#include "../HMap.hpp"
#include "../../utils/intrusive_data_structure_utils.hpp"
#include <vector>

struct Item {
    HNode node;
    int val;

    Item(uint64_t hval, int val) {
        node.hval = hval;
        this->val = val;
    }
};

/**
 * Callback which checks if two Items in an HTable are equal.
 * 
 * @param node1 The HNode contained by the first Item.
 * @param node2 The HNode contained by the second Item.
 * 
 * @return  True if Items are equal.
 *          False otherwise.
 */
bool are_items_equal(HNode *node1, HNode *node2) {
    Item *item1 = container_of(node1, Item, node);
    Item *item2 = container_of(node2, Item, node);
    return item1->val == item2->val;
}

/**
 * Callback which adds the value of an Item in an HMap to the given accumulator.
 * 
 * @param node  The HNode contained by the Item.
 * @param arg   Void pointer to an integer accumulator.
 */
void add(HNode *node, void *arg) {
    Item *item = container_of(node, Item, node);
    int *sum = (int *) arg;
    *sum += item->val;
}

/**
 * Callback which adds the address of an Item in an HMap to the given vector.
 * 
 * @param node  The HNode contained by the Item.
 * @param arg   Void pointer to the vector.
 */
void collect_items(HNode *node, void *arg) {
    Item *item = container_of(node, Item, node);
    std::vector<Item *> *items = (std::vector<Item *> *) arg;
    items->push_back(item);
}

/**
 * Checks that a node is in the map's new table.
 * 
 * @param map   Pointer to the map.
 * @param node  Pointer to the node.
 */
void check_node_in_new_table(HMap *map, HNode *node) {
    HNode **from = map->get_newer()->lookup(node, are_items_equal);
    assert(from != NULL);
    assert(*from == node);
}

/**
 * Checks that a node is in the map's old table.
 * 
 * @param map   Pointer to the map.
 * @param node  Pointer to the node.
 */
void check_node_in_old_table(HMap *map, HNode *node) {
    HNode **from = map->get_older()->lookup(node, are_items_equal);
    assert(from != NULL);
    assert(*from == node);
}

/**
 * Checks that a node is not in the map.
 * 
 * @param map   Pointer to the HMap.
 * @param node  Pointer to the node.
 */
void check_node_not_in_map(HMap *map, HNode *node) {
    node = map->lookup(node, are_items_equal);
    assert(node == NULL);
}

/**
 * Creates a map which is in the middle of rehashing its keys (i.e. some keys in the old table while some are in the new
 * table).
 * 
 * Changes a few properties of the map to make testing easier:
 * - Sets the max load factor of the map to 1 so that the first resize will happen once there are 8 keys in the map.
 * - Sets the number of keys the map rehashes to 1 so that it takes multiple operations for all keys to be rehashed.
 * 
 * @return  Pointer to a map in the middle of rehashing. Map has 1 key in the new table and 7 in the old table.
 */
HMap *create_map_in_the_middle_of_rehashing() {
    HMap *map = new HMap();
    map->set_max_load_factor(1);
    map->set_num_keys_to_rehash(1);

    for (uint32_t i = 0; i < 8; i++) {
        Item *item = new Item(i, i);
        map->insert(&item->node);
    }

    assert(map->get_newer()->num_keys == 1);
    assert(map->get_older()->num_keys == 7);

    return map;
}

/**
 * Deletes (deallocates) all Items in the map then deletes the map itself.
 * 
 * @param map   Pointer to the map.
 */
void clean_up_map(HMap *map) {
    std::vector<Item *> items;
    map->for_each(collect_items, (void *) &items);
    for (Item *item : items) {
        delete item;
    }
    
    delete map;
}

void test_constructor() {
    HMap *map = new HMap();
    assert(map->get_newer()->num_slots == 8);
    assert(map->get_older() == NULL);
    assert(map->get_migrate_pos() == 0);
    
    clean_up_map(map);
}

void test_insert_node() {
    HMap *map = new HMap();
    Item *item = new Item(0, 1);
    map->insert(&item->node);

    assert(map->length() == 1);
    assert(map->get_newer()->num_keys == 1);
    check_node_in_new_table(map, &item->node);

    clean_up_map(map);
}

void test_insert_in_the_middle_of_rehashing() {
    HMap *map = create_map_in_the_middle_of_rehashing();

    // new node should be inserted into the new table even if old one exists
    Item *item = new Item(10, -13);
    map->insert(&item->node);
    check_node_in_new_table(map, &item->node);

    clean_up_map(map);
}

void test_lookup_on_empty_map() {
    HMap *map = new HMap();
    Item key(7, 2);
    check_node_not_in_map(map, &key.node);

    clean_up_map(map);
}

void test_lookup_non_existent_node() {
    HMap *map = new HMap();
    Item *item = new Item(7, 2);
    map->insert(&item->node);

    Item key(14, 1);
    check_node_not_in_map(map, &key.node);

    clean_up_map(map);
}

void test_lookup_node() {
    HMap *map = new HMap();
    Item *item = new Item(7, 2);
    map->insert(&item->node);

    check_node_in_new_table(map, &item->node);

    clean_up_map(map);
}

void test_lookup_in_the_middle_of_rehashing() {
    HMap *map = create_map_in_the_middle_of_rehashing();

    // look-up a node still in the old table
    Item key1(7, 7);
    HNode *node = map->lookup(&key1.node, are_items_equal);
    check_node_in_old_table(map, node);

    // look-up a node that was moved to the new table
    Item key2(0, 0);
    node = map->lookup(&key2.node, are_items_equal);
    check_node_in_new_table(map, node);

    clean_up_map(map);
}

void test_remove_on_empty_map() {
    HMap *map = new HMap();
    
    Item key(10, 10);
    HNode *node = map->remove(&key.node, are_items_equal);
    assert(map->length() == 0);
    assert(node == NULL);

    clean_up_map(map);
}

void test_remove_non_existent_node() {
    HMap *map = new HMap();
    Item *item = new Item(5, 21);
    map->insert(&item->node);

    Item key(4, 0);
    HNode *node = map->remove(&key.node, are_items_equal);
    assert(map->length() == 1);
    assert(node == NULL);

    clean_up_map(map);
}

void test_remove_node() {
    HMap *map = new HMap();
    Item *item = new Item(2, 37);
    map->insert(&item->node);

    Item key(2, 37);
    HNode *node = map->remove(&key.node, are_items_equal);
    assert(map->length() == 0);
    check_node_not_in_map(map, node);

    clean_up_map(map);
}

void test_remove_in_the_middle_of_rehashing() {
    HMap *map = create_map_in_the_middle_of_rehashing();

    // remove a node still in the old table
    Item key1(7, 7);
    HNode *node = map->lookup(&key1.node, are_items_equal);
    check_node_in_old_table(map, node);
    map->remove(node, are_items_equal);
    check_node_not_in_map(map, node);

    // remove a node that was moved to the new table
    Item key2(0, 0);
    node = map->lookup(&key2.node, are_items_equal);
    check_node_in_new_table(map, node);
    map->remove(node, are_items_equal);
    check_node_not_in_map(map, node);

    clean_up_map(map);
}

void test_for_each() {
    HMap *map = new HMap();
    Item *item1 = new Item(0, 4);
    Item *item2 = new Item(6, 2);
    Item *item3 = new Item(6, 9);
    map->insert(&item1->node);
    map->insert(&item2->node);
    map->insert(&item3->node);

    int sum = 0;
    map->for_each(add, (void *) &sum);
    assert(sum == 4 + 2 + 9);

    clean_up_map(map);
}

void test_for_each_in_the_middle_of_rehashing() {
    HMap *map = create_map_in_the_middle_of_rehashing();
    
    int sum = 0;
    map->for_each(add, (void *) &sum);
    assert(sum == 0 + 1 + 2 + 3 + 4 + 5 + 6 + 7);
 
    clean_up_map(map);
}

void test_multi_step_rehash() {
    HMap *map = create_map_in_the_middle_of_rehashing();

    // perform 7 operations to complete rehash
    for (uint32_t i = 0; i < 7; i++) {
        Item key(i, i);
        map->lookup(&key.node, are_items_equal);
    }

    assert(map->get_older() == NULL);
    assert(map->get_newer()->num_keys == 8);

    clean_up_map(map);
}

int main() {
    test_constructor();

    test_insert_node();
    test_insert_in_the_middle_of_rehashing();

    test_lookup_on_empty_map();
    test_lookup_non_existent_node();
    test_lookup_node();
    test_lookup_in_the_middle_of_rehashing();

    test_remove_on_empty_map();
    test_remove_non_existent_node();
    test_remove_node();
    test_remove_in_the_middle_of_rehashing();

    test_for_each();
    test_for_each_in_the_middle_of_rehashing();

    test_multi_step_rehash();

    return 0;
}
