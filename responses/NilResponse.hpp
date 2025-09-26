#include "Response.hpp"

/* A Nil response */
class NilResponse : public Response {
    protected:
        void serialize(Buffer &buf) override;
        uint32_t length() override;
    public:
        NilResponse() {};

        /**
         * Deserializes a NilResponse from the provided byte buffer.
         * 
         * @param buf   Pointer to a byte buffer that stores the Response.
         * @param n     Size of the buffer.
         * 
         * @return  The Response.
         */
        static Response* deserialize(const char *buf, uint32_t n);

        std::string to_string() override;
};
