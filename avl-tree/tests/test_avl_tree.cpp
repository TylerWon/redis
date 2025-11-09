#include <vector>
#include <queue>
#include <cassert>
#include <list>

#include "../AVLTree.hpp"
#include "../../utils/intrusive_data_structure_utils.hpp"

struct Item {
    AVLNode node;
    uint32_t key;

    Item(uint32_t key) {
        this->key = key;
    }
};

/**
 * Compares two Items in an AVLTree.
 * 
 * @param node1 The AVLNode contained by the first Item.
 * @param node2 The AVLNode contained by the second Item.
 * 
 * @return  < 0 if first Item < second Item
 *          > 0 if first Item > second Item
 *          0 if the two are equal
 */
int32_t compare_items(AVLNode *node1, AVLNode *node2) {
    Item *item1 = container_of(node1, Item, node);
    Item *item2 = container_of(node2, Item, node);
    return item1->key - item2->key;
}

/**
 * Compares given key with a Item's key.
 * 
 * @param key   Void pointer to the key to compare it with.
 * @param node  The AVLNode contained by the Item.
 * 
 * @return  < 0 if key < Item
 *          > 0 if key > Item
 *          0 if the two are equal
 */
int32_t compare_key_to_node(void *key, AVLNode *node) {
    Item *item = container_of(node, Item, node);
    return *((int32_t *) key) - item->key;
}

/**
 * Creates an AVLTree with n nodes. The nodes are contained by Items which range in value from 1 to n.
 *
 * @param n Number of nodes to have in the tree.
 *
 * @return  Pointer to the tree.
 */
AVLTree *create_tree(uint32_t n) {
    AVLTree *tree = new AVLTree();
    for (uint32_t i = 1; i <= n; i++) {
        Item *item = new Item(i);
        tree->insert(&item->node, compare_items);
    }
    return tree;
}

/**
 * Cleans-up (deallocates) the nodes in an AVLTree, then the tree itself.
 *
 * @param tree  Pointer to the tree to clean-up.
 */
void clean_up_tree(AVLTree *tree) {
    std::queue<AVLNode *> queue;
    queue.push(tree->root);
    while (!queue.empty()) {
        AVLNode *node = queue.front();
        queue.pop();

        AVLNode *left = node->left;
        AVLNode *right = node->right;

        if (left != NULL) {
            queue.push(left);
        }
        if (right != NULL) {
            queue.push(right);
        }

        Item *item = container_of(node, Item, node);
        delete item;
    }

    delete tree;
}

/**
 * Checks if the nodes in the AVLTree match the expected structure.
 * 
 * @param tree  The AVLTree.
 * @param expected  Vector containing the nodes expected in the AVLTree. Nodes are in level order.
 */
void check_tree(AVLTree *tree, std::queue<AVLNode *> expected) {
    if (tree->root == NULL) {
        assert(expected.empty() == true);
        return;
    }

    std::queue<AVLNode *> queue;
    queue.push(tree->root);
    while (!queue.empty()) {
        AVLNode *tree_node = queue.front();
        AVLNode *expected_node = expected.front();
        queue.pop();
        expected.pop();

        assert(tree_node == expected_node);
        
        AVLNode *left = tree_node->left;
        AVLNode *right = tree_node->right;

        if (left != NULL) {
            assert(left->parent == tree_node);
            queue.push(left);
        }
        if (right != NULL) {
            assert(right->parent == tree_node);
            queue.push(right);
        }
    }
}

void test_insert_into_empty_tree() {
    AVLTree tree;
    Item twelve(12);
    tree.insert(&twelve.node, compare_items);

    std::queue<AVLNode *> expected({ &twelve.node });
    check_tree(&tree, expected);
}

void test_insert_smaller_node() {
    AVLTree tree;
    Item twelve(12);
    tree.insert(&twelve.node, compare_items);
    Item six(6);
    tree.insert(&six.node, compare_items);

    std::queue<AVLNode *> expected({ &twelve.node, &six.node });
    check_tree(&tree, expected);
}

void test_insert_bigger_node() {
    AVLTree tree;
    Item twelve(12);
    tree.insert(&twelve.node, compare_items);
    Item eighteen(18);
    tree.insert(&eighteen.node, compare_items);

    std::queue<AVLNode *> expected({ &twelve.node, &eighteen.node });
    check_tree(&tree, expected);
}

