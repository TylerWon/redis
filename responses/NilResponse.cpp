#include "NilResponse.hpp"

Response* NilResponse::deserialize(const char *buf, uint32_t n) {
    return new NilResponse();
}

void NilResponse::serialize(Buffer &buf) {
    buf.append_uint8(TAG_NIL);
}

uint32_t NilResponse::length() {
    return TAG_SIZE;
}

std::string NilResponse::to_string() {
    return "nil";
}
