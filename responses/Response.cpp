#include "Response.hpp"
#include "NilResponse.hpp"
#include "StrResponse.hpp"
#include "ErrResponse.hpp"
#include "IntResponse.hpp"
#include "../buf_utils.hpp"

Response::MarshalStatus Response::marshal(Buffer &buf) {
    if (length() > MAX_LEN) {
        return MarshalStatus::RES_TOO_BIG;
    }
    buf.append_uint32(length());
    serialize(buf);
    return MarshalStatus::SUCCESS;
}

std::pair<std::optional<Response *>, Response::UnmarshalStatus> Response::unmarshal(const char *buf, uint32_t n) {
    if (n < HEADER_SIZE) {
        return std::make_pair(std::nullopt, UnmarshalStatus::INCOMPLETE_RES);
    }

    uint32_t res_len;
    read_uint32(&res_len, &buf);

    if (res_len > MAX_LEN) {
        return std::make_pair(std::nullopt, UnmarshalStatus::RES_TOO_BIG);
    } else if (n < HEADER_SIZE + res_len) {
        return std::make_pair(std::nullopt, UnmarshalStatus::INCOMPLETE_RES);
    }

    uint8_t res_tag;
    read_uint8(&res_tag, &buf);
    buf -= 1;

    switch (res_tag) {
        case ResponseTag::TAG_NIL:
            return std::make_pair(NilResponse::deserialize(buf), UnmarshalStatus::SUCCESS);
        case ResponseTag::TAG_STR:
            return std::make_pair(StrResponse::deserialize(buf), UnmarshalStatus::SUCCESS);
        case ResponseTag::TAG_ERR:
            return std::make_pair(ErrResponse::deserialize(buf), UnmarshalStatus::SUCCESS);
        case ResponseTag::TAG_INT:
            return std::make_pair(IntResponse::deserialize(buf), UnmarshalStatus::SUCCESS);
        default:
            return std::make_pair(std::nullopt, UnmarshalStatus::INVALID_RES);
    }
}
