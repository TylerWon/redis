#include <assert.h>

#include "../HTable.hpp"
#include "../../../utils/intrusive_data_structure_utils.hpp"

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
 * Callback which multiples the value of an Item in an HTable by the given multiplier.
 * 
 * @param node  The HNode contained by the Item.
 * @param arg   Void pointer to an integer multiplier.
 */
void multiply(HNode *node, void *arg) {
    Item *item = container_of(node, Item, node);
    int multipler = *((int *) arg);
    item->val *= multipler;
}

void test_constructor() {
    HTable table(8);
    assert(table.num_slots == 8);
    assert(table.num_keys == 0);
}

void test_insert_node() {
    HTable table(8);
    Item item(0, 0);
    
    table.insert(&item.node);
    assert(table.num_keys == 1);
    assert(table.table[0] == &item.node);
    assert(item.node.next == NULL);
}

void test_insert_nodes_into_same_slot() {
    HTable table(8);
    Item item1(7, 0);
    Item item2(7, 1);

    table.insert(&item1.node);
    table.insert(&item2.node);
    assert(table.num_keys == 2);
    assert(table.table[7] == &item2.node);
    assert(item2.node.next == &item1.node);
    assert(item1.node.next == NULL);
}

void test_insert_node_with_hash_larger_than_number_of_slots() {
    HTable table(8);
    Item item(20, 0);
    
    table.insert(&item.node);
    assert(table.num_keys == 1);
    assert(table.table[4] == &item.node);
    assert(item.node.next == NULL);
}

void test_lookup_on_empty_table() {
    HTable table(8);

    Item item(3, 0);
    HNode **from = table.lookup(&item.node, are_items_equal);
    assert(from == NULL);
}

void test_lookup_non_existent_node() {
    HTable table(8);
    Item item1(3, 0);
    table.insert(&item1.node);

    Item item2(4, 1);
    HNode **from = table.lookup(&item2.node, are_items_equal);
    assert(from == NULL);
}

void test_lookup_node_at_start_of_chain() {
    HTable table(8);
    Item item(3, 0);
    table.insert(&item.node);

    HNode **from = table.lookup(&item.node, are_items_equal);
    assert(from != NULL);
    assert(*from == &item.node);
}

void test_lookup_node_in_chain() {
    HTable table(8);
    Item item1(3, 0);
    Item item2(3, 5);
    table.insert(&item1.node);
    table.insert(&item2.node);

    HNode **from = table.lookup(&item1.node, are_items_equal);
    assert(from != NULL);
    assert(*from == &item1.node);
}

void test_detach_node_at_start_of_chain() {
    HTable table(8);
    Item item1(1, 0);
    Item item2(1, 2);
    table.insert(&item1.node);
    table.insert(&item2.node);

    HNode **from = table.lookup(&item2.node, are_items_equal);
    HNode *node = table.detach(from);
    assert(table.num_keys == 1);
    assert(node == &item2.node);
    assert(table.table[1] == &item1.node);
    assert(item1.node.next == NULL);
}

void test_detach_node_in_chain() {
    HTable table(8);
    Item item1(6, 2);
    Item item2(6, 7);
    table.insert(&item1.node);
    table.insert(&item2.node);

    HNode **from = table.lookup(&item1.node, are_items_equal);
    HNode *node = table.detach(from);
    assert(table.num_keys == 1);
    assert(node == &item1.node);
    assert(table.table[6] == &item2.node);
    assert(item2.node.next == NULL);
}

void test_detach_only_node_in_chain() {
    HTable table(8);
    Item item(4, 11);
    table.insert(&item.node);

    HNode **from = table.lookup(&item.node, are_items_equal);
    HNode *node = table.detach(from);
    assert(table.num_keys == 0);
    assert(node == &item.node);
    assert(table.table[4] == NULL);
}

void test_for_each() {
    HTable table(8);
    Item item1(0, 4);
    Item item2(6, 2);
    Item item3(6, 9);
    table.insert(&item1.node);
    table.insert(&item2.node);
    table.insert(&item3.node);

    int multiplier = 2;
    table.for_each(multiply, (void *) &multiplier);
    assert(item1.val == 4 * multiplier);
    assert(item2.val == 2 * multiplier);
    assert(item3.val == 9 * multiplier);
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
