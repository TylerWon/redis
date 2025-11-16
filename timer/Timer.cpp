#include "Timer.hpp"
#include "../utils/time_utils.hpp"

const time_t TIMEOUT_MS = 5 * 1000;

Timer::Timer() {
    reset();
}

void Timer::reset() {
    expiry_time_ms = get_time_ms() + TIMEOUT_MS;
}
