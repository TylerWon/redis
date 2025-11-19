#define TEST_MODE

#include <assert.h>

#include "../MinHeap.hpp"
#include "../../utils/intrusive_data_structure_utils.hpp"

struct Item {
    int64_t val;
    MHNode node;

    Item(int64_t val) {
        this->val = val;
    } 
};

/**
 * Callback which checks if one Item is less than another in a MinHeap.
 * 
 * @param node1 The MHNode contained by the first Item.
 * @param node2 The MHNode contained by the second Item.
 * 
 * @return  True if the first Item is less than the second Item.
 *          False otherwise.
 */
bool is_item_less(MHNode *node1, MHNode *node2) {
    Item *item1 = container_of(node1, Item, node);
    Item *item2 = container_of(node2, Item, node);
    return item1->val < item2->val;
}

/**
 * Checks if the heap contains the expected values in the correct order.
 * 
 * @param heap      The heap to check.
 * @param expected  Vector containing the values expected in the heap.
 */
void check_heap(MinHeap *heap, std::vector<int64_t> expected) {
    std::vector<MHNode *> nodes = heap->get_nodes();
    assert(nodes.size() == expected.size());
    for (uint32_t i = 0; i < nodes.size(); i++) {
        MHNode *node = nodes[i];
        assert(node->pos == i);
        
        int64_t val = container_of(node, Item, node)->val;
        int64_t expected_val = expected[i];
        assert(val == expected_val);
    }
}

void test_insert_empty_heap() {
    MinHeap heap;
    Item item(1);
    heap.insert(&item.node, is_item_less);
    check_heap(&heap, {1});
}

void test_insert_then_heapify_up() {
    MinHeap heap;
    Item seven(7);
    heap.insert(&seven.node, is_item_less);
    Item ten(10);
    heap.insert(&ten.node, is_item_less);
    Item fourteen(14);
    heap.insert(&fourteen.node, is_item_less);
    check_heap(&heap, {7, 10, 14});

    Item five(5);
    heap.insert(&five.node, is_item_less);
    check_heap(&heap, {5, 7, 14, 10});
}

void test_remove_only_node() {
    MinHeap heap;
    Item item(1);
    heap.insert(&item.node, is_item_less);
    check_heap(&heap, {1});

    heap.remove(&item.node, is_item_less);
    assert(heap.is_empty() == true);
}

void test_remove_then_heapify_up() {
    MinHeap heap;
    Item one(1);
    heap.insert(&one.node, is_item_less);
    Item ten(10);
    heap.insert(&ten.node, is_item_less);
    Item five(5);
    heap.insert(&five.node, is_item_less);
    Item twenty(20);
    heap.insert(&twenty.node, is_item_less);
    Item seventeen(17);
    heap.insert(&seventeen.node, is_item_less);
    Item eight(8);
    heap.insert(&eight.node, is_item_less);
    check_heap(&heap, {1, 10, 5, 20, 17, 8});

    heap.remove(&twenty.node, is_item_less);
    check_heap(&heap, {1, 8, 5, 10, 17});
}

void test_remove_then_heapify_down() {
    MinHeap heap;
    Item four(4);
    heap.insert(&four.node, is_item_less);
    Item nine(9);
    heap.insert(&nine.node, is_item_less);
    Item seven(7);
    heap.insert(&seven.node, is_item_less);
    Item fifteen(15);
    heap.insert(&fifteen.node, is_item_less);
    Item fourteen(14);
    heap.insert(&fourteen.node, is_item_less);
    Item nineteen(19);
    heap.insert(&nineteen.node, is_item_less);
    Item twenty(20);
    heap.insert(&twenty.node, is_item_less);
    check_heap(&heap, {4, 9, 7, 15, 14, 19, 20});

    heap.remove(&four.node, is_item_less);
    check_heap(&heap, {7, 9, 19, 15, 14, 20});
}

void test_min_empty_heap() {
    MinHeap heap;
    MHNode *node = heap.min();
    assert(node == NULL);
}

void test_min_non_empty_heap() {
    MinHeap heap;
    Item four(4);
    heap.insert(&four.node, is_item_less);
    Item nine(9);
    heap.insert(&nine.node, is_item_less);
    Item seven(7);
    heap.insert(&seven.node, is_item_less);

    MHNode *node = heap.min();
    assert(node == &four.node);
}

void test_is_empty_empty_heap() {
    MinHeap heap;
    assert(heap.is_empty() == true);
}

void test_is_empty_non_empty_heap() {
    MinHeap heap;
    Item four(4);
    heap.insert(&four.node, is_item_less);
    assert(heap.is_empty() == false);
}

int main() {
    test_insert_empty_heap();
    test_insert_then_heapify_up();

    test_remove_only_node();
    test_remove_then_heapify_up();
    test_remove_then_heapify_down();

    test_min_empty_heap();
    test_min_non_empty_heap();

    test_is_empty_empty_heap();
    test_is_empty_non_empty_heap();

    return 0;
}
