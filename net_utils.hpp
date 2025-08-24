#include <stdio.h>

/**
 * Sends all n bytes of data in the provided buffer to the socket.
 * 
 * @param sockfd    The socket.
 * @param buf       Pointer to a char buffer containing the data to send.
 * @param n         The number of bytes to send.
 * 
 * @return  0 on success.
 *          -1 on error (and sets errno accordingly).
 */
ssize_t send_all(int sockfd, char *buf, size_t n);

/**
 * Receives all n bytes of data from the socket and stores it in the provided buffer.
 * 
 * @param sockfd    The socket.
 * @param buf       Pointer to a char buffer where the data will be stored.
 * @param n         The number of bytes to receive.
 * 
 * @return  1 on success.
 *          0 when connection has been closed.
 *          -1 on error (and sets errno accordingly).
 */
int recv_all(int sockfd, char *buf, size_t n);
