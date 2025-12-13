#define TEST_MODE

#include <assert.h>

#include "../IdleTimer.hpp"
#include "../TimerManager.hpp"
#include "../TTLTimer.hpp"
#include "../../conn/Conn.hpp"
#include "../../entry/Entry.hpp"
#include "../../utils/time_utils.hpp"
#include "../../utils/hash_utils.hpp"

void test_add_idle_timer() {
    TimerManager timers;
    IdleTimer timer;
    Queue *idle_timers = timers.get_idle_timers();

    assert(idle_timers->is_empty() == true);

    timers.add(&timer);

    assert(idle_timers->is_empty() == false);
    assert(idle_timers->front() == &timer.node);
    idle_timers->remove(&timer.node);
    assert(idle_timers->is_empty() == true);}

void test_update_idle_timer() {
    TimerManager timers;
    IdleTimer timer1;
    IdleTimer timer2;
    Queue *idle_timers = timers.get_idle_timers();

    timers.add(&timer1);
    timers.add(&timer2);

    assert(idle_timers->is_empty() == false);
    assert(idle_timers->front() == &timer1.node);

    timers.update(&timer1);

    assert(idle_timers->is_empty() == false);
    assert(idle_timers->front() == &timer2.node);
    idle_timers->remove(&timer2.node);
    assert(idle_timers->front() == &timer1.node);
    idle_timers->remove(&timer1.node);
    assert(idle_timers->is_empty() == true);}

void test_remove_idle_timer() {
    TimerManager timers;
    IdleTimer timer1;
    IdleTimer timer2;
    Queue *idle_timers = timers.get_idle_timers();

    timers.add(&timer1);
    timers.add(&timer2);

    assert(idle_timers->is_empty() == false);
    assert(idle_timers->front() == &timer1.node);

    timers.remove(&timer1);

    assert(idle_timers->is_empty() == false);
    assert(idle_timers->front() == &timer2.node);
    idle_timers->remove(&timer2.node);
    assert(idle_timers->is_empty() == true);
}

void test_add_ttl_timer() {
    TimerManager timers;
    TTLTimer timer;
    MinHeap *ttl_timers = timers.get_ttl_timers();

    assert(ttl_timers->is_empty() == true);

    timers.add(&timer);

    assert(ttl_timers->is_empty() == false);
    assert(ttl_timers->min() == &timer.node);
    ttl_timers->remove(&timer.node, is_ttl_timer_less);
    assert(ttl_timers->is_empty() == true);}

void test_update_ttl_timer() {
    TimerManager timers;
    TTLTimer timer1;
    TTLTimer timer2;
    MinHeap *ttl_timers = timers.get_ttl_timers();
    timer1.expiry_time_ms = 1;
    timer2.expiry_time_ms = 2;

    timers.add(&timer1);
    timers.add(&timer2);

    assert(ttl_timers->is_empty() == false);
    assert(ttl_timers->min() == &timer1.node);

    timer1.expiry_time_ms = 3;
    timers.update(&timer1);

    assert(ttl_timers->is_empty() == false);
    assert(ttl_timers->min() == &timer2.node);
    ttl_timers->remove(&timer2.node, is_ttl_timer_less);
    assert(ttl_timers->min() == &timer1.node);
    ttl_timers->remove(&timer2.node, is_ttl_timer_less);
    assert(ttl_timers->is_empty() == true);
}

void test_remove_ttl_timer() {
    TimerManager timers;
    TTLTimer timer1;
    TTLTimer timer2;
    MinHeap *ttl_timers = timers.get_ttl_timers();
    timer1.expiry_time_ms = 1;
    timer2.expiry_time_ms = 2;

    timers.add(&timer1);
    timers.add(&timer2);

    assert(ttl_timers->is_empty() == false);
    assert(ttl_timers->min() == &timer1.node);

    timers.remove(&timer1);

    assert(ttl_timers->is_empty() == false);
    assert(ttl_timers->min() == &timer2.node);
    ttl_timers->remove(&timer2.node, is_ttl_timer_less);
    assert(ttl_timers->is_empty() == true);
}

void test_get_time_until_expiry_no_timers() {
    TimerManager timers;
    int32_t result = timers.get_time_until_expiry_fn(get_time_ms);
    assert(result == -1);
}

void test_get_time_until_expiry_expired_idle_timer() {
    TimerManager timers;
    IdleTimer idle_timer1;
    IdleTimer idle_timer2;
    TTLTimer ttl_timer1;
    TTLTimer ttl_timer2;
    idle_timer1.expiry_time_ms = 1;
    idle_timer2.expiry_time_ms = 3;
    ttl_timer1.expiry_time_ms = 4;
    ttl_timer2.expiry_time_ms = 5;

    timers.add(&idle_timer1);
    timers.add(&idle_timer2);
    timers.add(&ttl_timer1);
    timers.add(&ttl_timer2);

    int32_t result = timers.get_time_until_expiry_fn([]() -> time_t { return 2; });
    
    assert(result == 0);
}

