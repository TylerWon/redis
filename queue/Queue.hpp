#pragma once

#include <cstddef>

#include "components/QNode.hpp"

class Queue {
    private:
        QNode *head;
    public:
        Queue();

        ~Queue();
        
        /**
         * Appends the node to the end of the Queue.
         * 
         * @param node  The node to append.
         */
        void push(QNode *node);

        /**
         * Removes the node from the Queue.
         * 
         * @param node  The node to remove.
         */
        void remove(QNode *node);

        /* Returns the node at the front of the Queue. */
        QNode *front();

        /* Checks if the Queue is empty. */
        bool is_empty();
};
