#pragma once

#include <cstddef>
#include <cstdint>

/* Node in an AVLTree */
class AVLNode {
    public:
        AVLNode *parent = NULL;
        AVLNode *left = NULL;
        AVLNode *right = NULL;
        uint32_t height = 1; // tree height
        uint32_t size = 1; // tree size
        
        /* Updates the node's metadata (height and size) */
        void update();

        /* Returns the balance factor for the AVLNode. */
        int8_t balance_factor();

        /* Returns the height of the tree rooted at the given AVLNode */
        static uint32_t get_height(AVLNode *root);

        /* Returns the size of the tree rooted at the given AVLNode */
        static uint32_t get_size(AVLNode *root);
};
