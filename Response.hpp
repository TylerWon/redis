#include <string>
#include <cstdint>

class Response {
    private:
        static const uint32_t STATUS_SIZE = 4;
        static const uint32_t MSG_LEN_SIZE  = 4;
    public:
        enum ResponseStatus {
            RES_OK = 0,
            RES_ERR = 1,
            RES_NX = 2
        };

        enum DeserializationStatus {
            SUCCESS,
            INCOMPLETE_RES,
            RES_TOO_LARGE
        };
        
        static const uint32_t RES_LEN_HEADER_SIZE  = 4;
        static const uint32_t MAX_RES_LEN = 4096;

        ResponseStatus status;
        std::string message;

        Response(ResponseStatus status, const std::string message);

        /**
         * Serializes the Response.
         * 
         * Serialized format:
         * - Response length header (4 bytes)
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
         * @param response  Double pointer to a Response where the result of the operation will be stored. Should be
         *                  freed when no longer in use.
         * 
         * @return  SUCCESS if a Response is deserialized from the buffer.
         *          INCOMPLETE_REQ if the buffer contains an incomplete Response.
         *          RES_TOO_LARGE if the buffer contains a Response that exceeds the size limit.
         */
        static DeserializationStatus deserialize(const char *buf, uint32_t buf_len, Response **response);

        /* Returns the Response as a string. */
        std::string to_string();

        /* Returns the length of the Response (in bytes) */
        uint32_t length();
};
