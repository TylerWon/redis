#include <ctime>

#include "../min-heap/components/MHNode.hpp"

/**
 * A timer to track the TTL (time-to-live) of an entry in the kv store. 
 * 
 * Once the expiry time is exceeded, the entry associated with the timer has expired and should be removed.
 */ 
struct TTLTimer {
    time_t expiry_time_ms = 0;
    MHNode node;
};