void test_insert_then_rebalance_with_right_rotation() {
    AVLTree tree;
    Item twelve(12);
    tree.insert(&twelve.node, compare_items);
    Item six(6);
    tree.insert(&six.node, compare_items);
    Item eighteen(18);
    tree.insert(&eighteen.node, compare_items);
    Item three(3);
    tree.insert(&three.node, compare_items);
    Item nine(9);
    tree.insert(&nine.node, compare_items);
    Item zero(0);
    tree.insert(&zero.node, compare_items);

    std::queue<AVLNode *> expected({ &six.node, &three.node, &twelve.node, &zero.node, &nine.node, &eighteen.node });
    check_tree(&tree, expected);
}

void test_insert_then_rebalance_with_left_right_rotation() {
   AVLTree tree;
    Item twelve(12);
    tree.insert(&twelve.node, compare_items);
    Item six(6);
    tree.insert(&six.node, compare_items);
    Item eighteen(18);
    tree.insert(&eighteen.node, compare_items);
    Item three(3);
    tree.insert(&three.node, compare_items);
    Item nine(9);
    tree.insert(&nine.node, compare_items);
    Item ten(10);
    tree.insert(&ten.node, compare_items);

    std::queue<AVLNode *> expected({ &nine.node, &six.node, &twelve.node, &three.node, &ten.node, &eighteen.node });
    check_tree(&tree, expected);
}

void test_insert_then_rebalance_with_left_rotation() {
    AVLTree tree;
    Item twelve(12);
    tree.insert(&twelve.node, compare_items);
    Item six(6);
    tree.insert(&six.node, compare_items);
    Item eighteen(18);
    tree.insert(&eighteen.node, compare_items);
    Item fourteen(14);
    tree.insert(&fourteen.node, compare_items);
    Item twenty(20);
    tree.insert(&twenty.node, compare_items);
    Item twenty_five(25);
    tree.insert(&twenty_five.node, compare_items);

    std::queue<AVLNode *> expected({ &eighteen.node, &twelve.node, &twenty.node, &six.node, &fourteen.node, &twenty_five.node });
    check_tree(&tree, expected);
}

void test_insert_then_rebalance_with_right_left_rotation() {
    AVLTree tree;
    Item twelve(12);
    tree.insert(&twelve.node, compare_items);
    Item six(6);
    tree.insert(&six.node, compare_items);
    Item eighteen(18);
    tree.insert(&eighteen.node, compare_items);
    Item fourteen(14);
    tree.insert(&fourteen.node, compare_items);
    Item twenty(20);
    tree.insert(&twenty.node, compare_items);
    Item thirteen(13);
    tree.insert(&thirteen.node, compare_items);

    std::queue<AVLNode *> expected({ &fourteen.node, &twelve.node, &eighteen.node, &six.node, &thirteen.node, &twenty.node });
    check_tree(&tree, expected);
}

void test_lookup_empty_tree() {
    AVLTree tree;
    Item key(5);
    AVLNode *node = tree.lookup(&key.node, compare_items);
    assert(node == NULL);
}

void test_lookup_non_existent_node() {
    AVLTree *tree = create_tree(25);

    Item key(45);
    AVLNode *node = tree->lookup(&key.node, compare_items);
    assert(node == NULL);

    clean_up_tree(tree);
}

void test_lookup_node() {
    AVLTree *tree = create_tree(25);

    Item key(7);
    AVLNode *node = tree->lookup(&key.node, compare_items);
    assert(container_of(node, Item, node)->key == key.key);

    clean_up_tree(tree);
}

void test_find_first_ge_empty_tree() {
    AVLTree tree;
    Item key(1);
    AVLNode *node = tree.find_first_ge(&key.node, compare_items);
    assert(node == NULL);
}

void test_find_first_ge_no_larger_node() {
    AVLTree *tree = create_tree(25);

    Item key(50);
    AVLNode *node = tree->find_first_ge(&key.node, compare_items);
    assert(node == NULL);

    clean_up_tree(tree);
}

void test_find_first_ge_equal_node_found() {
    AVLTree *tree = create_tree(25);

    Item key(18);
    AVLNode *node = tree->find_first_ge(&key.node, compare_items);
    AVLNode *expected = tree->lookup(&key.node, compare_items);
    assert(node == expected);

    clean_up_tree(tree);
}

void test_find_first_ge_larger_node_found() {
    AVLTree *tree = create_tree(25);
    Item *fifty = new Item(50); // dynamically allocate so clean up doesn't fail
    tree->insert(&fifty->node, compare_items);

    Item key(35);
    AVLNode *node = tree->find_first_ge(&key.node, compare_items);
    assert(node == &fifty->node);

    clean_up_tree(tree);
}

void test_find_offset_zero_offset() {
    AVLTree *tree = create_tree(25);

    Item key(10);
    AVLNode *node = tree->lookup(&key.node, compare_items);
    AVLNode *offset = tree->find_offset(node, 0);

    assert(offset == node);

    clean_up_tree(tree);
}

