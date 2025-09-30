#include "Request.hpp"
#include "CmdRequest.hpp"
#include "../utils/buf_utils.hpp"

Request::MarshalStatus Request::marshal(Buffer &buf) {
    if (length() > MAX_LEN) {
        return MarshalStatus::REQ_TOO_BIG;
    }
    buf.append_uint32(length());
    serialize(buf);
    return MarshalStatus::SUCCESS;
}

std::pair<std::optional<Request *>, Request::UnmarshalStatus> Request::unmarshal(const char *buf, uint32_t n) {
    if (n < HEADER_SIZE) {
        return std::make_pair(std::nullopt, UnmarshalStatus::INCOMPLETE_REQ);
    }

    uint32_t req_len;
    read_uint32(&req_len, &buf);

    if (req_len > MAX_LEN) {
        return std::make_pair(std::nullopt, UnmarshalStatus::REQ_TOO_BIG);
    } else if (n < HEADER_SIZE + req_len) {
        return std::make_pair(std::nullopt, UnmarshalStatus::INCOMPLETE_REQ);
    }

    uint8_t res_tag;
    read_uint8(&res_tag, &buf);
    buf -= 1; // undo increment by read_uint8()

    switch (res_tag) {
        case RequestTag::TAG_CMD:
            return std::make_pair(CmdRequest::deserialize(buf), UnmarshalStatus::SUCCESS);
        default:
            return std::make_pair(std::nullopt, UnmarshalStatus::INVALID_REQ);
    }
}
