#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include <format>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

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
 * Receives a request from a client.
 * 
 * @param client    The client socket.
 * @param buf       Pointer to a char buffer where the request will be stored.
 * @param n         Pointer to a uint32_t where the size of the request will be stored.
 * 
 * @return  True on success.
 *          False on failure.
 */
bool recv_request(int client, char *buf, uint32_t *n) {
    if (recv_all(client, buf, 4) == -1) {
        log("failed to receive request length");
        return false;
    }

    uint32_t len;
    read_uint32(&len, (const char **) &buf);
    if (len > Request::MAX_REQ_LEN) {
        log("request is too long");
        return false;
    }

    if (recv_all(client, buf, len) == -1) {
        log("failed to receive request body");
        return false;
    }

    *n = len + 4;

    return true;
}

/**
 * Executes the command in the Request.
 * 
 * There are three commands supported:
 * 1. get [key] - get the value of [key] in the kv store
 * 2. set [key] [value] - set the value of [key] to [value] in the kv store
 * 3. del [key] - delete [key] from the kv store
 * 
 * @param request   Pointer to the Request.
 * 
 * @return  The Response for executing the command.
 */
Response execute_command(Request *request) {
    Response response(Response::ResponseStatus::RES_OK, "");
    std::vector<std::string> &command = request->command;

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
        response.message = std::format("'{}' is not a valid command", request->to_string());
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
 * @return  True on success.
 *          False on failure.
 */
bool send_response(int client, Response response) {
    char *buf;
    uint32_t n;
    response.serialize(&buf, &n);
    if (buf == NULL) {
        log("failed to serialize response");
        return false;
    }

    if (send_all(client, buf, n) == -1) {
        free(buf);
        return false;
    }

    free(buf);

    return true;
}

/**
 * Handles a request from a client.
 * 
 * @param client    The client socket.
 * 
 * @return  True on success.
 *          False on failure.
 */
bool handle_request(int client) {
    char buf[4 + Request::MAX_REQ_LEN];
    uint32_t n;
    if (!recv_request(client, buf, &n)) {
        log("failed to receive request");
        return false;
    }

    int res;
    Request *request = Request::deserialize(buf, n, &res);
    if (res < 1) {
        log("failed to deserialize request");
        return false;
    }

    Response response = execute_command(request);
    if (!send_response(client, response)) {
        log("failed to send response");
        delete request;
        return false;
    }

    delete request;

    return true;
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

    if (!handle_request(client)) {
        fatal("failed to handle request");
    }

    log("handled request");
}