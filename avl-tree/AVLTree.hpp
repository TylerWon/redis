#pragma once

#include <cstddef>
#include <cstdint>

#include "components/AVLNode.hpp"

/** AVL tree */
class AVLTree {
    public:
        AVLNode *root = NULL;

        /**
         * Inserts the given node into the AVLTree.
         * 
         * @param new_node  Pointer to the node to insert.
         * @param cmp       Function that compares two nodes. Should return < 0 if first node < second node, > 0 if 
         *                  first node > second node, and 0 if the two are equal.
         */
        void insert(AVLNode *new_node, int32_t (*cmp)(AVLNode *, AVLNode *));

        /**
         * Removes the node with the given key from the AVLTree.
         * 
         * @param key   Void pointer to the key to remove.
         * @param cmp   Function that compares a node's key to the key. Should return < 0 if key < node, > 0 if key > 
         *              node, and 0 if the two are equal.
         * 
         * @return  Pointer to the node that was removed.
         *          NULL if a node with the key does not exist in the AVLTree.
         */
        AVLNode *remove(void *key, int32_t (*cmp)(void *, AVLNode *));

        /**
         * Removes the node from its AVLTree.
         * 
         * @param node  Pointer to the node to remove.
         * 
         * @return  Pointer to the root of the AVLTree.
         */
        static AVLNode *remove(AVLNode *node);
    private:
        /**
         * Fixes any imbalances starting at the given node up to the root of the AVLTree.
         * 
         * @param node  Pointer to the starting node.
         * 
         * @return  Pointer to the root of the AVLTree.
         */
        static AVLNode *fix_imbalances(AVLNode *node);

        /**
         * Fixes an imbalance due to an AVLTree's left subtree being taller.
         * 
         * @param root  The root of the unbalanced AVLTree.
         * 
         * @return  Pointer to the root of the AVLTree. 
         */
        static AVLNode *fix_left_imbalance(AVLNode *root);

        /**
         * Fixes an imbalance due to an AVLTree's right subtree being taller.
         * 
         * @param root  The root of the unbalanced AVLTree.
         * 
         * @return  Pointer to the root of the AVLTree. 
         */
        static AVLNode *fix_right_imbalance(AVLNode *root);

        /**
         * Rotates an AVLTree to the left.
         * 
         * @param root  The root of the AVLTree.
         * 
         * @return  Pointer to the root of the AVLTree.
         */
        static AVLNode *rotate_left(AVLNode *root);
        
        /**
         * Rotates an AVLTree to the right.
         * 
         * @param root  The root of the AVLTree.
         * 
         * @return  Pointer to the root of the AVLTree.
         */
        static AVLNode *rotate_right(AVLNode *root);

        /**
         * Removes a node with one or no child from its AVLTree.
         * 
         * @param node  Pointer to the node to remove.
         * 
         * @return  Pointer to the root of the AVLTree.
         */
        static AVLNode *remove_one_or_no_child(AVLNode *node);
};
