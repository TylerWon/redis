#include <cstddef>
#include <cstdint>

/* Node in an AVLTree */
class AVLNode {
    public:
        AVLNode *parent = NULL;
        AVLNode *left = NULL;
        AVLNode *right = NULL;
        uint32_t height = 1;

        /* Updates the height of the AVLNode. */
        void update_height();

        /* Returns the balance factor for the AVLNode. */
        int8_t balance_factor();
    private:
        /* Returns the height of the given AVLNode */
        uint32_t get_height(AVLNode *root);
};
