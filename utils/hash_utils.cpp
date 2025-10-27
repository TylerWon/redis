#include "hash_utils.hpp"

uint64_t str_hash(const std::string &str) {
    uint64_t hash = 0xcbf29ce484222325;
    for (const uint8_t c : str) {
        hash *= 0x100000001b3;
        hash ^= c;
    }
    return hash;
}

uint64_t str_hash(const char *str, uint32_t len) {
    uint64_t hash = 0xcbf29ce484222325;
    for (uint32_t i = 0; i < len; i++) {
        char c = str[i];
        hash *= 0x100000001b3;
        hash ^= c;
    }
    return hash;
}
