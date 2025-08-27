#include <string>
#include <cstdint>

class Response {
    private:
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
         * @param buf   Pointer to a char buffer where the serialized Response will be stored.
         * @param n     Pointer to a uint32_t where the length of the buffer will be stored.
         */
        void serialize(char *buf, uint32_t *n);

        /**
         * Deserializes the Response in the provided buffer.
         * 
         * @param buf   Pointer to a char buffer where the Response is stored.
         * 
         * @return  The Request.
         */
        static Response deserialize(const char *buf);

        /**
         * Returns the Response as a string.
         * 
         * @return  The Response as a string.
         */
        std::string to_string();
};
