#include <assert.h>

#include "../HTable.hpp"
#include "../../../utils/intrusive_data_structure_utils.hpp"

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
 * Callback which multiples the value of a Data in an HTable by the given arg.
 * 
 * @param node  The HNode contained by Data.
 * @param arg   Void pointer to an integer multiplier.
 */
void multiply(HNode *node, void *arg) {
    Data *data = container_of(node, Data, node);
    int multipler = *((int *) arg);
    data->val *= multipler;
}

void test_constructor() {
    HTable table(8);
    assert(table.num_slots == 8);
    assert(table.num_keys == 0);
}

void test_insert_node() {
    HTable table(8);
    Data data(0, 0);
    
    table.insert(&data.node);
    assert(table.num_keys == 1);
    assert(table.table[0] == &data.node);
    assert(data.node.next == NULL);
}

void test_insert_nodes_into_same_slot() {
    HTable table(8);
    Data data1 (7, 0);
    Data data2 (7, 1);

    table.insert(&data1.node);
    table.insert(&data2.node);
    assert(table.num_keys == 2);
    assert(table.table[7] == &data2.node);
    assert(data2.node.next == &data1.node);
    assert(data1.node.next == NULL);
}

void test_insert_node_with_hash_larger_than_number_of_slots() {
    HTable table(8);
    Data data(20, 0);
    
    table.insert(&data.node);
    assert(table.num_keys == 1);
    assert(table.table[4] == &data.node);
    assert(data.node.next == NULL);
}

void test_lookup_on_empty_table() {
    HTable table(8);

    Data data (3, 0);
    HNode **from = table.lookup(&data.node, are_data_equal);
    assert(from == NULL);
}

void test_lookup_non_existent_node() {
    HTable table(8);
    Data data1 (3, 0);
    table.insert(&data1.node);

    Data data2 (4, 1);
    HNode **from = table.lookup(&data2.node, are_data_equal);
    assert(from == NULL);
}

void test_lookup_node_at_start_of_chain() {
    HTable table(8);
    Data data (3, 0);
    table.insert(&data.node);

    HNode **from = table.lookup(&data.node, are_data_equal);
    assert(from != NULL);
    assert(*from == &data.node);
}

void test_lookup_node_in_chain() {
    HTable table(8);
    Data data1 (3, 0);
    Data data2 (3, 5);
    table.insert(&data1.node);
    table.insert(&data2.node);

    HNode **from = table.lookup(&data1.node, are_data_equal);
    assert(from != NULL);
    assert(*from == &data1.node);
}

void test_detach_node_at_start_of_chain() {
    HTable table(8);
    Data data1 (1, 0);
    Data data2 (1, 2);
    table.insert(&data1.node);
    table.insert(&data2.node);

    HNode **from = table.lookup(&data2.node, are_data_equal);
    HNode *node = table.detach(from);
    assert(table.num_keys == 1);
    assert(node == &data2.node);
    assert(table.table[1] == &data1.node);
    assert(data1.node.next == NULL);
}

void test_detach_node_in_chain() {
    HTable table(8);
    Data data1 (6, 2);
    Data data2 (6, 7);
    table.insert(&data1.node);
    table.insert(&data2.node);

    HNode **from = table.lookup(&data1.node, are_data_equal);
    HNode *node = table.detach(from);
    assert(table.num_keys == 1);
    assert(node == &data1.node);
    assert(table.table[6] == &data2.node);
    assert(data2.node.next == NULL);
}

void test_detach_only_node_in_chain() {
    HTable table(8);
    Data data (4, 11);
    table.insert(&data.node);

    HNode **from = table.lookup(&data.node, are_data_equal);
    HNode *node = table.detach(from);
    assert(table.num_keys == 0);
    assert(node == &data.node);
    assert(table.table[4] == NULL);
}

void test_for_each() {
    HTable table(8);
    Data data1 (0, 4);
    Data data2 (6, 2);
    Data data3 (6, 9);
    table.insert(&data1.node);
    table.insert(&data2.node);
    table.insert(&data3.node);

    int multiplier = 2;
    table.for_each(multiply, (void *) &multiplier);
    assert(data1.val == 4 * multiplier);
    assert(data2.val == 2 * multiplier);
    assert(data3.val == 9 * multiplier);
}

int main() {
    test_constructor();

    test_insert_node();
    test_insert_nodes_into_same_slot();
    test_insert_node_with_hash_larger_than_number_of_slots();

    test_lookup_on_empty_table();
    test_lookup_non_existent_node();
    test_lookup_node_at_start_of_chain();
    test_lookup_node_in_chain();

    test_detach_node_at_start_of_chain();
    test_detach_node_in_chain();
    test_detach_only_node_in_chain();

    test_for_each();

    return 0;
}
