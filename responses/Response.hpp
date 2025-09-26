#include <string>
#include <cstdint>
#include <optional>

#include <Buffer.hpp>

/* Base response class */
class Response {
    protected:
        const uint32_t MAX_LEN = 30 << 20; // likely larger than the kernel buffer
        const uint8_t HEADER_SIZE = 4;
        const uint8_t TAG_SIZE = 1;

        /* Identifies the type of response when serialized */
        enum ResponseTag {
            TAG_NIL,
            TAG_ERR,
            TAG_STR,
            TAG_INT
        };
        
        /**
         * Serializes the Response.
         * 
         * @param buf   The Buffer that will store the serialized Response.
         */
        virtual void serialize(Buffer &buf) = 0;

        /* Returns the length of the Response */
        virtual uint32_t length() = 0;
    public:
        enum MarshalStatus {
            SUCCESS,
            RES_TOO_BIG
        };

        enum UnmarshalStatus {
            SUCCESS,
            INCOMPLETE_RES,
            RES_TOO_BIG,
            INVALID_RES
        };
        
        /**
         * Marshals the Response into a packet to be sent over the network.
         * 
         * Packet structure:
         * - Length header (4 bytes): size of the packet
         * - Data (variable length): serialized Response data
         * 
         * @param buf   The Buffer that will store the packet.
         * 
         * @return  SUCCESS on success.
         *          RES_TOO_BIG when the Response exceeds the size limit.
         */
        MarshalStatus marshal(Buffer &buf);

        /**
         * Unmarshals a Response from a response packet in the provided byte buffer.
         * 
         * @param buf   Pointer to a byte buffer that stores the response packet.
         * @param n     Size of the buffer.
         * 
         * @return  (Response, SUCCESS) on success.
         *          (NULL, INCOMPLETE_RES) when the buffer contains an incomplete Response.
         *          (NULL, RES_TOO_BIG) when the Response in the buffer exceeds the size limit.
         *          (NULL, INVALID_RES) when the Response is not one of the Response tags.
         */
        std::pair<std::optional<Response *>, UnmarshalStatus> unmarshal(const char *buf, uint32_t n);

        /* Returns the Response as a string */
        virtual std::string to_string() = 0;
};
