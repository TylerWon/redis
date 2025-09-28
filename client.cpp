#include <string>
#include <vector>
#include <stdexcept>
#include <sys/socket.h>
#include <cstring>
#include <netdb.h>
#include <unistd.h>

#include "utils/net_utils.hpp"
#include "utils/log.hpp"
#include "constants.hpp"
#include "requests/Request.hpp"
#include "responses/Response.hpp"
#include "utils/buf_utils.hpp"

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
    if (recv_all(server, buf, Response::HEADER_SIZE) == -1) {
        perror("failed to receive response header");
        return false;
    }

    uint32_t len;
    read_uint32(&len, (const char **) &buf);
    if (len > Response::MAX_LEN) {
        printf("response is too long\n");
        return false;
    }

    if (recv_all(server, buf, len) == -1) {
        perror("failed to receive response body");
        return false;
    }

    *n = Response::HEADER_SIZE + len;

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
    char buf[Response::HEADER_SIZE + Response::MAX_LEN];
    uint32_t n;
    if (!recv_response(server, buf, &n)) {
        log("failed to receive response");
        return false;
    }

    auto [response, status] = Response::unmarshal(buf, n);
    if (status == Response::UnmarshalStatus::INCOMPLETE_RES) {
        log("received incomplete response");
        return false;
    } else if (status == Response::UnmarshalStatus::RES_TOO_BIG) {
        log("response is too big");
        return false;
    } else if (status == Response::UnmarshalStatus::INVALID_RES) {
        log("response is invalid");
        return false;
    }

    log((*response)->to_string().data());

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

    // // TESTING
    // // Test pipelined requests
    // std::vector<std::vector<std::string>> commands = {
    //     { "set", "key", "test" },
    //     { "get", "key" },
    //     { "del", "key" }
    // };

    // for (const std::vector<std::string> &command : commands) {
    //     Request request(command);
    //     if(!send_request(server, request)) {
    //         fatal("failed to send '%s'", request.to_string().data());
    //     }
    // }

    // for (uint32_t i = 0; i < commands.size(); ++i) {
    //     if (!handle_response(server)) {
    //         fatal("failed to handle response");
    //     }
    // }

    // // Test max request size
    // std::vector<std::string> command = { std::string(Request::MAX_REQ_LEN - 8, 'a')}; // subtract 8 for number of strings and string length headers that get added to a request

    // Request request(command);
    // if (!send_request(server, request)) {
    //     fatal("failed to send request");
    // }

    // // Test exceeds max request size
    // std::vector<std::string> command = { std::string(Request::MAX_REQ_LEN - 7, 'a')};

    // Request request(command);
    // if (send_request(server, request)) {
    //     fatal("shouldn't have sent request");
    // }

    return 0;
}
