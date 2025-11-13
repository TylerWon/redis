#include "Queue.hpp"

Queue::Queue() {
    // head points to a "dummy" node
    // avoids having to deal with a special case when inserting into an empty queue, or removing last node in the queue
    head = new QNode();
    head->prev = head;
    head->next = head;
}

Queue::~Queue() {
    delete head;
}

void Queue::push(QNode *node) {
    QNode *prev = head->prev;
    prev->next = node;
    node->prev = prev;
    node->next = head;
    head->prev = node;
}

void Queue::remove(QNode *node) {
    QNode *prev = node->prev;
    QNode *next = node->next;
    prev->next = next;
    next->prev = prev;
}

QNode *Queue::front() {
    if (is_empty()) {
        return NULL;
    }
    return head->next;
}

bool Queue::is_empty() {
    return head->next == head;
}
