#include "Response.hpp"

/* A string response */
class StrResponse : public Response {
    private:
        const uint8_t LEN_SIZE = 4;

        std::string message;
        uint32_t len;
    protected:
        void serialize(Buffer &buf) override;
        uint32_t length() override;
    public:
        StrResponse(std::string msg);

        /**
         * Deserializes a StrResponse from the provided byte buffer.
         * 
         * @param buf   Pointer to a byte buffer that stores the StrResponse.
         * @param n     Size of the buffer.
         * 
         * @return  The Response.
         */
        static Response* deserialize(const char *buf, uint32_t n);

        std::string to_string() override;
};
