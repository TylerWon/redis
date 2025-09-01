#include <format>

#include "Response.hpp"
#include "buf_utils.hpp"

Response::Response(ResponseStatus status, const std::string message) {
    Response::status = status;
    Response::message = message;
}

void Response::serialize(char **buf, uint32_t *buf_len) {
    uint32_t res_len = length();
    if (res_len > MAX_RES_LEN) {
        *buf = NULL;
        return;
    }

    *buf_len = RES_LEN_HEADER_SIZE + res_len;
    *buf = (char *) malloc(*buf_len);
    char *b = *buf; // Use b instead of buf when writing because pointer gets incremented

    write_uint32(&b, res_len);
    write_uint32(&b, (uint32_t) status);
    write_uint32(&b, message.length());
    write_str(&b, message);
}

Response::DeserializationStatus Response::deserialize(const char *buf, uint32_t buf_len, Response **response) {
    if (buf_len < RES_LEN_HEADER_SIZE) {
        return DeserializationStatus::INCOMPLETE_RES;
    }

    uint32_t res_len;
    read_uint32(&res_len, &buf);

    if (res_len > MAX_RES_LEN) {
        return DeserializationStatus::RES_TOO_LARGE;
    } else if (buf_len < RES_LEN_HEADER_SIZE + res_len) {
        return DeserializationStatus::INCOMPLETE_RES;
    }

    ResponseStatus status;
    read_uint32((uint32_t *) &status, &buf);

    uint32_t message_len;
    read_uint32(&message_len, &buf);

    std::string message;
    read_str(message, message_len, &buf);

    *response = new Response(status, message);

    return DeserializationStatus::SUCCESS;
}

std::string Response::to_string() {
    return std::format("status: {}, message: {}", (uint32_t) status, message);
}

uint32_t Response::length() {
    return STATUS_SIZE + MSG_LEN_SIZE + message.length();
}
 