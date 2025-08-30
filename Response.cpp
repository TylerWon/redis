#include <format>

#include "Response.hpp"
#include "buf_utils.hpp"

Response::Response(ResponseStatus status, const std::string message) {
    Response::status = status;
    Response::message = message;
}

void Response::serialize(char **buf, uint32_t *buf_len) {
    uint32_t res_len = Response::length();
    if (res_len > Response::MAX_RES_LEN) {
        *buf = NULL;
        return;
    }

    *buf_len = Response::RES_LEN_SIZE + res_len;
    *buf = (char *) malloc(*buf_len);
    char *b = *buf; // Use b instead of buf when writing because pointer gets incremented

    write_uint32(&b, res_len);
    write_uint32(&b, (uint32_t) Response::status);
    write_uint32(&b, Response::message.length());
    write_str(&b, Response::message);
}

Response *Response::deserialize(const char *buf, uint32_t buf_len, int *res) {
    *res = 1;

    if (buf_len < Response::RES_LEN_SIZE) {
        *res = 0;
        return NULL;
    }

    uint32_t res_len;
    read_uint32(&res_len, &buf);

    if (res_len > Response::MAX_RES_LEN) {
        *res = -1;
        return NULL;
    } else if (buf_len < Response::RES_LEN_SIZE + res_len) {
        *res = 0;
        return NULL;
    }

    Response::ResponseStatus status;
    read_uint32((uint32_t *) &status, &buf);

    uint32_t message_len;
    read_uint32(&message_len, &buf);

    std::string message;
    read_str(message, message_len, &buf);

    return new Response(status, message);
}

std::string Response::to_string() {
    return std::format("status: {}, message: {}", (uint32_t) Response::status, Response::message);
}

uint32_t Response::length() {
    return Response::STATUS_SIZE + Response::MSG_LEN_SIZE + Response::message.length();
}
 