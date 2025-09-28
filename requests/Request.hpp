#include <vector>
#include <string>
#include <cstdint>

class Request {
    private:
        static const uint32_t NUM_STRS_SIZE = 4;
        static const uint32_t STR_LEN_SIZE  = 4;
    public:
        enum DeserializationStatus {
            SUCCESS,
            INCOMPLETE_REQ,
            REQ_TOO_LARGE
        };

        static const uint32_t REQ_LEN_HEADER_SIZE  = 4;
        static const uint32_t MAX_REQ_LEN = 4096;
        
        std::vector<std::string> command;

        Request(const std::vector<std::string> &command);

        /**
         * Serializes the Request.
         * 
         * Serialized format:
         * - Request length header (4 bytes)
         * - Number of strings (4 bytes)
         * - Length of string 1 (4 bytes)
         * - String 1 (variable size)
         * - Length of string 2 (4 bytes)
         * - etc.
         * 
         * @param buf       Double pointer to a char buffer where the serialized Request will be stored. Should be freed 
         *                  when no longer in use. 
         * @param buf_len   Pointer to a uint32_t where the length of the buffer will be stored.
         * 
         * @return  The serialized Request in buf on success.
         *          NULL in buf if Request exceeds the length limit.
         */
        void serialize(char **buf, uint32_t *buf_len);

        /**
         * Deserializes the Request in the provided buffer.
         * 
         * @param buf       Pointer to a char buffer where the Request is stored.
         * @param buf_len   Number of bytes in the buffer.
         * @param request   Double pointer to a Request where the result of the operation will be stored. Should be
         *                  freed when no longer in use.
         * 
         * @return  SUCCESS if a Request is deserialized from the buffer.
         *          INCOMPLETE_REQ if the buffer contains an incomplete Request.
         *          REQ_TOO_LARGE if the buffer contains a Request that exceeds the size limit.
         */
        static DeserializationStatus deserialize(const char *buf, uint32_t buf_len, Request **request);

        /* Returns the Request as a string. */
        std::string to_string();

        /* Returns the length of the Request (in bytes) */
        uint32_t length();
};