void test_find_offset_positive_offset() {
    AVLTree *tree = create_tree(25);

    Item key(10);
    AVLNode *node = tree->lookup(&key.node, compare_items);
    AVLNode *offset = tree->find_offset(node, 15);

    key = Item(25);
    node = tree->lookup(&key.node, compare_items);
    assert(offset == node);

    clean_up_tree(tree);
}

void test_find_offset_negative_offset() {
    AVLTree *tree = create_tree(25);

    Item key(10);
    AVLNode *node = tree->lookup(&key.node, compare_items);
    AVLNode *offset = tree->find_offset(node, -5);

    key = Item(5);
    node = tree->lookup(&key.node, compare_items);
    assert(offset == node);

    clean_up_tree(tree);
}

void test_find_offset_after_largest_node() {
    AVLTree *tree = create_tree(25);

    Item key(10);
    AVLNode *node = tree->lookup(&key.node, compare_items);
    AVLNode *offset = tree->find_offset(node, 16);
    
    assert(offset == NULL);

    clean_up_tree(tree);
}


void test_find_offset_before_smallest_node() {
    AVLTree *tree = create_tree(25);

    Item key(10);
    AVLNode *node = tree->lookup(&key.node, compare_items);
    AVLNode *offset = tree->find_offset(node, -11);
    
    assert(offset == NULL);

    clean_up_tree(tree);
}

void test_remove_non_existent_node() {
    AVLTree tree;
    Item twelve(12);
    tree.insert(&twelve.node, compare_items);
    Item six(6);
    tree.insert(&six.node, compare_items);
    Item eighteen(18);
    tree.insert(&eighteen.node, compare_items);

    Item four(4);
    void *key = (void *) &four.key;
    AVLNode *node = tree.remove(key, compare_key_to_node);

    assert(node == NULL);
    std::queue<AVLNode *> expected({ &twelve.node, &six.node, &eighteen.node });
    check_tree(&tree, expected);
}

void test_remove_node_with_no_children() {
    AVLTree tree;
    Item twelve(12);
    tree.insert(&twelve.node, compare_items);

    void *key = (void *) &twelve.key;
    AVLNode *node = tree.remove(key, compare_key_to_node);

    assert(node == &twelve.node);
    std::queue<AVLNode *> expected;
    check_tree(&tree, expected);
}

void test_remove_node_with_one_child() {
    AVLTree tree;
    Item twelve(12);
    tree.insert(&twelve.node, compare_items);
    Item six(6);
    tree.insert(&six.node, compare_items);
    
    void *key = (void *) &twelve.key;
    AVLNode *node = tree.remove(key, compare_key_to_node);

    assert(node == &twelve.node);
    std::queue<AVLNode *> expected({ &six.node });
    check_tree(&tree, expected);
}

void test_remove_node_with_two_children() {
    AVLTree tree;
    Item twelve(12);
    tree.insert(&twelve.node, compare_items);
    Item six(6);
    tree.insert(&six.node, compare_items);
    Item eighteen(18);
    tree.insert(&eighteen.node, compare_items);

    void *key = (void *) &twelve.key;
    AVLNode *node = tree.remove(key, compare_key_to_node);

    assert(node == &twelve.node);
    std::queue<AVLNode *> expected({ &eighteen.node, &six.node });
    check_tree(&tree, expected);
}

void test_remove_then_rebalance_with_right_rotation() {
    AVLTree tree;
    Item twelve(12);
    tree.insert(&twelve.node, compare_items);
    Item six(6);
    tree.insert(&six.node, compare_items);
    Item eighteen(18);
    tree.insert(&eighteen.node, compare_items);
    Item three(3);
    tree.insert(&three.node, compare_items);
    Item nine(9);
    tree.insert(&nine.node, compare_items);

    void *key = (void *) &eighteen.key;
    AVLNode *node = tree.remove(key, compare_key_to_node);
    
    assert(node == &eighteen.node);
    std::queue<AVLNode *> expected({ &six.node, &three.node, &twelve.node, &nine.node });
    check_tree(&tree, expected);
}

