#include <vector>
#include <string>
#include <cstdint>

class Request {
    public:
        static const uint32_t MAX_REQ_LEN = 4096;

        std::vector<std::string> command;

        Request(const std::vector<std::string> &command);

        /**
         * Serializes the Request.
         * 
         * Serialized format:
         * - Request length (4 bytes)
         * - Number of strings (4 bytes)
         * - Length of string 1 (4 bytes)
         * - String 1 (variable size)
         * - Length of string 2 (4 bytes)
         * - etc.
         * 
         * @param buf   Pointer to a char buffer where the serialized request will be stored.
         * @param n     Pointer to a uint32_t where the length of the buffer will be stored.
         */
        void serialize(char *buf, uint32_t *n);

        /**
         * Deserializes the Request in the provided buffer.
         * 
         * @param buf   Pointer to a char buffer where the request is stored.
         * 
         * @return  The Request.
         */
        static Request deserialize(const char *buf);

        /**
         * Returns the Request as a string.
         * 
         * @return  The Request as a string.
         */
        std::string to_string();
};
