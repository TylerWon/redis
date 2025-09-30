#pragma once

#include "Request.hpp"

/* A command request */
class CmdRequest : public Request {
    private:
        const uint8_t LEN_SIZE = 4;
        const uint8_t STR_LEN_SIZE = 4;

        std::vector<std::string> cmd;
        uint32_t len;
    public:
        CmdRequest(std::vector<std::string> cmd) : cmd(cmd), len(cmd.size()) {};

        /**
         * Serialized structure:
         * +----------+-------------+------------------+----------------------+-----+------------------+----------------------+
         * | tag (1B) | length (4B) | str1 length (4B) | str1 (variable size) | ... | strn length (4B) | strn (variable size) |
         * +----------+-------------+------------------+----------------------+-----+------------------+----------------------+
         */
        void serialize(Buffer &buf) override;

        /**
         * Deserializes a CmdRequest from the provided byte buffer.
         * 
         * @param buf   Pointer to a byte buffer that stores the CmdRequest.
         * 
         * @return  The Request.
         */
        static CmdRequest* deserialize(const char *buf);
        
        uint32_t length() override;

        std::string to_string() override;

        std::vector<std::string> get_cmd() override;
};
