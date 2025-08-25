#include <cstdint>

/**
 * Reads a uint32_t from the src buffer, storing it in dest. Moves the buffer pointer by 4 after.
 * 
 * @param dest  Pointer a uint32_t where the result will be stored.
 * @param src   Double pointer to a char buffer where the uint32_t will be read from.
 */
void read_uint32(uint32_t *dest, const char **src);

/**
 * Reads a string of length str_len from the src buffer, storing it in dest. Moves the buffer pointer by str_len after.
 * 
 * @param dest      Reference to a string where the result will be stored.
 * @param str_len   The length of the string to read.
 * @param src       Double pointer to a char buffer where the string will be read from.
 */
void read_str(std::string &dest, uint32_t str_len, const char **src);

/**
 * Writes a uint32_t to the dest buffer. Moves the buffer pointer by 4 after.
 * 
 * @param dest  Double pointer a char buffer where the uint32_t will be written. 
 * @param src   The uint32_t to write.
 */
void write_uint32(char **dest, uint32_t src);

/**
 * Writes a string to the dest buffer. Moves the buffer pointer by the length of the string after.
 * 
 * @param dest      Double pointer a char buffer where the string will be written. 
 * @param src       The string to write.
 */
void write_str(char **dest, const std::string &src);
