#include "IntResponse.hpp"
#include "buf_utils.hpp"

void IntResponse::serialize(Buffer &buf) {
    buf.append_uint8(TAG_INT);
    buf.append_uint32(num);
}

IntResponse* IntResponse::deserialize(const char *buf, uint32_t n) {
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
