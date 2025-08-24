#include <stdio.h>
#include <stdlib.h>

void log(const char *msg) {
    fprintf(stderr, "%s\n", msg);
}

void fatal(const char *msg) {
    log(msg);
    exit(EXIT_FAILURE);
}
