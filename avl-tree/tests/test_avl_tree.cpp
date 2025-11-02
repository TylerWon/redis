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
int32_t compare_nodes(AVLNode *node1, AVLNode *node2) {
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
 * Checks if the nodes in the AVLTree match the expected structure.
 * 
 * @param avl_tree  The AVLTree.
 * @param expected  Vector containing the nodes expected in the AVLTree. Nodes are in level order.
 */
void check_tree(AVLTree *avl_tree, std::queue<AVLNode *> expected) {
    if (avl_tree->root == NULL) {
        assert(expected.empty() == true);
        return;
    }

    std::queue<AVLNode *> queue;
    queue.push(avl_tree->root);
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
    AVLTree avl_tree;
    Item twelve(12);
    avl_tree.insert(&twelve.node, compare_nodes);

    std::queue<AVLNode *> expected({ &twelve.node });
    check_tree(&avl_tree, expected);
}

void test_insert_smaller_node() {
    AVLTree avl_tree;
    Item twelve(12);
    avl_tree.insert(&twelve.node, compare_nodes);
    Item six(6);
    avl_tree.insert(&six.node, compare_nodes);

    std::queue<AVLNode *> expected({ &twelve.node, &six.node });
    check_tree(&avl_tree, expected);
}

void test_insert_bigger_node() {
    AVLTree avl_tree;
    Item twelve(12);
    avl_tree.insert(&twelve.node, compare_nodes);
    Item eighteen(18);
    avl_tree.insert(&eighteen.node, compare_nodes);

    std::queue<AVLNode *> expected({ &twelve.node, &eighteen.node });
    check_tree(&avl_tree, expected);
}

void test_insert_then_rebalance_with_right_rotation() {
    AVLTree avl_tree;
    Item twelve(12);
    avl_tree.insert(&twelve.node, compare_nodes);
    Item six(6);
    avl_tree.insert(&six.node, compare_nodes);
    Item eighteen(18);
    avl_tree.insert(&eighteen.node, compare_nodes);
    Item three(3);
    avl_tree.insert(&three.node, compare_nodes);
    Item nine(9);
    avl_tree.insert(&nine.node, compare_nodes);
    Item zero(0);
    avl_tree.insert(&zero.node, compare_nodes);

    std::queue<AVLNode *> expected({ &six.node, &three.node, &twelve.node, &zero.node, &nine.node, &eighteen.node });
    check_tree(&avl_tree, expected);
}

void test_insert_then_rebalance_with_left_right_rotation() {
   AVLTree avl_tree;
    Item twelve(12);
    avl_tree.insert(&twelve.node, compare_nodes);
    Item six(6);
    avl_tree.insert(&six.node, compare_nodes);
    Item eighteen(18);
    avl_tree.insert(&eighteen.node, compare_nodes);
    Item three(3);
    avl_tree.insert(&three.node, compare_nodes);
    Item nine(9);
    avl_tree.insert(&nine.node, compare_nodes);
    Item ten(10);
    avl_tree.insert(&ten.node, compare_nodes);

    std::queue<AVLNode *> expected({ &nine.node, &six.node, &twelve.node, &three.node, &ten.node, &eighteen.node });
    check_tree(&avl_tree, expected);
}

void test_insert_then_rebalance_with_left_rotation() {
    AVLTree avl_tree;
    Item twelve(12);
    avl_tree.insert(&twelve.node, compare_nodes);
    Item six(6);
    avl_tree.insert(&six.node, compare_nodes);
    Item eighteen(18);
    avl_tree.insert(&eighteen.node, compare_nodes);
    Item fourteen(14);
    avl_tree.insert(&fourteen.node, compare_nodes);
    Item twenty(20);
    avl_tree.insert(&twenty.node, compare_nodes);
    Item twenty_five(25);
    avl_tree.insert(&twenty_five.node, compare_nodes);

    std::queue<AVLNode *> expected({ &eighteen.node, &twelve.node, &twenty.node, &six.node, &fourteen.node, &twenty_five.node });
    check_tree(&avl_tree, expected);
}

void test_insert_then_rebalance_with_right_left_rotation() {
    AVLTree avl_tree;
    Item twelve(12);
    avl_tree.insert(&twelve.node, compare_nodes);
    Item six(6);
    avl_tree.insert(&six.node, compare_nodes);
    Item eighteen(18);
    avl_tree.insert(&eighteen.node, compare_nodes);
    Item fourteen(14);
    avl_tree.insert(&fourteen.node, compare_nodes);
    Item twenty(20);
    avl_tree.insert(&twenty.node, compare_nodes);
    Item thirteen(13);
    avl_tree.insert(&thirteen.node, compare_nodes);

    std::queue<AVLNode *> expected({ &fourteen.node, &twelve.node, &eighteen.node, &six.node, &thirteen.node, &twenty.node });
    check_tree(&avl_tree, expected);
}

void test_remove_node_with_no_children() {
    AVLTree avl_tree;
    Item twelve(12);
    avl_tree.insert(&twelve.node, compare_nodes);

    void *key = (void *) &twelve.key;
    avl_tree.remove(key, compare_key_to_node);

    std::queue<AVLNode *> expected;
    check_tree(&avl_tree, expected);
}

void test_remove_node_with_one_child() {
    AVLTree avl_tree;
    Item twelve(12);
    avl_tree.insert(&twelve.node, compare_nodes);
    Item six(6);
    avl_tree.insert(&six.node, compare_nodes);
    
    void *key = (void *) &twelve.key;
    avl_tree.remove(key, compare_key_to_node);

    std::queue<AVLNode *> expected({ &six.node });
    check_tree(&avl_tree, expected);
}

void test_remove_node_with_two_children() {
    AVLTree avl_tree;
    Item twelve(12);
    avl_tree.insert(&twelve.node, compare_nodes);
    Item six(6);
    avl_tree.insert(&six.node, compare_nodes);
    Item eighteen(18);
    avl_tree.insert(&eighteen.node, compare_nodes);

    void *key = (void *) &twelve.key;
    avl_tree.remove(key, compare_key_to_node);

    std::queue<AVLNode *> expected({ &eighteen.node, &six.node });
    check_tree(&avl_tree, expected);
}

void test_remove_then_rebalance_with_right_rotation() {
    AVLTree avl_tree;
    Item twelve(12);
    avl_tree.insert(&twelve.node, compare_nodes);
    Item six(6);
    avl_tree.insert(&six.node, compare_nodes);
    Item eighteen(18);
    avl_tree.insert(&eighteen.node, compare_nodes);
    Item three(3);
    avl_tree.insert(&three.node, compare_nodes);
    Item nine(9);
    avl_tree.insert(&nine.node, compare_nodes);

    void *key = (void *) &eighteen.key;
    avl_tree.remove(key, compare_key_to_node);

    std::queue<AVLNode *> expected({ &six.node, &three.node, &twelve.node, &nine.node });
    check_tree(&avl_tree, expected);
}

void test_remove_then_rebalance_with_left_right_rotation() {
    AVLTree avl_tree;
    Item twelve(12);
    avl_tree.insert(&twelve.node, compare_nodes);
    Item six(6);
    avl_tree.insert(&six.node, compare_nodes);
    Item eighteen(18);
    avl_tree.insert(&eighteen.node, compare_nodes);
    Item three(3);
    avl_tree.insert(&three.node, compare_nodes);
    Item nine(9);
    avl_tree.insert(&nine.node, compare_nodes);
    Item sixteen(16);
    avl_tree.insert(&sixteen.node, compare_nodes);
    Item ten(10);
    avl_tree.insert(&ten.node, compare_nodes);

    void *key = (void *) &sixteen.key;
    avl_tree.remove(key, compare_key_to_node);

    std::queue<AVLNode *> expected({ &nine.node, &six.node, &twelve.node, &three.node, &ten.node, &eighteen.node });
    check_tree(&avl_tree, expected);
}

void test_remove_then_rebalance_with_left_rotation() {
    AVLTree avl_tree;
    Item twelve(12);
    avl_tree.insert(&twelve.node, compare_nodes);
    Item six(6);
    avl_tree.insert(&six.node, compare_nodes);
    Item eighteen(18);
    avl_tree.insert(&eighteen.node, compare_nodes);
    Item fourteen(14);
    avl_tree.insert(&fourteen.node, compare_nodes);
    Item twenty(20);
    avl_tree.insert(&twenty.node, compare_nodes);

    void *key = (void *) &six.key;
    avl_tree.remove(key, compare_key_to_node);

    std::queue<AVLNode *> expected({ &eighteen.node, &twelve.node, &twenty.node, &fourteen.node });
    check_tree(&avl_tree, expected);
}

void test_remove_then_rebalance_with_right_left_rotation() {
    AVLTree avl_tree;
    Item twelve(12);
    avl_tree.insert(&twelve.node, compare_nodes);
    Item six(6);
    avl_tree.insert(&six.node, compare_nodes);
    Item eighteen(18);
    avl_tree.insert(&eighteen.node, compare_nodes);
    Item fourteen(14);
    avl_tree.insert(&fourteen.node, compare_nodes);
    Item twenty(20);
    avl_tree.insert(&twenty.node, compare_nodes);    
    Item four(4);
    avl_tree.insert(&four.node, compare_nodes);
    Item thirteen(13);
    avl_tree.insert(&thirteen.node, compare_nodes);

    void *key = (void *) &four.key;
    avl_tree.remove(key, compare_key_to_node);

    std::queue<AVLNode *> expected({ &fourteen.node, &twelve.node, &eighteen.node, &six.node, &thirteen.node, &twenty.node });
    check_tree(&avl_tree, expected);
}

int main() {
    test_insert_into_empty_tree();
    test_insert_smaller_node();
    test_insert_bigger_node();
    test_insert_then_rebalance_with_right_rotation();
    test_insert_then_rebalance_with_left_right_rotation();
    test_insert_then_rebalance_with_left_rotation();
    test_insert_then_rebalance_with_right_left_rotation();

    test_remove_node_with_no_children();
    test_remove_node_with_one_child();
    test_remove_node_with_two_children();
    test_remove_then_rebalance_with_right_rotation();
    test_remove_then_rebalance_with_left_right_rotation();
    test_remove_then_rebalance_with_left_rotation();
    test_remove_then_rebalance_with_right_left_rotation();

    return 0;
}
