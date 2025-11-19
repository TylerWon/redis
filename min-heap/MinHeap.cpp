#include "MinHeap.hpp"

void MinHeap::insert(MHNode *node, bool (*is_less)(MHNode *, MHNode *)) {
    nodes.push_back(node);
    node->pos = nodes.size() - 1;
    heapify(node->pos, is_less);
}

void MinHeap::remove(MHNode *node, bool (*is_less)(MHNode *, MHNode *)) {
    MHNode *last = nodes[nodes.size() - 1];
    nodes[node->pos] = last;
    last->pos = node->pos;
    nodes.pop_back();
    heapify(last->pos, is_less);

}

MHNode *MinHeap::min() {
    return !is_empty() ? nodes[0] : NULL;
}

bool MinHeap::is_empty() {
    return nodes.empty();
}

void MinHeap::heapify(uint32_t pos, bool (*is_less)(MHNode *, MHNode *)) {
    MHNode *parent = get_parent(pos);
    MHNode *node = nodes[pos];
    if (parent != NULL && is_less(node, parent)) {
        heapify_up(pos, is_less);
    } else {
        heapify_down(pos, is_less);
    }
}

void MinHeap::heapify_up(uint32_t pos, bool (*is_less)(MHNode *, MHNode *)) {
    MHNode *parent = get_parent(pos);
    MHNode *node = nodes[pos];
    while (parent != NULL && is_less(node, parent)) {
        nodes[pos] = parent;
        nodes[parent->pos] = node;

        node->pos = parent->pos;
        parent->pos = pos;
        pos = node->pos;

        parent = get_parent(pos);
        node = nodes[pos];
    }
}

void MinHeap::heapify_down(uint32_t pos, bool (*is_less)(MHNode *, MHNode *)) {
    while (true) {
        MHNode *node = nodes[pos];
        MHNode *min = node;
        MHNode *left = get_left(pos);
        MHNode *right = get_right(pos);
        
        if (left != NULL && is_less(left, min)) {
            min = left;
        }
        if (right != NULL && is_less(right, min)) {
            min = right;
        }

        if (min == node) {
            break;
        }

        nodes[pos] = min;
        nodes[min->pos] = node;

        node->pos = min->pos;
        min->pos = pos;
        pos = node->pos;
    }
}

MHNode *MinHeap::get_parent(uint32_t pos) {
    if (pos == 0) {
        return NULL;
    }
    uint32_t parent_pos = (pos + 1) / 2 - 1;
    return nodes[parent_pos];
}

MHNode *MinHeap::get_left(uint32_t pos) {
    uint32_t left_pos = pos * 2 + 1;
    return left_pos < nodes.size() ? nodes[left_pos] : NULL;
}

MHNode *MinHeap::get_right(uint32_t pos) {
    uint32_t right_pos = pos * 2 + 2;
    return right_pos < nodes.size() ? nodes[right_pos] : NULL;
}
