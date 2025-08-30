#include <stdexcept>
#include <cstring>
#include <netinet/in.h>

#include "Request.hpp"
#include "buf_utils.hpp"
#include "log.hpp"

Request::Request(const std::vector<std::string> &command) {
    Request::command = command;
}

void Request::serialize(char **buf, uint32_t *buf_len) {
    uint32_t req_len = Request::length();
    if (req_len > Request::MAX_REQ_LEN) {
        *buf = NULL;
        return;
    }

    *buf_len = Request::REQ_LEN_SIZE + req_len;
    *buf = (char *) malloc(*buf_len);
    char *b = *buf; // Use b instead of buf when writing because pointer gets incremented

    write_uint32(&b, req_len);
    write_uint32(&b, Request::command.size());
    for (const std::string &s : Request::command) {
        uint32_t s_len = s.length();
        write_uint32(&b, s_len);
        write_str(&b, s);
    }
}

Request *Request::deserialize(const char *buf, uint32_t buf_len, int *res) {
    *res = 1;

    if (buf_len < Request::REQ_LEN_SIZE) {
        *res = 0;
        return NULL;
    }

    uint32_t req_len;
    read_uint32(&req_len, &buf);

    if (req_len > Request::MAX_REQ_LEN) {
        *res = -1;
        return NULL;
    } else if (buf_len < Request::REQ_LEN_SIZE + req_len) {
        *res = 0;
        return NULL;
    }

    uint32_t num_strs;
    read_uint32(&num_strs, &buf);

    std::vector<std::string> command;
    while (num_strs > command.size()) {
        uint32_t s_len;
        read_uint32(&s_len, &buf);

        std::string s;
        read_str(s, s_len, &buf);
        command.push_back(s);
    }

    return new Request(command);
}

std::string Request::to_string() {
    if (Request::command.size() < 1) {
        return "";
    }

    std::string result = Request::command[0];
    for (uint32_t i = 1; i < Request::command.size(); i++) {
        result += " ";
        result += command[i];
    }

    return result;
}

uint32_t Request::length() {
    uint32_t n = Request::NUM_STRS_SIZE;
    for (const std::string &s : Request::command) {
        n += Request::STR_LEN_SIZE + s.length();
    }   
    return n;
}
