#include <ctime>

#include "../queue/Queue.hpp"

/* A timer with a fixed timeout */
class Timer {
    public:
        time_t expiry_time_ms;
        QNode node;

        Timer();

        /* Resets the expiry time of the Timer. */
        void reset();
};
