#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "net_utils.h"
#include "log.h"
#include "constants.h"

/**
 * Gets the address info for the server which can be used in bind().
 * 
 * @return  Pointer to a struct addrinfo on success. Should be freed when no longer in use.
 *          NULL on error.
 */
struct addrinfo *get_server_addr_info() {
    struct addrinfo *res;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;        // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;    // Stream socket
    hints.ai_flags = AI_PASSIVE;        // Returns wildcard address

    if (getaddrinfo(NULL, PORT, &hints, &res) != 0) {
        return NULL;
    }

    return res;
}

/**
 * Starts the server by creating a listener socket bound to a pre-defined port. 
 * 
 * @param res   Pointer to a struct addrinfo containing the addrinfo for the server.
 * 
 * @return  The listener socket on success.
 *          -1 on error.
 */
int start_server(struct addrinfo *res) {
    struct addrinfo *p;
    int listener;
    for (p = res; p != NULL; p = p->ai_next) {
        if ((listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("socket()");
            continue;
        }

        int yes = 1;
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));  // Allows port to be re-used

        if (bind(listener, p->ai_addr, p->ai_addrlen) == -1) {
            perror("bind()");
            close(listener);
            continue;
        }

        if (listen(listener, SOMAXCONN) == -1) {
            perror("listen()");
            close(listener);
            continue;
        }

        return listener;
    }

    return -1;
}

/**
 * Receives a message from a client.
 * 
 * @param client    The client socket.
 * @param buf       Pointer to a char buffer where the message will be stored.
 * @param buf_len   Pointer to a uint32_t where the buffer length will be stored.
 * 
 * @return  0 on success.
 *          -1 on error.
 */
int recv_message(int server, char *buf, uint32_t *buf_len) {
    if (recv_all(server, buf, 4) == -1) {
        perror("failed to receive message length");
        return -1;
    }

    uint32_t msg_len_nbe;
    memcpy(&msg_len_nbe, buf, 4);
    uint32_t msg_len = ntohl(msg_len_nbe);
    if (msg_len > MAX_MSG_LEN) {
        printf("message is too long\n");
        return -1;
    }

    if (recv_all(server, buf+4, msg_len) == -1) {
        perror("failed to receive message");
        return -1;
    }

    *buf_len = 4 + msg_len;

    return 0;
}

int main() {
    struct addrinfo *res = get_server_addr_info();
    if (res == NULL) {
        kill("failed to get server's addrinfo");
    }

    int listener;
    if ((listener = start_server(res)) == -1) {
        kill("failed to start server");
    }

    log("started server");

    int client;
    if ((client = accept(listener, NULL, NULL)) == -1) {
        kill("failed to accept client connection");
    }

    log("accepted connection");

    while (true) {
        char buf[4 + MAX_MSG_LEN];
        uint32_t buf_len;
        if (recv_message(client, buf, &buf_len) == -1) {
            kill("failed to receive message");
        }

        log("received message");

        if (send_all(client, buf, buf_len) == -1) {
            kill("failed to send reply");
        }

        log("sent reply");
    }
}