#include "CmdRequest.hpp"
#include "../utils/buf_utils.hpp"
#include "../utils/log.hpp"

void CmdRequest::serialize(Buffer &buf) {
    buf.append_uint8(RequestTag::TAG_CMD);
    buf.append_uint32(len);
    for (const std::string &str : cmd) {
        uint32_t str_len = str.length();
        buf.append_uint32(str_len);
        buf.append(str.data(), str_len);
    }
}

CmdRequest* CmdRequest::deserialize(const char *buf) {
    buf += 1; // skip tag

    uint32_t len;
    read_uint32(&len, &buf);

    std::vector<std::string> cmd;
    for (uint32_t i = 0; i < len; i++) {
        uint32_t str_len;
        read_uint32(&str_len, &buf);

        std::string str;
        read_str(str, str_len, &buf);

        cmd.push_back(str);
    }

    return new CmdRequest(cmd);
}

uint32_t CmdRequest::length() {
    uint32_t str_size = 0;
    for (const std::string &str : cmd) {
        str_size += STR_LEN_SIZE + str.length();
    }
    return RequestTag::TAG_CMD + LEN_SIZE + str_size;
}

std::string CmdRequest::to_string() {
    if (len < 1) {
        return "";
    }

    std::string cmd_str = cmd[0];
    for (uint32_t i = 1; i < len; i++) {
        cmd_str += " ";
        cmd_str += cmd[i];
    }

    return cmd_str;
}

std::vector<std::string> CmdRequest::get_cmd() {
    return cmd;
}
