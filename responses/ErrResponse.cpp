#include "ErrResponse.hpp"
#include <buf_utils.hpp>
#include <format>

ErrResponse* ErrResponse::deserialize(const char *buf, uint32_t n) {
    buf += 1; // skip tag

    ErrorCode code;
    read_uint8((uint8_t *) &code, &buf);

    StrResponse* str_response = StrResponse::deserialize(buf, n - TAG_SIZE - ERR_CODE_SIZE);
    std::string msg = str_response->get_msg();
    delete str_response;

    return new ErrResponse(code, msg);
}

void ErrResponse::serialize(Buffer &buf) {
    buf.append_uint8(TAG_ERR);
    buf.append_uint8(code);
    str_response.serialize(buf);
}

uint32_t ErrResponse::length() {
    return TAG_SIZE + ERR_CODE_SIZE + str_response.length();
}

std::string ErrResponse::to_string() {
    return std::format("(error) {}", str_response.get_msg());
}

ErrResponse::ErrorCode ErrResponse::get_err_code() {
    return code;
}

std::string ErrResponse::get_err_msg() {
    return str_response.get_msg();
}
