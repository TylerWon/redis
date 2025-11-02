#define TEST_MODE

#include <assert.h>

#include "../HMap.hpp"
#include "../../utils/intrusive_data_structure_utils.hpp"
#include <vector>

struct Data {
    HNode node;
    int val;

    Data(uint64_t hval, int val) {
        node.hval = hval;
        this->val = val;
    }
};

/**
 * Callback which checks if two Data in an HTable are equal.
 * 
 * @param node1 The HNode contained by the first Data.
 * @param node2 The HNode contained by the second Data.
 * 
 * @return  True if Data are equal.
 *          False otherwise.
 */
bool are_data_equal(HNode *node1, HNode *node2) {
    Data *data1 = container_of(node1, Data, node);
    Data *data2 = container_of(node2, Data, node);
    return data1->val == data2->val;
}

/**
 * Callback which adds the value of a Data in an HMap to the given arg.
 * 
 * @param node  The HNode contained by Data.
 * @param arg   Void pointer to an integer.
 */
void add(HNode *node, void *arg) {
    Data *data = container_of(node, Data, node);
    int *sum = (int *) arg;
    *sum += data->val;
}

/**
 * Checks that a node is in the map's new table.
 * 
 * @param map   Pointer to the map.
 * @param node  Pointer to the node.
 */
void check_node_in_new_table(HMap *map, HNode *node) {
    HNode **from = map->get_newer()->lookup(node, are_data_equal);
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
    HNode **from = map->get_older()->lookup(node, are_data_equal);
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
    node = map->lookup(node, are_data_equal);
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
        Data *data = new Data(i, i);
        map->insert(&data->node);
    }

    assert(map->get_newer()->num_keys == 1);
    assert(map->get_older()->num_keys == 7);

    return map;
}

void test_constructor() {
    HMap map;
    assert(map.get_newer()->num_slots == 8);
    assert(map.get_older() == NULL);
    assert(map.get_migrate_pos() == 0);
}

void test_insert_node() {
    HMap map;
    Data data(0, 1);
    map.insert(&data.node);

    assert(map.length() == 1);
    assert(map.get_newer()->num_keys == 1);
    check_node_in_new_table(&map, &data.node);
}

void test_insert_triggers_resize() {

}

void test_insert_in_the_middle_of_rehashing() {
    HMap *map = create_map_in_the_middle_of_rehashing();

    // new node should be inserted into the new table even if old one exists
    Data data(10, -13);
    map->insert(&data.node);
    check_node_in_new_table(map, &data.node);
}

void test_lookup_on_empty_map() {
    HMap map;
    Data key(7, 2);
    check_node_not_in_map(&map, &key.node);
}

void test_lookup_non_existent_node() {
    HMap map;
    Data data1(7, 2);
    map.insert(&data1.node);

    Data key(14, 1);
    check_node_not_in_map(&map, &key.node);
}

void test_lookup_node() {
    HMap map;
    Data data(7, 2);
    map.insert(&data.node);
    check_node_in_new_table(&map, &data.node);
}

void test_lookup_in_the_middle_of_rehashing() {
    HMap *map = create_map_in_the_middle_of_rehashing();

    // look-up a node still in the old table
    Data key1(7, 7);
    HNode *node = map->lookup(&key1.node, are_data_equal);
    check_node_in_old_table(map, node);

    // look-up a node that was moved to the new table
    Data key2(0, 0);
    node = map->lookup(&key2.node, are_data_equal);
    check_node_in_new_table(map, node);
}

void test_remove_on_empty_map() {
    HMap map;
    Data key(10, 10);
    HNode *node = map.remove(&key.node, are_data_equal);
    assert(map.length() == 0);
    assert(node == NULL);
}

void test_remove_non_existent_node() {
    HMap map;
    Data data1(5, 21);
    map.insert(&data1.node);

    Data key(4, 0);
    HNode *node = map.remove(&key.node, are_data_equal);
    assert(map.length() == 1);
    assert(node == NULL);
}

void test_remove_node() {
    HMap map;
    Data data(2, 37);
    map.insert(&data.node);

    Data key(2, 37);
    HNode *node = map.remove(&key.node, are_data_equal);
    assert(map.length() == 0);
    check_node_not_in_map(&map, node);
}

void test_remove_in_the_middle_of_rehashing() {
    HMap *map = create_map_in_the_middle_of_rehashing();

    // remove a node still in the old table
    Data key1(7, 7);
    HNode *node = map->lookup(&key1.node, are_data_equal);
    check_node_in_old_table(map, node);
    map->remove(node, are_data_equal);
    check_node_not_in_map(map, node);

    // remove a node that was moved to the new table
    Data key2(0, 0);
    node = map->lookup(&key2.node, are_data_equal);
    check_node_in_new_table(map, node);
    map->remove(node, are_data_equal);
    check_node_not_in_map(map, node);
}

void test_for_each() {
    HMap map;
    Data data1(0, 4);
    Data data2(6, 2);
    Data data3(6, 9);
    map.insert(&data1.node);
    map.insert(&data2.node);
    map.insert(&data3.node);

    int sum = 0;
    map.for_each(add, (void *) &sum);
    assert(sum == 4 + 2 + 9);
}

void test_for_each_in_the_middle_of_rehashing() {
    HMap *map = create_map_in_the_middle_of_rehashing();
    
    int sum = 0;
    map->for_each(add, (void *) &sum);
    assert(sum == 0 + 1 + 2 + 3 + 4 + 5 + 6 + 7);
}

void test_multi_step_rehash() {
    HMap *map = create_map_in_the_middle_of_rehashing();

    // perform 7 operations to complete rehash
    for (uint32_t i = 0; i < 7; i++) {
        Data data(i, i);
        map->lookup(&data.node, are_data_equal);
    }

    assert(map->get_older() == NULL);
    assert(map->get_newer()->num_keys == 8);
}

int main() {
    test_constructor();

    test_insert_node();
    test_insert_triggers_resize();
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
