#include <stdio.h>
#include <stdlib.h>
#include <string>

void log(const std::string msg) {
    fprintf(stderr, "%s\n", msg.data());
}

void fatal(const std::string msg) {
    log(msg);
    exit(EXIT_FAILURE);
}
