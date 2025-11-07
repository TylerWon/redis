#include <assert.h>
#include <cstring>

#include "../SortedSet.hpp"

void test_insert_pair() {
    SortedSet set;
    set.insert(10, "tyler", 5);

    assert(set.length() == 1);

    SPair *pair = set.lookup("tyler", 5);
    assert(pair->len == 5);
    assert(strcmp(pair->name, "tyler") == 0);
    assert(pair->score == 10);
}

void test_insert_existing_pair() {
    SortedSet set;
    set.insert(10, "tyler", 5);
    set.insert(20, "tyler", 5);

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
    set.insert(0, "won", 3);

    SPair *pair = set.lookup("tyler", 5);
    assert(pair == NULL);
}

void test_lookup_pair() {
    SortedSet set;
    set.insert(10, "tyler", 5);
    set.insert(0, "won", 3);

    SPair *pair = set.lookup("tyler", 5);
    assert(pair->len == 5);
    assert(strcmp(pair->name, "tyler") == 0);
    assert(pair->score == 10);
}

void test_find_all_ge_on_empty_set() {
    SortedSet set;
    std::vector<SPair *> results = set.find_all_ge(10, "tyler'", 5);
    assert(results.size() == 0);
}

void test_find_all_ge_no_pairs_with_higher_score() {
    SortedSet set;
    set.insert(10, "tyler", 5);
    set.insert(0, "won", 3);

    std::vector<SPair *> results = set.find_all_ge(11, "jeff'", 4);
    assert(results.size() == 0);
}

void test_find_all_ge_no_pairs_with_higher_name() {
    SortedSet set;
    set.insert(10, "tyler", 5);
    set.insert(10, "won", 3);

    std::vector<SPair *> results = set.find_all_ge(10, "zed'", 3);
    assert(results.size() == 0);
}

void test_find_all_ge_pairs_with_higher_score_found() {
    SortedSet set;
    set.insert(11, "jeff", 4);
    set.insert(10, "tyler", 5);
    set.insert(0, "won", 3);

    std::vector<SPair *> results = set.find_all_ge(5, "adam", 4);
    assert(results.size() == 2);
    assert(results[0]->score == 10);
    assert(strcmp(results[0]->name, "tyler") == 0);
    assert(results[1]->score == 11);
    assert(strcmp(results[1]->name, "jeff") == 0);
}

void test_find_all_ge_pairs_with_higher_name_found() {
    SortedSet set;
    set.insert(10, "zed", 3);
    set.insert(10, "tyler", 5);
    set.insert(10, "adam", 4);

    std::vector<SPair *> results = set.find_all_ge(10, "mark", 4);
    assert(results.size() == 2);
    assert(results[0]->score == 10);
    assert(strcmp(results[0]->name, "tyler") == 0);
    assert(results[1]->score == 10);
    assert(strcmp(results[1]->name, "zed") == 0);
}

void test_find_all_ge_given_pair_in_set() {
    SortedSet set;
    set.insert(11, "jeff", 4);
    set.insert(10, "tyler", 5);
    set.insert(0, "won", 3);

    std::vector<SPair *> results = set.find_all_ge(10, "tyler", 5);
    assert(results.size() == 2);
    assert(results[0]->score == 10);
    assert(strcmp(results[0]->name, "tyler") == 0);
    assert(results[1]->score == 11);
    assert(strcmp(results[1]->name, "jeff") == 0);
}

void test_find_all_ge_with_offset() {
    SortedSet set;
    set.insert(11, "jeff", 4);
    set.insert(10, "tyler", 5);
    set.insert(0, "won", 3);

    std::vector<SPair *> results = set.find_all_ge(5, "adam", 4, 1);
    assert(results.size() == 1);
    assert(results[0]->score == 11);
    assert(strcmp(results[0]->name, "jeff") == 0);
}

void test_find_all_ge_offset_skips_all_results() {
    SortedSet set;
    set.insert(11, "jeff", 4);
    set.insert(10, "tyler", 5);
    set.insert(0, "won", 3);

    std::vector<SPair *> results = set.find_all_ge(10, "tyler", 5, 2);
    assert(results.size() == 0);
}

void test_remove_non_existent_pair() {
    SortedSet set;
    set.insert(10, "tyler", 5);

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
    set.insert(10, "tyler", 5);

    SPair *pair = spair_new("tyler", 5, 10);
    set.remove(pair);

    pair = set.lookup("tyler", 5);
    assert(set.length() == 0);
    assert(pair == NULL);
}

void test_rank_on_empty_set() {
    SortedSet set;
    int64_t rank = set.rank("tyler", 5);
    assert(rank == -1);
}

void test_rank_non_existent_pair() {
    SortedSet set;
    set.insert(10, "tyler", 5);
    set.insert(0, "won", 3);

    int64_t rank = set.rank("jim", 3);
    assert(rank == -1);
}

void test_rank_lowest_pair() {
    SortedSet set;
    set.insert(10, "tyler", 5);
    set.insert(0, "won", 3);
    set.insert(-4, "lucy", 4);

    int64_t rank = set.rank("lucy", 4);
    assert(rank == 1);
}

void test_rank_middle_pair() {
    SortedSet set;
    set.insert(10, "tyler", 5);
    set.insert(0, "won", 3);
    set.insert(-4, "lucy", 4);

    int64_t rank = set.rank("won", 3);
    assert(rank == 2);
}

void test_rank_highest_pair() {
    SortedSet set;
    set.insert(10, "tyler", 5);
    set.insert(0, "won", 3);
    set.insert(-4, "lucy", 4);

    int64_t rank = set.rank("tyler", 5);
    assert(rank == 3);
}

int main() {
    test_insert_pair();
    test_insert_existing_pair();

    test_lookup_on_empty_set();
    test_lookup_non_existent_pair();
    test_lookup_pair();

    test_find_all_ge_on_empty_set();
    test_find_all_ge_no_pairs_with_higher_score();
    test_find_all_ge_no_pairs_with_higher_name();
    test_find_all_ge_pairs_with_higher_score_found();
    test_find_all_ge_pairs_with_higher_name_found();
    test_find_all_ge_given_pair_in_set();
    test_find_all_ge_with_offset();
    test_find_all_ge_offset_skips_all_results();

    test_remove_non_existent_pair();
    test_remove_pair();

    test_rank_on_empty_set();
    test_rank_non_existent_pair();
    test_rank_lowest_pair();
    test_rank_middle_pair();
    test_rank_highest_pair();

    return 0;
}
