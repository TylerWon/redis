#include "AVLTree.hpp"
#include <cstdio>

void AVLTree::insert(AVLNode *new_node, int32_t (*cmp)(AVLNode *, AVLNode *)) {
    AVLNode *parent = NULL;
    AVLNode **to_spot = &root; // incoming pointer to the node's spot in the tree (i.e. address of parent's left or right)
    for (AVLNode *node = *to_spot; node != NULL; node = *to_spot) {
        to_spot = cmp(new_node, node) < 0 ? &node->left : &node->right; // duplicates are inserted to the right of the original
        parent = node;
    }

    *to_spot = new_node;
    new_node->parent = parent;

    root = fix_imbalances(*to_spot);
}

AVLNode *AVLTree::remove(void *key, int32_t (*cmp)(void *, AVLNode *)) {
    AVLNode *node = root;
    while (node != NULL) {
        int32_t v = cmp(key, node);
        if (v < 0) {
            node = node->left;
        } else if (v > 0) {
            node = node->right;
        } else {
            root = remove(node);
            break;
        }
    }

    return node;
}

AVLNode *AVLTree::remove(AVLNode *node) {
    if (node->left == NULL || node->right == NULL) {
        return remove_one_or_no_child(node);
    } 

    AVLNode *successor = node->right;
    while (successor->left != NULL) {
        successor = successor->left;
    }

    AVLNode *root = remove_one_or_no_child(successor); // in-order successor is the left-most child in the right subtree so can have at most 1 child 

    // swap node with successor
    // copy node's pointers
    *successor = *node;

    // change child's parent pointer to be successor
    if (successor->left != NULL) {
        successor->left->parent = successor;
    }
    if (successor->right != NULL) {
        successor->right->parent = successor;
    }

    // change parent's child pointer to be successor
    AVLNode *parent = successor->parent;
    if (parent == NULL) {
        return successor;
    }
    AVLNode **to_successor = parent->left == node ? &parent->left : &parent->right;
    *to_successor = successor;

    return root;
}

AVLNode *AVLTree::fix_imbalances(AVLNode *node) {
    while (true) {
        AVLNode **to_node = &node; // incoming pointer to the node (i.e. address of parent's left or right)
        AVLNode *parent = node->parent;
        if (parent != NULL) {
            to_node = parent->left == node ? &parent->left : &parent->right;
        }

        node->update_height();

        // if re-balancing is required, update to_node because rotations will change the root of the subtree that was
        // originally rooted by node
        int8_t balance_factor = node->balance_factor();
        if (balance_factor > 1) {
            *to_node = fix_left_imbalance(node);
        } else if (balance_factor < -1) {
            *to_node = fix_right_imbalance(node);
        }

        if (parent == NULL) {
            return *to_node;
        }

        node = parent; 
    }
}

AVLNode *AVLTree::fix_left_imbalance(AVLNode *root) {
    if (root->left->balance_factor() < 0) {
        root->left = rotate_left(root->left);
    }
    
    return rotate_right(root);
}

AVLNode *AVLTree::fix_right_imbalance(AVLNode *root) {
    if (root->right->balance_factor() > 0) {
        root->right = rotate_right(root->right);
    }

    return rotate_left(root);
}

AVLNode *AVLTree::rotate_left(AVLNode *root) {
    AVLNode *new_root = root->right;
    root->right = new_root->left;
    if (root->right != NULL) {
        root->right->parent = root;
    }
    new_root->left = root;
    new_root->parent = root->parent;
    root->parent = new_root;

    root->update_height();
    new_root->update_height();

    return new_root;
}

AVLNode *AVLTree::rotate_right(AVLNode *root) {
    AVLNode *new_root = root->left;
    root->left = new_root->right;
    if (root->left != NULL) {
        root->left->parent = root;
    }
    new_root->right = root;
    new_root->parent = root->parent;
    root->parent = new_root;

    root->update_height();
    new_root->update_height();

    return new_root;
}

AVLNode *AVLTree::remove_one_or_no_child(AVLNode *node) {
    AVLNode *replacement = node->left != NULL ? node->left : node->right;
    AVLNode *parent = node->parent;

    if (replacement != NULL) {
        replacement->parent = parent;
    }

    if (parent == NULL) {
        return replacement;
    }

    AVLNode **to_node = parent->left == node ? &parent->left : &parent->right;
    *to_node = replacement;

    return fix_imbalances(parent); // replacement's height doesn't change so first possible unbalanced node is its parent
}