void test_get_time_until_expiry_expired_ttl_timer() {
    TimerManager timers;
    IdleTimer idle_timer1;
    IdleTimer idle_timer2;
    TTLTimer ttl_timer1;
    TTLTimer ttl_timer2;
    idle_timer1.expiry_time_ms = 4;
    idle_timer2.expiry_time_ms = 5;
    ttl_timer1.expiry_time_ms = 1;
    ttl_timer2.expiry_time_ms = 3;
    
    timers.add(&idle_timer1);
    timers.add(&idle_timer2);
    timers.add(&ttl_timer1);
    timers.add(&ttl_timer2);

    int32_t result = timers.get_time_until_expiry_fn([]() -> time_t { return 2; });
    
    assert(result == 0);
}

void test_get_time_until_expiry_idle_timer_next_to_expire() {
    TimerManager timers;
    IdleTimer idle_timer1;
    IdleTimer idle_timer2;
    TTLTimer ttl_timer1;
    TTLTimer ttl_timer2;
    idle_timer1.expiry_time_ms = 1;
    idle_timer2.expiry_time_ms = 2;
    ttl_timer1.expiry_time_ms = 3;
    ttl_timer2.expiry_time_ms = 4;
    
    timers.add(&idle_timer1);
    timers.add(&idle_timer2);
    timers.add(&ttl_timer1);
    timers.add(&ttl_timer2);

    int32_t result = timers.get_time_until_expiry_fn([]() -> time_t { return 0; });
    
    assert(result == 1);
}

void test_get_time_until_expiry_ttl_timer_next_to_expire() {
    TimerManager timers;
    IdleTimer idle_timer1;
    IdleTimer idle_timer2;
    TTLTimer ttl_timer1;
    TTLTimer ttl_timer2;
    idle_timer1.expiry_time_ms = 3;
    idle_timer2.expiry_time_ms = 4;
    ttl_timer1.expiry_time_ms = 1;
    ttl_timer2.expiry_time_ms = 2;
    
    timers.add(&idle_timer1);
    timers.add(&idle_timer2);
    timers.add(&ttl_timer1);
    timers.add(&ttl_timer2);

    int32_t result = timers.get_time_until_expiry_fn([]() -> time_t { return 0; });
    
    assert(result == 1);
}

void test_process_timers_no_expired_timers() {
    TimerManager timers;
    HMap kv_store;
    ThreadPool thread_pool(4);
    Queue *idle_timers = timers.get_idle_timers();
    MinHeap *ttl_timers = timers.get_ttl_timers();
    
    // create conns (idle timers)
    Conn *conn1 = new Conn(10, false, false, false);
    Conn *conn2 = new Conn(11, false, false, false);
    std::vector<Conn *> fd_to_conn(conn2->fd + 1);
    conn1->idle_timer.expiry_time_ms = 1;
    conn2->idle_timer.expiry_time_ms = 2;
    fd_to_conn[conn1->fd] = conn1;
    fd_to_conn[conn2->fd] = conn2;
    timers.add(&conn1->idle_timer);
    timers.add(&conn2->idle_timer);

    // create entries (ttl timers)
    Entry *entry1 = new Entry("entry1", EntryType::STR, "1");
    Entry *entry2 = new Entry("entry2", EntryType::STR, "2");
    entry1->ttl_timer.expiry_time_ms = 3;
    entry2->ttl_timer.expiry_time_ms = 4;
    kv_store.insert(&entry1->node);
    kv_store.insert(&entry2->node);
    timers.add(&entry1->ttl_timer);
    timers.add(&entry2->ttl_timer);

    timers.process_timers_fn(kv_store, fd_to_conn, thread_pool, []() -> time_t { return 0; });

    // check timers still managed by timer manager
    assert(idle_timers->is_empty() == false);
    assert(idle_timers->front() == &conn1->idle_timer.node);
    idle_timers->remove(&conn1->idle_timer.node);
    assert(idle_timers->front() == &conn2->idle_timer.node);
    idle_timers->remove(&conn2->idle_timer.node);
    assert(idle_timers->is_empty() == true);

    assert(ttl_timers->is_empty() == false);
    assert(ttl_timers->min() == &entry1->ttl_timer.node);
    ttl_timers->remove(&entry1->ttl_timer.node, is_ttl_timer_less);
    assert(ttl_timers->min() == &entry2->ttl_timer.node);
    ttl_timers->remove(&entry2->ttl_timer.node, is_ttl_timer_less);
    assert(ttl_timers->is_empty() == true);

    // check entries still in kv store
    assert(kv_store.length() == 2);
    assert(kv_store.lookup(&entry1->node, are_entries_equal) != NULL);
    assert(kv_store.lookup(&entry2->node, are_entries_equal) != NULL);

    // check conns still in fd_to_conn
    assert(fd_to_conn[conn1->fd] != NULL);
    assert(fd_to_conn[conn2->fd] != NULL);
}

