#include <cstdarg>
#include <cstdio>
#include <cstdlib>

/* Helper that takes a va_list */
void vlog(const char* fmt, va_list args) {
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
}

void log(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vlog(fmt, args);
    va_end(args);
}

void fatal(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vlog(fmt, args);
    va_end(args);
    exit(EXIT_FAILURE);
}
