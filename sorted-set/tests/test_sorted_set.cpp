#include <assert.h>
#include <cstring>

#include "../SortedSet.hpp"

void test_insert_pair() {
    SortedSet set;
    set.insert("tyler", 5, 10);

    assert(set.length() == 1);

    SPair *pair = set.lookup("tyler", 5);
    assert(pair->len == 5);
    assert(strcmp(pair->name, "tyler") == 0);
    assert(pair->score == 10);
}

void test_insert_existing_pair() {
    SortedSet set;
    set.insert("tyler", 5, 10);
    set.insert("tyler", 5, 20);

    assert(set.length() == 1);

    SPair *pair = set.lookup("tyler", 5);
    assert(pair->len == 5);
    assert(strcmp(pair->name, "tyler") == 0);
    assert(pair->score == 20);
}

void test_lookup_on_empty_set() {
    SortedSet set;

    SPair *pair = set.lookup("tyler", 5);
    assert(pair == NULL);
}

void test_lookup_non_existent_pair() {
    SortedSet set;
    set.insert("won", 3, 0);

    SPair *pair = set.lookup("tyler", 5);
    assert(pair == NULL);
}

void test_lookup_pair() {
    SortedSet set;
    set.insert("tyler", 5, 10);
    set.insert("won", 3, 0);

    SPair *pair = set.lookup("tyler", 5);
    assert(pair->len == 5);
    assert(strcmp(pair->name, "tyler") == 0);
    assert(pair->score == 10);
}

void test_remove_non_existent_pair() {
    SortedSet set;
    set.insert("tyler", 5, 10);

    SPair *pair = spair_new("won", 3, 0);
    set.remove(pair);

    assert(set.length() == 1);

    pair = set.lookup("tyler", 5);
    assert(pair->len == 5);
    assert(strcmp(pair->name, "tyler") == 0);
    assert(pair->score == 10);
}

void test_remove_pair() {
    SortedSet set;
    set.insert("tyler", 5, 10);

    SPair *pair = spair_new("tyler", 5, 10);
    set.remove(pair);

    pair = set.lookup("tyler", 5);
    assert(set.length() == 0);
    assert(pair == NULL);
}

int main() {
    test_insert_pair();
    test_insert_existing_pair();

    test_lookup_on_empty_set();
    test_lookup_non_existent_pair();
    test_lookup_pair();

    test_remove_non_existent_pair();
    test_remove_pair();

    return 0;
}
