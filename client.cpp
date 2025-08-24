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
 * Gets the server's address info which can be used in connect(). 
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

    if (getaddrinfo(NULL, PORT, &hints, &res) != 0) { // node == NULL so loopback address is returned
        return NULL;
    }

    return res;
}

/**
 * Connects to the server using the addrinfo provided in res.
 * 
 * @param res   Pointer to a struct addrinfo containing the addrinfo for the server.
 * 
 * @return  The socket for the connection on success.
 *          -1 on error.
 */
int connect_to_server(struct addrinfo *res) {
    struct addrinfo *p;
    int server;
    for (p = res; p != NULL; p = p->ai_next) {
        if ((server = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("socket()");
            continue;
        }

        if (connect(server, p->ai_addr, p->ai_addrlen) == -1) {
            perror("connect()");
            close(server);
            continue;
        }

        return server;
    }

    return -1;
}

/**
 * Serializes a message to be sent to the server.
 * 
 * Message format:
 * - Message length (4 bytes)
 * - Message (max 4086 bytes)
 * 
 * @param msg       The message.
 * @param msg_len   The length of the message.
 * @param buf       Pointer to a char buffer where the serialized message will be stored. 
 */
void serialize(const char *msg, uint32_t msg_len, char *buf) {
    char *b = buf;

    uint32_t msg_len_nbe = htonl(msg_len);
    memcpy(b, &msg_len_nbe, 4);
    b += 4;

    memcpy(b, msg, msg_len);
}

/**
 * Sends a request to the server.
 * 
 * @param server    The server socket.
 * @param msg       The request message.
 * 
 * @return  0 on success.
 *          -1 on error.
 */
int send_request(int server, const char *msg) {
    uint32_t msg_len = (uint32_t) strlen(msg);
    if (msg_len > MAX_MSG_LEN) {
        printf("message is too long\n");
        return -1;
    }

    char buf[4 + MAX_MSG_LEN];
    serialize(msg, msg_len, buf);

    if (send_all(server, buf, 4 + msg_len) == -1) {
        perror("failed to send message");
        return -1;
    }

    printf("sent: %s\n", msg);

    return 0;
}

/**
 * Receives a response from the server. The server repeats the message sent by the client for its response.
 * 
 * @param server    The server socket.
 * 
 * @return  0 on success.
 *          -1 on error.
 */
int recv_response(int server) {
    char buf[4 + MAX_MSG_LEN];
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

    printf("received: %s\n", buf+4);

    return 0;
}

int main() {
    struct addrinfo *res = get_server_addr_info();
    if (res == NULL) {
        kill("failed to get server's addrinfo");
    }

    int server;
    if ((server = connect_to_server(res)) == -1) {
        kill("failed to connect to server");
    }
    
    freeaddrinfo(res);

    log("connected to server");

    if (send_request(server, "hello") == -1) {
        kill("failed to send message");
    }

    if (recv_response(server) == -1) {
        kill("failed to receive response");
    }

    if (send_request(server, "another message") == -1) {
        kill("failed to send message");
    }

    if (recv_response(server) == -1) {
        kill("failed to receive response");
    }

    return 0;
}
