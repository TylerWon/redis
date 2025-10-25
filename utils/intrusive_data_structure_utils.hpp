/* Returns a pointer to the container of an an intrusive data structure */
#define container_of(ptr, T, member) \
    ((T *)((char *)ptr - offsetof(T, member)))
