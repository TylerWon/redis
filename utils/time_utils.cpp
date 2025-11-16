#include "time_utils.hpp"
#include "log.hpp"

time_t get_time_ms() {
    timespec res;
    if (clock_gettime(CLOCK_MONOTONIC, &res) == -1) {
        fatal("failed to get time");
    }
    return res.tv_sec * 1000 + res.tv_nsec / 1000 / 1000;
}
