#include <stdio.h>
#include <stdlib.h>

void log(const char *msg) {
    fprintf(stderr, "%s\n", msg);
}

void kill(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}
