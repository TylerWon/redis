#include <ctime>

#include "../queue/Queue.hpp"

/**
 * A timer to track when a connection has been idle for too long. 
 * 
 * Once the expiry time is exceeded, the connection associated with the timer has been idle for too long and should be 
 * removed.
 */ 
class IdleTimer {
    public:
        time_t expiry_time_ms;
        QNode node;

        IdleTimer();

        /* Resets the expiry time of the timer */
        void reset();
};
