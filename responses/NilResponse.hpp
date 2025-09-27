#include "Response.hpp"

/* A Nil response */
class NilResponse : public Response {
    public:
        /**
         * Deserializes a NilResponse from the provided byte buffer.
         * 
         * @param buf   Pointer to a byte buffer that stores the NilResponse.
         * @param n     Size of the buffer.
         * 
         * @return  The Response.
         */
        static Response* deserialize(const char *buf, uint32_t n);

        void serialize(Buffer &buf) override;
        
        uint32_t length() override;

        std::string to_string() override;
};
