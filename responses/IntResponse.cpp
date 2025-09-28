#include "IntResponse.hpp"
#include "../utils/buf_utils.hpp"

void IntResponse::serialize(Buffer &buf) {
    buf.append_uint8(ResponseTag::TAG_INT);
    buf.append_uint32(num);
}

IntResponse* IntResponse::deserialize(const char *buf) {
    buf += 1; // skip tag

    uint32_t num;
    read_uint32(&num, &buf);

    return new IntResponse(num);
}

uint32_t IntResponse::length() {
    return TAG_SIZE + NUM_SIZE;
}

std::string IntResponse::to_string() {
    return std::to_string(num);
}

uint32_t IntResponse::get_int() {
    return num;
}
