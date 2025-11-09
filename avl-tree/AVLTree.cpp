#include <cstdio>
#include <cmath>

#include "AVLTree.hpp"

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

AVLNode *AVLTree::lookup(AVLNode *key, int32_t (*cmp)(AVLNode *, AVLNode *)) {
    AVLNode *node = root;
    while (node != NULL) {
        int32_t v = cmp(key, node);
        if (v < 0) {
            node = node->left;
        } else if (v > 0) {
            node = node->right;
        } else {
            break;
        }
    }

    return node;
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

AVLNode *AVLTree::find_first_ge(AVLNode *key, int32_t (*cmp)(AVLNode *, AVLNode *)) {
    AVLNode *found = NULL;
    AVLNode *node = root;
    while (node != NULL) {
        int32_t v = cmp(key, node);
        if (v <= 0) {
            found = node;
            node = node->left; // could find node that is larger than key, but smaller than this one in left subtree
        } else {
            node = node->right;
        } 
    }

    return found;
}

AVLNode *AVLTree::find_offset(AVLNode *node, int64_t offset) {
    while (offset != 0) {
        if (offset > 0 && offset <= AVLNode::get_size(node->right)) {
            // case 1: offset in right subtree
            node = node->right;
            offset -= AVLNode::get_size(node->left) + 1;
        } else if (offset < 0 && std::abs(offset) <= AVLNode::get_size(node->left)) {
            // case 2: negative offset and offset in left subtree
            node = node->left;
            offset += AVLNode::get_size(node->right) + 1;
        } else {
            // case 3: offset in neither left or right subtree, go to parent
            AVLNode *parent = node->parent;
            if (parent == NULL) {
                // reached root without finding offset
                node = parent;
                break;
            }

            if (parent->left == node) {
                offset -= AVLNode::get_size(node->right) + 1;
            } else {
                offset += AVLNode::get_size(node->left) + 1;
            }

            node = parent;
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

uint64_t AVLTree::rank(AVLNode *node) {
    uint64_t offset_from_root = 0;
    while (node->parent != NULL) {
        AVLNode *parent = node->parent;

        if (parent->left == node) {
            offset_from_root -= AVLNode::get_size(node->right) + 1;
        } else {
            offset_from_root += AVLNode::get_size(node->left) + 1;
        }

        node = parent;
    }

    uint64_t root_rank = AVLNode::get_size(node->left) + 1;
    return root_rank + offset_from_root;
}

AVLNode *AVLTree::fix_imbalances(AVLNode *node) {
    while (true) {
        AVLNode **to_node = &node; // incoming pointer to the node (i.e. address of parent's left or right)
        AVLNode *parent = node->parent;
        if (parent != NULL) {
            to_node = parent->left == node ? &parent->left : &parent->right;
        }

        node->update();

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

    root->update();
    new_root->update();

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

    root->update();
    new_root->update();

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
