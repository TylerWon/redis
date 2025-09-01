#include <cstdint>

/* Dynamic char buffer with constant time append and remove from front */
class Buffer {
    private:
        char *buffer_start;
        char *buffer_end;
        char *data_start;
        char *data_end;
    public:
        Buffer();

        /**
         * Appends n bytes from the provided array to the Buffer.
         * 
         * @param arr   Pointer to a char array which contains the data to append.
         * @param n     The number of bytes to append to the Buffer from arr.
         */
        void append(const char *arr, uint32_t n);

        /**
         * Removes n bytes from the front of the Buffer.
         * 
         * @param n The number of bytes to remove from the front of the Buffer.
         */
        void consume(uint32_t n);

        /* Returns a direct pointer to the memory array used internally by the Buffer to store its own elements */
        char *data();

        /* Returns the number of elements in the Buffer */
        uint32_t size();
};
