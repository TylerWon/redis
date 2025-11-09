#include <algorithm>

#include "AVLNode.hpp"

void AVLNode::update() {
    height = 1 + std::max(get_height(left), get_height(right));
    size = 1 + get_size(left) + get_size(right);
}

int8_t AVLNode::balance_factor() {
    return get_height(left) - get_height(right);
}

uint32_t AVLNode::get_height(AVLNode *root) {
    return root != NULL ? root->height : 0;
}

uint32_t AVLNode::get_size(AVLNode *root) {
    return root != NULL ? root->size : 0;
}
