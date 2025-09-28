#include "StrResponse.hpp"
#include "../utils/buf_utils.hpp"

void StrResponse::serialize(Buffer &buf) {
    buf.append_uint8(ResponseTag::TAG_STR);
    buf.append_uint32(len);
    buf.append(msg.data(), len);
}

StrResponse* StrResponse::deserialize(const char *buf) {
    buf += 1; // skip tag

    uint32_t len;
    read_uint32(&len, &buf);

    std::string msg;
    read_str(msg, len, &buf);

    return new StrResponse(msg);
}

uint32_t StrResponse::length() {
    return TAG_SIZE + LEN_SIZE + len;
}

std::string StrResponse::to_string() {
    return msg;
}

std::string StrResponse::get_msg() {
    return msg;
}