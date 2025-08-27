#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <stdexcept>

#include "net_utils.hpp"
#include "log.hpp"
#include "constants.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "buf_utils.hpp"

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
 * Sends a Request to the server.
 * 
 * @param server    The server socket.
 * @param request   The Request.
 * 
 * @return  True on success.
 *          False on failure.
 */
bool send_request(int server, Request request) {
    char buf[4 + MAX_MSG_LEN];
    uint32_t n;
    request.serialize(buf, &n);

    if (send_all(server, buf, n) == -1) {
        perror("failed to send message");
        return false;
    }

    return true;
}

/**
 * Receives a response from the server.
 * 
 * @param server    The server socket.
 * @param buf       Pointer to a char buffer where the response will be stored.
 * 
 * @return  True on success.
 *          False on failure.
 */
bool recv_response(int server, char *buf) {
    if (recv_all(server, buf, 4) == -1) {
        perror("failed to receive response length");
        return false;
    }

    uint32_t len;
    read_uint32(&len, (const char **) &buf);
    if (len > Response::MAX_RES_LEN) {
        printf("response is too long\n");
        return false;
    }

    if (recv_all(server, buf, len) == -1) {
        perror("failed to receive response body");
        return false;
    }

    return true;
}

/**
 * Handles a response from the server.
 * 
 * @param server    The server socket.
 * 
 * @return  True on success.
 *          False on failure.
 */
bool handle_response(int server) {
    char buf[4 + Response::MAX_RES_LEN];
    if (!recv_response(server, buf)) {
        log("failed to receive response");
        return false;
    }

    Response response = Response::deserialize(buf);
    log(response.to_string());

    return true;
}

int main(int argc, char *argv[]) {
    struct addrinfo *res = get_server_addr_info();
    if (res == NULL) {
        fatal("failed to get server's addrinfo");
    }

    int server;
    if ((server = connect_to_server(res)) == -1) {
        fatal("failed to connect to server");
    }
    
    freeaddrinfo(res);

    log("connected to server");

    std::vector<std::string> command;
    for (int i = 1; i < argc; i++) {
        command.push_back(argv[i]);
    }

    uint32_t len = 0;
    for (const std::string &s : command) {
        len += 4 + s.length();  // 4 byte length header
    }
    if (len > Request::MAX_REQ_LEN) {
        fatal("command is too long");
    }

    log("read command");

    Request request(command);
    if (!send_request(server, request)) {
        fatal("failed to send request");
    }

    log("sent request");

    if (!handle_response(server)) {
        fatal("failed to handle response");
    }

    return 0;
}
