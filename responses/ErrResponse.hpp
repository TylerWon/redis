#include "Response.hpp"
#include "StrResponse.hpp"

/* An error response */
class ErrResponse : public Response {
    public:
        enum ErrorCode {
            ERR_UNKNOWN,
            ERR_TOO_BIG
        };

        ErrResponse(ErrorCode code, std::string msg): code(code), str_response(msg) {};

        /**
         * Deserializes an ErrResponse from the provided byte buffer.
         * 
         * @param buf   Pointer to a byte buffer that stores the ErrResponse.
         * @param n     Size of the buffer.
         * 
         * @return  The Response.
         */
        static ErrResponse* deserialize(const char *buf, uint32_t n);

        /**
         * Serialized structure:
         * +----------+-----------+-----------------------------+
         * | tag (1B) | code (1B) | StrResponse (variable size) |
         * +----------+-----------+-----------------------------+
         */
        void serialize(Buffer &buf) override;

        uint32_t length() override;

        std::string to_string() override;

        /* Returns the error code */
        ErrorCode get_err_code();

        /* Returns the error message */
        std::string get_err_msg();
    private:
        static const uint8_t ERR_CODE_SIZE = 1;

        ErrorCode code;
        StrResponse str_response;
};
