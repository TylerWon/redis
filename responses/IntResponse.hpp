#include "Response.hpp"

/* An integer response */
class IntResponse : public Response {
    private:
        const uint8_t NUM_SIZE = 4;

        uint32_t num;
    public:
        IntResponse(uint32_t num) : num(num) {};

        /**
         * Serialized structure:
         * +----------+--------------+
         * | tag (1B) | integer (4B) |
         * +----------+--------------+
         */
        void serialize(Buffer &buf) override;

        /**
         * Deserializes an IntResponse from the provided byte buffer.
         * 
         * @param buf   Pointer to a byte buffer that stores the IntResponse.
         * @param n     Size of the buffer.
         * 
         * @return  The Response.
         */
        static IntResponse* deserialize(const char *buf, uint32_t n);
        
        uint32_t length() override;

        std::string to_string() override;

        /* Returns the integer */
        uint32_t get_int();
};
