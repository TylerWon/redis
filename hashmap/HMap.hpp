#include <string>
#include <cstdint>

#include "components/HTable.hpp"
#include "components/HNode.hpp"

/* Dynamic hashtable using progressive rehashing */
class HMap {
    private:
        static const uint8_t INITIAL_SIZE = 8;
        static const uint8_t MAX_LOAD_FACTOR = 1;
        static const uint8_t NUM_KEYS_TO_MIGRATE = 128;

        HTable *newer;
        HTable *older;
        uint64_t migrate_pos; // last slot in older that keys were migrated from during progressive rehashing
    public: 
        /**
         * Initializes a HMap with INITIAL_SIZE slots.
         */
        HMap();

        /**
         * Inserts the node into the HMap.
         * 
         * Doubles HMap size if load factor exceeds MAX_LOAD_FACTOR after insert, but does not rehash all keys 
         * immediately. Instead, rehashing occurs progressively; a constant number of keys moved from old table to new 
         * table every time an insert, lookup, or remove is performed until the old table is empty. 
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

        /* Returns the number of keys in the HMap */
        uint32_t length();
    private:
        /** 
         * Migrates a constant number of keys (NUM_KEYS_TO_MIGRATE) from the old hashtable to the new one. 
         */
        void migrate_keys();

        /**
         * Moves the newer hashtable to older and allocates a bigger hashtable (2x) for newer.
         */
        void resize();

    #ifdef TEST_MODE
    public:      
        HTable *get_newer() { return newer; }

        HTable *get_older() { return older; }

        uint64_t get_migrate_pos() { return migrate_pos; }

        static uint8_t get_initial_size() { return INITIAL_SIZE; }

        static uint8_t get_max_load_factor() { return MAX_LOAD_FACTOR; }

        static uint8_t get_num_keys_to_migrate() { return NUM_KEYS_TO_MIGRATE; }
    #endif
};
