#include <string>
#include <cstdint>

class Response {
    private:
        // Constants for header sizes
        static const uint32_t RES_LEN_SIZE  = 4;
        static const uint32_t STATUS_SIZE = 4;
        static const uint32_t MSG_LEN_SIZE  = 4;
    public:
        static const uint32_t MAX_RES_LEN = 4096;

        enum ResponseStatus {
            RES_OK = 0,
            RES_ERR = 1,
            RES_NX = 2
        };

        ResponseStatus status;
        std::string message;

        Response(ResponseStatus status, const std::string message);

        /**
         * Serializes the Response.
         * 
         * Serialized format:
         * - Response length (4 bytes)
         * - Status (4 bytes)
         * - Message length (4 bytes)
         * - Message (variable size)
         * 
         * @param buf       Double pointer to a char buffer where the serialized Response will be stored. Should be 
         *                  freed when no longer in use. 
         * @param buf_len   Pointer to a uint32_t where the length of the buffer will be stored.
         * 
         * @return  The serialized Response in buf on success.
         *          NULL in buf if Response exceeds the length limit.
         */
        void serialize(char **buf, uint32_t *buf_len);

        /**
         * Deserializes the Response in the provided buffer.
         * 
         * @param buf       Pointer to a char buffer where the Response is stored.
         * @param buf_len   Number of bytes in the buffer.
         * @param res       Pointer to an int where the result of the operation will be stored.
         * 
         * @return  Pointer to the Response and 1 in res on success. Returned Response should be deleted when no longer 
         *          in use.
         *          NULL and 0 in res if buffer contains half a Response.
         *          NULL and -1 in res if Response in buffer exceeds the length limit.
         */
        static Response *deserialize(const char *buf, uint32_t buf_len, int *res);

        /* Returns the Response as a string. */
        std::string to_string();

        /* Returns the length of the Request (in bytes) */
        uint32_t length();
};
