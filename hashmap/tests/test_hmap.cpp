#define TEST_MODE

#include <assert.h>

#include "../HMap.hpp"
#include "../../utils/intrusive_data_structure_utils.hpp"
#include <vector>

const uint8_t INITIAL_SIZE = HMap::get_initial_size();
const uint8_t MAX_LOAD_FACTOR = HMap::get_max_load_factor();
const uint8_t NUM_KEYS_TO_MIGRATE = HMap::get_num_keys_to_migrate();

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
 * Callback which multiples the value of a Data in an HMap by the given arg.
 * 
 * @param node  The HNode contained by Data.
 * @param arg   Void pointer to an integer multiplier.
 */
void multiply(HNode *node, void *arg) {
    Data *data = container_of(node, Data, node);
    int multipler = *((int *) arg);
    data->val *= multipler;
}

/**
 * Checks that a node is in the map's new table.
 * 
 * @param map   Pointer to the map.
 * @param node  Pointer to the node to check for.
 */
void check_node_in_new_table(HMap *map, HNode *node) {
    HNode **from = map->get_newer()->lookup(node, are_data_equal);
    assert(from != NULL);
    assert(*from == node);
}

/**
 * Checks that a node is not in the map.
 * 
 * @param map   Pointer to the HMap.
 * @param node  Pointer to the node to check for.
 */
void check_node_not_in_map(HMap *map, HNode *node) {
    node = map->lookup(node, are_data_equal);
    assert(node == NULL);
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
    // check new node inserted into new table even when old table exists
}

void test_lookup_on_empty_map() {
    HMap map;
    Data data(7, 2);
    check_node_not_in_map(&map, &data.node);
}

void test_lookup_non_existent_node() {
    HMap map;
    Data data1(7, 2);
    map.insert(&data1.node);

    Data data2(14, 1);
    check_node_not_in_map(&map, &data2.node);
}

void test_lookup_node() {
    HMap map;
    Data data(7, 2);
    map.insert(&data.node);
    check_node_in_new_table(&map, &data.node);
}

void test_lookup_in_the_middle_of_rehashing() {
    // look-up a node in old table
    // look-up a node in new table
}

void test_remove_on_empty_map() {
    HMap map;
    Data data(10, 10);
    HNode *node = map.remove(&data.node, are_data_equal);
    assert(node == NULL);
    assert(map.length() == 0);
}

void test_remove_non_existent_node() {
    HMap map;
    Data data1(5, 21);
    map.insert(&data1.node);

    Data data2(4, 0);
    HNode *node = map.remove(&data2.node, are_data_equal);
    assert(node == NULL);
    assert(map.length() == 1);
}

void test_remove_node() {
    HMap map;
    Data data(2, 37);
    map.insert(&data.node);

    HNode *node = map.remove(&data.node, are_data_equal);
    assert(node == &data.node);
    assert(map.length() == 0);
}

void test_remove_in_the_middle_of_rehashing() {
    // remove node in old table
    // remove node in new table
}

void test_for_each() {
    HMap map;
    Data data1(0, 4);
    Data data2(6, 2);
    Data data3(6, 9);
    map.insert(&data1.node);
    map.insert(&data2.node);
    map.insert(&data3.node);

    int multiplier = 2;
    map.for_each(multiply, (void *) &multiplier);
    assert(data1.val == 4 * multiplier);
    assert(data2.val == 2 * multiplier);
    assert(data3.val == 9 * multiplier);
}

void test_for_each_in_the_middle_of_rehashing() {
    
}

void test_perform_operations_until_rehashing_finishes() {

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

    test_perform_operations_until_rehashing_finishes();

    return 0;
}
