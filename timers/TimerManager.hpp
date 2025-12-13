#pragma once

#include <vector>

#include "../hashmap/HMap.hpp"
#include "../min-heap/MinHeap.hpp"
#include "../queue/Queue.hpp"
#include "../thread-pool/ThreadPool.hpp"

// Forward declarations to break circular dependency
class Conn;
class IdleTimer;
class TTLTimer;

/* Manages expirations of idle connection timers and TTL timers for kv store entries */
class TimerManager {
    private:
        Queue idle_timers; // can use a queue because idle timers have a fixed timeout value
        MinHeap ttl_timers;
    public:
        static const uint16_t MAX_TTL_EXPIRATIONS = 1000;
        
        /**
         * Gets the time until the next timer expires.
         * 
         * @return  The time until the next timer expires.
         *          0 if the next timer has already expired.
         *          -1 if there are no active timers.
         */
        int32_t get_time_until_expiry();

        /**
         * Checks the idle and TTL timers to see if any have expired.
         * 
         * If a timer has expired, the associated connection or entry is removed. The number of expired TTL timers 
         * processed is limited to MAX_TTL_EXPIRATIONS to prevent holding up the event loop.
         * 
         * @param kv_store      Reference to the kv store.
         * @param fd_to_conn    Reference to the map of all connections, indexed by fd.
         * @param thread_pool   Reference to the thread pool used for asynchronous work.
         */
        void process_timers(HMap &kv_store, std::vector<Conn *> &fd_to_conn, ThreadPool &thread_pool);

        /**
         * Adds an idle timer to be managed by the TimerManager. Should be called after the timer's expiry is first set. 
         * 
         * Since idle timers have a fixed timeout value, setting the timer's expiry will make it the last timer to 
         * expire. As a result, it is added to the end of the expiration order.
         */
        void add(IdleTimer *timer);

        /**
         * Updates the position of an idle timer in the expiration order. Should be called after the timer's expiry is 
         * updated. 
         * 
         * Since idle timers have a fixed timeout value, updating the timer's expiry will make it the last timer to 
         * expire. As a result, it is moved to the end of the expiration order.
         */
        void update(IdleTimer *timer);

        /** 
         * Removes an idle timer from being managed by the TimerManager. Should be called after the timer's expiry is 
         * cleared.
         */
        void remove(IdleTimer *timer);

        /* Adds a TTL timer to be managed by the TimerManager. Should be called after the timer's expiry is first set. */
        void add(TTLTimer *timer);

        /**
         * Updates the position of a TTL timer in the expiration order. Should be called after the timer's expiry is 
         * updated.
         */
        void update(TTLTimer *timer);

        /**
         * Removes a TTL timer from being managed by the TimerManager. Should be called after the timer's expiry is 
         * cleared.
         */
        void remove(TTLTimer *timer);

    #ifdef TEST_MODE
    public:  
        Queue *get_idle_timers() { return &idle_timers; };
        MinHeap *get_ttl_timers() { return &ttl_timers; }; 
    #else
    private:
    #endif
        /**
         * Logic for get_time_until_expiry(). 
         * 
         * Accepts a function for getting the current time. This allows the time to be mocked which improves 
         * testability.
         * 
         * @param get_time_ms  Function to use for getting the current time in ms.
         */
        int32_t get_time_until_expiry_fn(time_t (*get_time_ms)());

        /**
         * Logic for process_timers(). 
         * 
         * In addition to the parameters for process_timers(), accepts a function for getting the current time. This 
         * allows the time to be mocked which improves testability.
         * 
         * @param kv_store      Reference to the kv store.
         * @param fd_to_conn    Reference to the map of all connections, indexed by fd.
         * @param thread_pool   Reference to the thread pool used for asynchronous work.
         * @param get_time_ms   Function to use for getting the current time in ms.
         */
        void process_timers_fn(HMap &kv_store, std::vector<Conn *> &fd_to_conn, ThreadPool &thread_pool, time_t (*get_time_ms)());
};
