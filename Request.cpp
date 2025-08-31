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

    *buf_len = Request::REQ_LEN_HEADER_SIZE + req_len;
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

Request::DeserializationStatus Request::deserialize(const char *buf, uint32_t buf_len, Request **request) {
    if (buf_len < Request::REQ_LEN_HEADER_SIZE) {
        return Request::DeserializationStatus::INCOMPLETE_REQ;
    }

    uint32_t req_len;
    read_uint32(&req_len, &buf);

    if (req_len > Request::MAX_REQ_LEN) {
        return Request::DeserializationStatus::REQ_TOO_LARGE;
    } else if (buf_len < Request::REQ_LEN_HEADER_SIZE + req_len) {
        return Request::DeserializationStatus::INCOMPLETE_REQ;
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

    *request = new Request(command);

    return Request::DeserializationStatus::SUCCESS;
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
