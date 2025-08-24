#include <sys/socket.h>

#include "net_utils.hpp"

ssize_t send_all(int sockfd, const char *buf, size_t n) {
    while (n > 0) {
        ssize_t sent = send(sockfd, buf, n, 0);
        if (sent == -1) {
            return sent;
        }
        n -= (size_t) sent;
        buf += sent;
    }

    return 0;
}

int recv_all(int sockfd, char *buf, size_t n) {
    while (n > 0) {
        ssize_t recvd = recv(sockfd, buf, n, 0);
        if (recvd <= 0) {
            return recvd;
        }
        n -= (size_t) recvd;
        buf += recvd;
    }
    return 0;
}
