#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <optional>

#include "../buffer/Buffer.hpp"

/* Base request class */
class Request {
    protected:
        static const uint8_t TAG_SIZE = 1;

        /* Identifies the type of request when serialized */
        enum RequestTag {
            TAG_CMD
        };
    public:
        static const uint8_t HEADER_SIZE = 4;
        static const uint32_t MAX_LEN = 4096;

        enum class MarshalStatus {
            SUCCESS,
            REQ_TOO_BIG
        };

        enum class UnmarshalStatus {
            SUCCESS,
            INCOMPLETE_REQ,
            REQ_TOO_BIG,
            INVALID_REQ
        };

        virtual ~Request() = default; // ensures destructor of concrete subclass is called

        /**
         * Marshals the Request into a packet to be sent over the network.
         * 
         * Packet structure:
         * +--------------------+----------------------+
         * | length header (4B) | data (variable size) |
         * +--------------------+----------------------+
         * 
         * @param buf   The Buffer that will store the packet.
         * 
         * @return  SUCCESS on success.
         *          REQ_TOO_BIG when the Request exceeds the size limit.
         */
        MarshalStatus marshal(Buffer &buf);

        /**
         * Unmarshals a Request from a Request packet in the provided byte buffer.
         * 
         * @param buf   Pointer to a byte buffer that stores the Request packet.
         * @param n     Size of the buffer.
         * 
         * @return  (Request, SUCCESS) on success.
         *          (NULL, INCOMPLETE_REQ) when the buffer contains an incomplete Request.
         *          (NULL, REQ_TOO_BIG) when the Request in the buffer exceeds the size limit.
         *          (NULL, INVALID_REQ) when the Request is not one of the Request tags.
         */
        static std::pair<std::optional<Request *>, UnmarshalStatus> unmarshal(const char *buf, uint32_t n);

        /**
         * Serializes the Request.
         * 
         * @param buf   The Buffer that will store the serialized Request.
         */
        virtual void serialize(Buffer &buf) = 0;

        /* Returns the length of the Request */
        virtual uint32_t length() = 0;
        
        /* Returns the Request as a string */
        virtual std::string to_string() = 0;

        /* Returns the command */
        virtual std::vector<std::string> get_cmd() = 0;
};
