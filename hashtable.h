#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stddef.h>

typedef struct hashtable_node {
    void *key;
    void *value;
    struct hashtable_node *next;
} hashtable_node_t;

typedef struct hashtable {
    size_t bucket_count;
    size_t size;
    float max_load_factor;
    hashtable_node_t **buckets;

    size_t key_size;
    size_t value_size;

    int (*insert)(struct hashtable *self, const void *key, const void *value);
    void *(*get)(struct hashtable *self, const void *key);
    int (*remove)(struct hashtable *self, const void *key);
    int (*contains)(struct hashtable *self, const void *key);
    void (*destroy)(struct hashtable *self);
} hashtable_t;

int hashtable_init(hashtable_t *ht, size_t key_size, size_t value_size);

#endif
