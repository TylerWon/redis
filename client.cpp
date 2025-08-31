#include <string>
#include <vector>
#include <stdexcept>
#include <sys/socket.h>
#include <cstring>
#include <netdb.h>
#include <unistd.h>

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
    char *buf;
    uint32_t buf_len;
    request.serialize(&buf, &buf_len);
    if (buf == NULL) {
        log("request exceeds size limit");
        return false;
    }

    if (send_all(server, buf, buf_len) == -1) {
        perror("failed to send message");
        free(buf);
        return false;
    }

    free(buf);

    return true;
}

/**
 * Receives a response from the server.
 * 
 * @param server    The server socket.
 * @param buf       Pointer to a char buffer where the response will be stored.
 * @param n         Pointer to a uint32_t where the size of the response will be stored.
 * 
 * @return  True on success.
 *          False on failure.
 */
bool recv_response(int server, char *buf, uint32_t *n) {
    if (recv_all(server, buf, Response::RES_LEN_HEADER_SIZE) == -1) {
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

    *n = len + 4;

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
    char buf[Response::RES_LEN_HEADER_SIZE + Response::MAX_RES_LEN];
    uint32_t n;
    if (!recv_response(server, buf, &n)) {
        log("failed to receive response");
        return false;
    }

    Response *response;
    Response::deserialize(buf, n, &response); // recv_response ensures valid response is received so don't need to 
                                              // check status of deserialize operation
    
    log(response->to_string().data());

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
