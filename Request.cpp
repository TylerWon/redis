#include <stdexcept>
#include <cstring>
#include <netinet/in.h>

#include "Request.hpp"
#include "buf_utils.hpp"

Request::Request(const std::vector<std::string> &command) {
    Request::command = command;
}

void Request::serialize(char *buf, uint32_t *n) {
    *n = 4 + 4; // Request length header + number of strings header
    for (const std::string &s : Request::command) {
        *n += 4 + s.length(); // String length header + string 
    }

    write_uint32(&buf, *n-4); // Subtract request length header

    write_uint32(&buf, Request::command.size());

    for (const std::string &s : Request::command) {
        uint32_t s_len = s.length();

        write_uint32(&buf, s_len);
        write_str(&buf, s);
    }
}

Request Request::deserialize(const char *buf) {
    buf += 4;   // Skip over request length

    uint32_t num_str;
    read_uint32(&num_str, &buf);

    std::vector<std::string> command;
    while (num_str > command.size()) {
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
