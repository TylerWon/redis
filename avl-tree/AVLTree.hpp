#pragma once

#include <cstddef>
#include <cstdint>

#include "components/AVLNode.hpp"

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
         * Searches for a node in the AVLTree with the given key. 
         * 
         * @param key   AVLNode whose container stores the key.
         * @param cmp   Function that compares two nodes. Should return < 0 if first node < second node, > 0 if first 
         *              node > second node, and 0 if the two are equal.
         * 
         * @return  A pointer to the node if found.
         *          NULL if the node is not found.
         */
        AVLNode *lookup(AVLNode *key, int32_t (*cmp)(AVLNode *, AVLNode *));

        /**
         * Removes the node with the given key from the AVLTree.
         * 
         * @param key   AVLNode whose container stores the key.
         * @param cmp   Function that compares two nodes. Should return < 0 if first node < second node, > 0 if first 
         *              node > second node, and 0 if the two are equal.
         * 
         * @return  Pointer to the node that was removed.
         *          NULL if a node with the key does not exist in the AVLTree.
         */
        AVLNode *remove(AVLNode *key, int32_t (*cmp)(AVLNode *, AVLNode *));

        /**
         * Finds the first node in the AVLTree with a key greater than or equal to the given key. 
         * 
         * @param key   AVLNode whose container stores the key.
         * @param cmp   Function that compares two nodes. Should return < 0 if first node < second node, > 0 if first 
         *              node > second node, and 0 if the two are equal.
         * 
         * @return  Pointer to the node if found.
         *          NULL if no such node exists in the tree.
         */
        AVLNode *find_first_ge(AVLNode *key, int32_t (*cmp)(AVLNode *, AVLNode *));

        /**
         * Finds the node located offset positions away from the specified node in its AVLTree.
         *
         * @param node      Pointer to the starting node.
         * @param offset    Number of positions away the new node is from the starting node.
         *
         * @return  Pointer to the node if found.
         *          NULL if no such node exists in the tree.
         */
        static AVLNode *find_offset(AVLNode *node, int64_t offset);

        /**
         * Removes the node from its AVLTree.
         * 
         * @param node  Pointer to the node to remove.
         * 
         * @return  Pointer to the root of the AVLTree.
         */
        static AVLNode *remove(AVLNode *node);

        /**
         * Finds the rank (position in sorted order) of the given node in its AVLTree.
         * 
         * The rank is 0-based, so the lowest node is rank 0.
         * 
         * @param node  Pointer to the node to rank.
         * 
         * @return  The rank of the node.
         */
        static uint64_t rank(AVLNode *node);
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
