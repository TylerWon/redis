#include <string>
#include <cstdint>

#include "components/HTable.hpp"
#include "components/HNode.hpp"

/* Dynamic hashtable using progressive rehashing */
class HMap {
    private:
        const uint8_t MAX_LOAD_FACTOR = 8;
        const uint8_t NUM_KEYS_TO_MIGRATE = 128;

        HTable *newer;
        HTable *older;
        uint64_t migrate_pos; // last slot in older that keys were migrated from during progressive rehashing
    public: 
        HMap();

        /**
         * Inserts the node into the HMap.
         * 
         * @param node  The node to insert.
         */
        void insert(HNode *node);

        /** 
         * Searches for the key in the HMap and returns its node if found.
         * 
         * @param key   A HNode containing the key to search for.
         * @param eq    A function that checks for the equality of two nodes.
         * 
         * @return  A pointer to the node if found.
         *          NULL if the node is not found.
         */
        HNode *lookup(HNode *key, bool (*eq)(HNode *, HNode *));

        /**
         * Removes the key in the HMap.
         * 
         * @param key   A HNode containing the key to remove.
         * @param eq    A function that checks for the equality of two nodes.
         * 
         * @return  The node that was removed.
         *          NULL if the key does not exist in the HMap.
         */
        HNode *remove(HNode *key, bool (*eq)(HNode *, HNode *));

        /**
         * Executes the provided callback function on each of the nodes in the HMap.
         * 
         * @param cb        The callback function.
         * @param cb_arg    An argument for the callback. Void pointer type allows flexibility in argument type for 
         *                  different callbacks.
         */
        void for_each(void (*cb)(HNode *, void *), void *cb_arg);
    private:
        /** 
         * Migrates a constant number of keys from the old hashtable to the new one. 
         */
        void migrate_keys();

        /**
         * Moves the newer hashtable to older and allocates a bigger hashtable for newer.
         */
        void resize();
};
