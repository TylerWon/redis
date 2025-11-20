#include "IdleTimer.hpp"
#include "../utils/time_utils.hpp"

const time_t TIMEOUT_MS = 60 * 1000;

IdleTimer::IdleTimer() {
    reset();
}

void IdleTimer::reset() {
    expiry_time_ms = get_time_ms() + TIMEOUT_MS;
}
