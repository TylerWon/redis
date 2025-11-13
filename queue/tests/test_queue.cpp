#include <assert.h>

#include "../Queue.hpp"

void test_push_empty_list() {
    Queue queue;
    
    QNode node;
    queue.push(&node);

    assert(queue.is_empty() == false);
    assert(queue.front() == &node);
    queue.remove(&node);
    assert(queue.is_empty() == true);
}

void test_push_non_empty_list() {
    Queue queue;
    
    QNode node1;
    queue.push(&node1);
    QNode node2;
    queue.push(&node2);

    assert(queue.is_empty() == false);
    assert(queue.front() == &node1);
    queue.remove(&node1);
    assert(queue.front() == &node2);
    queue.remove(&node2);
    assert(queue.is_empty() == true);
}

void test_remove_first_node() {
    Queue queue;

    QNode node1;
    queue.push(&node1);
    QNode node2;
    queue.push(&node2);
    QNode node3;
    queue.push(&node3);

    queue.remove(&node1);

    assert(queue.is_empty() == false);
    assert(queue.front() == &node2);
    queue.remove(&node2);
    assert(queue.front() == &node3);
    queue.remove(&node3);
    assert(queue.is_empty() == true);
}

void test_remove_middle_node() {
    Queue queue;

    QNode node1;
    queue.push(&node1);
    QNode node2;
    queue.push(&node2);
    QNode node3;
    queue.push(&node3);

    queue.remove(&node2);

    assert(queue.is_empty() == false);
    assert(queue.front() == &node1);
    queue.remove(&node1);
    assert(queue.front() == &node3);
    queue.remove(&node3);
    assert(queue.is_empty() == true);
}

void test_remove_last_node() {
    Queue queue;

    QNode node1;
    queue.push(&node1);
    QNode node2;
    queue.push(&node2);
    QNode node3;
    queue.push(&node3);

    queue.remove(&node3);

    assert(queue.is_empty() == false);
    assert(queue.front() == &node1);
    queue.remove(&node1);
    assert(queue.front() == &node2);
    queue.remove(&node2);
    assert(queue.is_empty() == true);
}

void test_front_empty_list() {
    Queue queue;
    QNode *node = queue.front();
    assert(node == NULL);
}

void test_front_non_empty_list() {
    Queue queue;
    
    QNode node1;
    queue.push(&node1);
    QNode node2;
    queue.push(&node2);

    QNode *node = queue.front();
    assert(node == &node1);
}

void test_is_empty_empty_list() {
    Queue queue;
    assert(queue.is_empty() == true);
}

void test_is_empty_non_empty_list() {
    Queue queue;
    QNode node;
    queue.push(&node);
    assert(queue.is_empty() == false);
}

int main() {
    test_push_empty_list();
    test_push_non_empty_list();

    test_remove_first_node();
    test_remove_middle_node();
    test_remove_last_node();

    test_front_empty_list();
    test_front_non_empty_list();

    test_is_empty_empty_list();
    test_is_empty_non_empty_list();

    return 0;
}
