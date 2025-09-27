#include "NilResponse.hpp"

void NilResponse::serialize(Buffer &buf) {
    buf.append_uint8(TAG_NIL);
}

NilResponse* NilResponse::deserialize(const char *buf, uint32_t n) {
    return new NilResponse();
}

uint32_t NilResponse::length() {
    return TAG_SIZE;
}

std::string NilResponse::to_string() {
    return "nil";
}
