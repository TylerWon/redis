#include <algorithm>

#include "AVLNode.hpp"

void AVLNode::update_height() {
    height = 1 + std::max(get_height(left), get_height(right));
}

int8_t AVLNode::balance_factor() {
    return get_height(left) - get_height(right);
}

uint32_t AVLNode::get_height(AVLNode *root) {
    return root != NULL ? root->height : 0;
}
