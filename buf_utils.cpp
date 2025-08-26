#include <cstring>
#include <netinet/in.h>
#include <string>

#include "buf_utils.hpp"

// Helpers for reading/writing various data types to a buffer used in send()/recv()

void read_uint32(uint32_t *dest, const char **src) {
    memcpy(dest, *src, 4);
    *dest = ntohl(*dest);
    *src += 4;
}

void read_str(std::string &dest, uint32_t str_len, const char **src) {
    dest.assign(*src, *src + str_len);
    *src += str_len;
}

void write_uint32(char **dest, uint32_t src) {
    uint32_t nbe = htonl(src);
    memcpy(*dest, &nbe, 4);
    *dest += 4;
}

void write_str(char **dest, const std::string &src) {
    memcpy(*dest, src.data(), src.length());
    *dest += src.length();
}
