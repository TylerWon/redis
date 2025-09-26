#include "StrResponse.hpp"
#include "buf_utils.hpp"

StrResponse::StrResponse(std::string msg) {
    message = msg;
    len = message.length();
}

void StrResponse::serialize(Buffer &buf) {
    buf.append_uint8(TAG_STR);
    buf.append_uint32(len);
    buf.append(message.data(), len);
}

uint32_t StrResponse::length() {
    return TAG_SIZE + LEN_SIZE + len;
}

Response* StrResponse::deserialize(const char *buf, uint32_t n) {
    buf += 1; // skip tag

    uint32_t len;
    read_uint32(&len, &buf);

    std::string msg;
    read_str(msg, len, &buf);

    return new StrResponse(msg);
}

std::string StrResponse::to_string() {
    return message;
}
