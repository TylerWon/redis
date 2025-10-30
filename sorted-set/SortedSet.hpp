#include "./components/SPair.hpp"
#include "../avl-tree/AVLTree.hpp"
#include "../hashmap/HMap.hpp"

/* A collection of sorted (score, name) pairs */
class SortedSet {
    public:
        /**
         * Inserts a new (score, name) pair into the SortedSet if it doesn't already exist. Otherwise, updates the 
         * existing pair.
         * 
         * @param name  Byte array that stores the name.
         * @param len   Length of the name.
         * @param score The score.
         * 
         * @return  True if the a new pair is inserted.
         *          False if the pair already exists.
         */
        bool insert(const char *name, uint32_t len, double score);

        /** 
         * Searches for a pair with the given name in the SortedSet.
         * 
         * @param name  Byte array that stores the name.
         * @param len   Length of the name.
         * 
         * @return  A pointer to the pair if found.
         *          NULL if a pair is not found.
         */
        SPair *lookup(const char *name, uint32_t len);

        /**
         * Removes the pair from the SortedSet.
         * 
         * @param pair  Pointer to the pair to remove.
         */
        void remove(SPair *pair);

        /* Returns the number of pairs in the SortedSet */
        uint32_t length();
    private:
        HMap map; // used for point queries
        AVLTree tree; // used for range and rank queries

        /**
         * Updates the given pair and adjusts its order in the SortedSet.
         * 
         * @param pair  Pointer to the pair to update.
         * @param score New score for the pair.
         */
        void update(SPair *pair, double score);
};