void test_process_timers_expired_timers() {
    TimerManager timers;
    HMap kv_store;
    ThreadPool thread_pool(4);
    Queue *idle_timers = timers.get_idle_timers();
    MinHeap *ttl_timers = timers.get_ttl_timers();
    
    // create conns (idle timers)
    Conn *conn1 = new Conn(10, false, false, false);
    Conn *conn2 = new Conn(11, false, false, false);
    std::vector<Conn *> fd_to_conn(conn2->fd + 1);
    conn1->idle_timer.expiry_time_ms = 1;
    conn2->idle_timer.expiry_time_ms = 2;
    fd_to_conn[conn1->fd] = conn1;
    fd_to_conn[conn2->fd] = conn2;
    timers.add(&conn1->idle_timer);
    timers.add(&conn2->idle_timer);

    // create entries (ttl timers)
    Entry *entry1 = new Entry("entry1", EntryType::STR, "1");
    Entry *entry2 = new Entry("entry2", EntryType::STR, "2");
    entry1->ttl_timer.expiry_time_ms = 3;
    entry2->ttl_timer.expiry_time_ms = 4;
    kv_store.insert(&entry1->node);
    kv_store.insert(&entry2->node);
    timers.add(&entry1->ttl_timer);
    timers.add(&entry2->ttl_timer);

    timers.process_timers_fn(kv_store, fd_to_conn, thread_pool, []() -> time_t { return 5; });

    // check timers removed from timer manager
    assert(idle_timers->is_empty() == true);
    assert(ttl_timers->is_empty() == true);

    // check entries removed from kv store
    assert(kv_store.length() == 0);
    LookupEntry lookup1("entry1");
    LookupEntry lookup2("entry1");
    assert(kv_store.lookup(&lookup1.node, are_entries_equal) == NULL);
    assert(kv_store.lookup(&lookup2.node, are_entries_equal) == NULL);

    // check conns removed from_to_conn
    assert(fd_to_conn[10] == NULL);
    assert(fd_to_conn[11] == NULL);
}

void test_process_timers_hit_limit_of_expired_ttl_timers() {
    TimerManager timers;
    HMap kv_store;
    ThreadPool thread_pool(4);
    std::vector<Conn *> fd_to_conn;
    MinHeap *ttl_timers = timers.get_ttl_timers();

    // create MAX_TTL_EXPIRATIONS + 1 entries (ttl timers)
    Entry *last_entry;
    for (uint32_t i = 1; i <= TimerManager::MAX_TTL_EXPIRATIONS + 1; i++) {
        Entry *entry = new Entry(std::to_string(i), EntryType::STR, std::to_string(i));
        entry->ttl_timer.expiry_time_ms = i;
        kv_store.insert(&entry->node);
        timers.add(&entry->ttl_timer);

        if (i == TimerManager::MAX_TTL_EXPIRATIONS + 1) {
            last_entry = entry;
        }
    }

    timers.process_timers_fn(kv_store, fd_to_conn, thread_pool, []() -> time_t { return TimerManager::MAX_TTL_EXPIRATIONS + 2; });

    // check only 1 timer managed by timer manager
    assert(ttl_timers->is_empty() == false);
    assert(ttl_timers->min() == &last_entry->ttl_timer.node);
    ttl_timers->remove(&last_entry->ttl_timer.node, is_ttl_timer_less);
    assert(ttl_timers->is_empty() == true);

    // check only 1 entry in kv store
    assert(kv_store.length() == 1);
    assert(kv_store.lookup(&last_entry->node, are_entries_equal) != NULL);
}

int main() {
    test_add_idle_timer();
    test_update_idle_timer();
    test_remove_idle_timer();

    test_add_ttl_timer();
    test_update_ttl_timer();
    test_remove_ttl_timer();

    test_get_time_until_expiry_no_timers();
    test_get_time_until_expiry_expired_idle_timer();
    test_get_time_until_expiry_expired_ttl_timer();
    test_get_time_until_expiry_idle_timer_next_to_expire();
    test_get_time_until_expiry_ttl_timer_next_to_expire();

    test_process_timers_no_expired_timers();
    test_process_timers_expired_timers();
    test_process_timers_hit_limit_of_expired_ttl_timers();

    return 0;
}