void test_remove_then_rebalance_with_left_right_rotation() {
    AVLTree tree;
    Item twelve(12);
    tree.insert(&twelve.node, compare_items);
    Item six(6);
    tree.insert(&six.node, compare_items);
    Item eighteen(18);
    tree.insert(&eighteen.node, compare_items);
    Item three(3);
    tree.insert(&three.node, compare_items);
    Item nine(9);
    tree.insert(&nine.node, compare_items);
    Item sixteen(16);
    tree.insert(&sixteen.node, compare_items);
    Item ten(10);
    tree.insert(&ten.node, compare_items);

    void *key = (void *) &sixteen.key;
    AVLNode *node = tree.remove(key, compare_key_to_node);

    assert(node == &sixteen.node);
    std::queue<AVLNode *> expected({ &nine.node, &six.node, &twelve.node, &three.node, &ten.node, &eighteen.node });
    check_tree(&tree, expected);
}

void test_remove_then_rebalance_with_left_rotation() {
    AVLTree tree;
    Item twelve(12);
    tree.insert(&twelve.node, compare_items);
    Item six(6);
    tree.insert(&six.node, compare_items);
    Item eighteen(18);
    tree.insert(&eighteen.node, compare_items);
    Item fourteen(14);
    tree.insert(&fourteen.node, compare_items);
    Item twenty(20);
    tree.insert(&twenty.node, compare_items);

    void *key = (void *) &six.key;
    AVLNode *node = tree.remove(key, compare_key_to_node);

    assert(node == &six.node);
    std::queue<AVLNode *> expected({ &eighteen.node, &twelve.node, &twenty.node, &fourteen.node });
    check_tree(&tree, expected);
}

void test_remove_then_rebalance_with_right_left_rotation() {
    AVLTree tree;
    Item twelve(12);
    tree.insert(&twelve.node, compare_items);
    Item six(6);
    tree.insert(&six.node, compare_items);
    Item eighteen(18);
    tree.insert(&eighteen.node, compare_items);
    Item fourteen(14);
    tree.insert(&fourteen.node, compare_items);
    Item twenty(20);
    tree.insert(&twenty.node, compare_items);    
    Item four(4);
    tree.insert(&four.node, compare_items);
    Item thirteen(13);
    tree.insert(&thirteen.node, compare_items);

    void *key = (void *) &four.key;
    AVLNode *node = tree.remove(key, compare_key_to_node);

    assert(node == &four.node);
    std::queue<AVLNode *> expected({ &fourteen.node, &twelve.node, &eighteen.node, &six.node, &thirteen.node, &twenty.node });
    check_tree(&tree, expected);
}

void test_rank_only_node_in_tree() {
    AVLTree tree;
    Item ten(10);
    tree.insert(&ten.node, compare_items);

    uint64_t rank = AVLTree::rank(&ten.node);
    assert(rank == 1);
}

void test_rank_smallest_node() {
    AVLTree *tree = create_tree(25);
    Item key(1);
    AVLNode *node = tree->lookup(&key.node, compare_items);

    uint64_t rank = AVLTree::rank(node);
    assert(rank == 1);

    clean_up_tree(tree);
}

void test_rank_middle_node() {
    AVLTree *tree = create_tree(25);
    Item key(15);
    AVLNode *node = tree->lookup(&key.node, compare_items);

    uint64_t rank = AVLTree::rank(node);
    assert(rank == 15);

    clean_up_tree(tree);
}

void test_rank_largest_node() {
    AVLTree *tree = create_tree(25);
    Item key(25);
    AVLNode *node = tree->lookup(&key.node, compare_items);

    uint64_t rank = AVLTree::rank(node);
    assert(rank == 25);

    clean_up_tree(tree);
}

int main() {
    test_insert_into_empty_tree();
    test_insert_smaller_node();
    test_insert_bigger_node();
    test_insert_then_rebalance_with_right_rotation();
    test_insert_then_rebalance_with_left_right_rotation();
    test_insert_then_rebalance_with_left_rotation();
    test_insert_then_rebalance_with_right_left_rotation();

    test_lookup_empty_tree();
    test_lookup_non_existent_node();
    test_lookup_node();

    test_find_first_ge_empty_tree();
    test_find_first_ge_no_larger_node();
    test_find_first_ge_equal_node_found();
    test_find_first_ge_larger_node_found();

    test_find_offset_zero_offset();
    test_find_offset_positive_offset();
    test_find_offset_negative_offset();
    test_find_offset_after_largest_node();
    test_find_offset_before_smallest_node();

    test_remove_non_existent_node();
    test_remove_node_with_no_children();
    test_remove_node_with_one_child();
    test_remove_node_with_two_children();
    test_remove_then_rebalance_with_right_rotation();
    test_remove_then_rebalance_with_left_right_rotation();
    test_remove_then_rebalance_with_left_rotation();
    test_remove_then_rebalance_with_right_left_rotation();

    test_rank_only_node_in_tree();
    test_rank_smallest_node();
    test_rank_middle_node();
    test_rank_largest_node();

    return 0;
}
