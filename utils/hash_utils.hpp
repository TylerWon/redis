#include <cstdint>
#include <string>

/**
 * Hashes a string using the FNV-1 hashing algorithm.
 * 
 * Reference: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1_hash
 * 
 * @param str   The string to hash.
 * 
 * @return  The FNV-1 hash value of the string.
 */
uint64_t str_hash(const std::string &str);

/**
 * Hashes a string using the FNV-1 hashing algorithm.
 * 
 * Reference: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1_hash
 * 
 * @param str   Byte array that stores the string to hash.
 * @param len   Length of the string.
 * 
 * @return  The FNV-1 hash value of the string.
 */
uint64_t str_hash(const char *str, uint32_t len);
