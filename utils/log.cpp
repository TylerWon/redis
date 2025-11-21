#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <ostream>
#include <cstring>

/* Helper that takes a va_list */
void vlog(const char* fmt, va_list args) {
    time_t timestamp = time(NULL);
    char *str = ctime(&timestamp);
    size_t len = strlen(str);
    str[len - 1] = '\0'; // remove newline from end of timestamp
    printf("[%s] ", str);

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
