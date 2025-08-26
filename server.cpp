#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include <format>

#include "net_utils.hpp"
#include "log.hpp"
#include "constants.hpp"
#include "Request.hpp"
#include "buf_utils.hpp"
#include "Response.hpp"

std::map<std::string, std::string> kvstore;

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

/**
 * Receives a request from a client.
 * 
 * @param client    The client socket.
 * @param buf       Pointer to a char buffer where the request will be stored.
 * 
 * @return  0 on success.
 *          -1 on error.
 */
int recv_request(int client, char *buf) {
    if (recv_all(client, buf, 4) == -1) {
        log("failed to receive request length");
        return -1;
    }

    uint32_t len;
    read_uint32(&len, (const char **) &buf);
    if (len > Request::MAX_REQ_LEN) {
        log("request is too long");
        return -1;
    }

    if (recv_all(client, buf, len) == -1) {
        log("failed to receive request body");
        return -1;
    }

    return 0;
}

/**
 * Executes the command in the Request.
 * 
 * There are three commands supported:
 * 1. get [key] - get the value of [key] in the kv store
 * 2. set [key] [value] - set the value of [key] to [value] in the kv store
 * 3. del [key] - delete [key] from the kv store
 * 
 * @param request   The request.
 * 
 * @return  The Response for executing the command.
 */
Response execute_command(Request request) {
    Response response(Response::ResponseStatus::RES_OK, "");
    std::vector<std::string> &command = request.command;

    if (command.size() == 2 && command[0] == "get") {
        try {
            std::string value = kvstore.at(command[1]);
            response.message = std::format("value for key '{}' is '{}'", command[1], value);
        } catch (std::out_of_range &e) {
            response.status = Response::ResponseStatus::RES_NX;           
            response.message = std::format("key '{}' does not exist", command[1]);
        }
    } else if (command.size() == 3 && command[0] == "set") {
        kvstore[command[1]].swap(command[2]);
        response.message = std::format("set key '{}' to value '{}'", command[1], kvstore[command[1]]);
    } else if (command.size() == 2 && command[0] == "del") {
        kvstore.erase(command[1]);
        response.message = std::format("removed key '{}'", command[1]);
    } else {
        response.status = Response::ResponseStatus::RES_ERR;
        response.message = std::format("'{}' is not a valid command", request.to_string());
    }

    log(response.message);

    return response;
}

/**
 * Sends a response to the client.
 * 
 * @param client    The client socket.
 * @param response  The Response.
 * 
 * @param   0 on success.
 *          -1 on errror.
 */
int send_response(int client, Response response) {
    char buf[4 + Response::MAX_RES_LEN];
    uint32_t n;
    response.serialize(buf, &n);

    if (send_all(client, buf, n) == -1) {
        return -1;
    }

    return 0;
}

/**
 * Handles a request from a client.
 * 
 * @param client    The client socket.
 * 
 * @return  0 on success.
 *          -1 on error.
 */
int handle_request(int client) {
    char buf[4 + Request::MAX_REQ_LEN];
    if (recv_request(client, buf) == -1) {
        log("failed to receive request");
        return -1;
    }

    Request request = Request::deserialize(buf);

    Response response = execute_command(request);
    if (send_response(client, response) == -1) {
        log("failed to send response");
        return -1;
    }

    return 0;
}

int main() {
    struct addrinfo *res = get_server_addr_info();
    if (res == NULL) {
        fatal("failed to get server's addrinfo");
    }

    int listener;
    if ((listener = start_server(res)) == -1) {
        fatal("failed to start server");
    }

    log("started server");

    int client;
    if ((client = accept(listener, NULL, NULL)) == -1) {
        fatal("failed to accept client connection");
    }

    log("accepted connection");

    if (handle_request(client) == -1) {
        fatal("failed to handle request");
    }

    log("handled request");
}