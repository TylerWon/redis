#include <format>

#include "Response.hpp"
#include "buf_utils.hpp"

Response::Response(ResponseStatus status, const std::string message) {
    Response::status = status;
    Response::message = message;
}

void Response::serialize(char *buf, uint32_t *n) {
    *n = 4 + 4 + 4 + Response::message.length();

    write_uint32(&buf, *n-4); // Substract response length header
    write_uint32(&buf, (uint32_t) Response::status);
    write_uint32(&buf, message.length());
    write_str(&buf, Response::message);
}

Response Response::deserialize(const char *buf) {
    buf += 4; // Skip over response length

    Response::ResponseStatus status;
    read_uint32((uint32_t *) &status, &buf);

    uint32_t message_len;
    read_uint32(&message_len, &buf);

    std::string message;
    read_str(message, message_len, &buf);

    return Response(status, message);
}

std::string Response::to_string() {
    return std::format("status: {}, message: {}", (uint32_t) Response::status, Response::message);
}
 