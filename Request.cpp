#include <stdexcept>
#include <cstring>
#include <netinet/in.h>

#include "Request.hpp"
#include "buf_utils.hpp"

Request::Request(const std::vector<std::string> &command) {
    Request::command = command;
}

void Request::serialize(char *buf, uint32_t *n) {
    *n = Request::REQ_LEN_SIZE + Request::NUM_STRS_SIZE;
    for (const std::string &s : Request::command) {
        *n += Request::STR_LEN_SIZE + s.length();
    }

    write_uint32(&buf, *n-Request::REQ_LEN_SIZE);
    write_uint32(&buf, Request::command.size());
    for (const std::string &s : Request::command) {
        uint32_t s_len = s.length();

        write_uint32(&buf, s_len);
        write_str(&buf, s);
    }
}

Request Request::deserialize(const char *buf) {
    buf += Request::REQ_LEN_SIZE;

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

    return Request(command);
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
