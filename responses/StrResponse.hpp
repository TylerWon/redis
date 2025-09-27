#include "Response.hpp"

/* A string response */
class StrResponse : public Response {
    private:
        const uint8_t LEN_SIZE = 4;

        std::string msg;
        uint32_t len;
    public:
        StrResponse(std::string msg) : msg(msg), len(msg.length()) {};

        /**
         * Deserializes a StrResponse from the provided byte buffer.
         * 
         * @param buf   Pointer to a byte buffer that stores the StrResponse.
         * @param n     Size of the buffer.
         * 
         * @return  The Response.
         */
        static Response* deserialize(const char *buf, uint32_t n);

        /**
         * Serialized structure:
         * +----------+-------------+------------------------+
         * | tag (1B) | length (4B) | string (variable size) |
         * +----------+-------------+------------------------+
         */
        void serialize(Buffer &buf) override;
        
        uint32_t length() override;

        std::string to_string() override;
};
