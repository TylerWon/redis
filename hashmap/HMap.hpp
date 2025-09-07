#include <string>
#include <cstdint>

typedef struct HNode {
    HNode *next = NULL;
    uint64_t hval = 0; // hash value
} HNode;

/* Returns a pointer to the container of an HNode */
#define container_of(ptr, T, member) \
    ((T *)((char *)ptr - offsetof(T, member)))

/* Dynamic hashtable using progressive rehashing */
class HMap {
    private:
        /* A fixed-size chaining hashtable using linked lists */
        class HTable {
            private:
                uint64_t mask; // num_slots - 1, used to perform modulo without division
            public:
                HNode **table;    // array of linked lists
                uint64_t num_slots;  
                uint64_t num_keys;
            
                /**
                 * Initializes an HTable.
                 * 
                 * @param n     Size of the HTable. Must be a power of 2.
                 */
                HTable(uint64_t n);

                /**
                 * Inserts the node into the HTable.
                 * 
                 * @param node  The node to insert.
                 */
                void insert(HNode *node);

                /**
                 * Searches for the key in the HTable and returns its node if found.
                 * 
                 * @param key   A HNode containing the key to search for.
                 * @param eq    A function that checks for the equality of two nodes.
                 * 
                 * @return  If the node is found, the address of the pointer that points to the node (i.e. address of 'next' in 
                 *          the node's parent or the slot in the table if the node is the head of the slot's linked list).
                 *          NULL if the node is not found.
                 */
                HNode **lookup(HNode *key, bool (*eq)(HNode *, HNode *));

                /**
                 * Detaches the node pointed to by *from from the linked list it is a part of.
                 * 
                 * @param from  The address of the pointer that points to the node. 
                 * 
                 * @return  A pointer to the detached node.
                 */
                HNode *detach(HNode **from);
        };

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
