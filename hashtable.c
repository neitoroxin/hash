#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

static size_t default_hash(const void *key, size_t key_size) {
    const unsigned char *p = key;
    size_t h = 5381;
    for (size_t i = 0; i < key_size; i++)
        h = ((h << 5) + h) + p[i];
    return h;
}

static int default_eq(const void *a, const void *b, size_t key_size) {
    return memcmp(a, b, key_size) == 0;
}

static void *copy_data(const void *src, size_t size) {
    void *p = malloc(size);
    if (!p) return NULL;
    memcpy(p, src, size);
    return p;
}

static void free_node(hashtable_node_t *n) {
    if (n) {
        free(n->key);
        free(n->value);
        free(n);
    }
}

static int ht_insert_impl(hashtable_t *ht, const void *key, const void *value) {
    if (!ht || !key) return -1;
    size_t h = default_hash(key, ht->key_size);
    size_t idx = h % ht->bucket_count;

    hashtable_node_t *cur = ht->buckets[idx];
    while (cur) {
        if (default_eq(cur->key, key, ht->key_size)) {
            memcpy(cur->value, value, ht->value_size);
            return 0;
        }
        cur = cur->next;
    }

    hashtable_node_t *n = malloc(sizeof(hashtable_node_t));
    if (!n) return -1;
    n->key = copy_data(key, ht->key_size);
    n->value = copy_data(value, ht->value_size);
    n->next = ht->buckets[idx];
    ht->buckets[idx] = n;
    ht->size++;
    return 0;
}

static void *ht_get_impl(hashtable_t *ht, const void *key) {
    size_t h = default_hash(key, ht->key_size);
    size_t idx = h % ht->bucket_count;
    hashtable_node_t *cur = ht->buckets[idx];
    while (cur) {
        if (default_eq(cur->key, key, ht->key_size))
            return cur->value;
        cur = cur->next;
    }
    return NULL;
}

static int ht_remove_impl(hashtable_t *ht, const void *key) {
    size_t h = default_hash(key, ht->key_size);
    size_t idx = h % ht->bucket_count;
    hashtable_node_t *cur = ht->buckets[idx];
    hashtable_node_t *prev = NULL;
    while (cur) {
        if (default_eq(cur->key, key, ht->key_size)) {
            if (prev) prev->next = cur->next;
            else ht->buckets[idx] = cur->next;
            free_node(cur);
            ht->size--;
            return 1;
        }
        prev = cur;
        cur = cur->next;
    }
    return 0;
}

static int ht_contains_impl(hashtable_t *ht, const void *key) {
    return ht_get_impl(ht, key) != NULL;
}

static void ht_destroy_impl(hashtable_t *ht) {
    for (size_t i = 0; i < ht->bucket_count; i++) {
        hashtable_node_t *cur = ht->buckets[i];
        while (cur) {
            hashtable_node_t *next = cur->next;
            free_node(cur);
            cur = next;
        }
    }
    free(ht->buckets);
    ht->buckets = NULL;
    ht->bucket_count = 0;
    ht->size = 0;
}

int hashtable_init(hashtable_t *ht, size_t key_size, size_t value_size) {
    ht->bucket_count = 16;
    ht->size = 0;
    ht->max_load_factor = 0.75f;
    ht->key_size = key_size;
    ht->value_size = value_size;
    ht->buckets = calloc(ht->bucket_count, sizeof(hashtable_node_t *));
    if (!ht->buckets) return -1;

    ht->insert = ht_insert_impl;
    ht->get = ht_get_impl;
    ht->remove = ht_remove_impl;
    ht->contains = ht_contains_impl;
    ht->destroy = ht_destroy_impl;

    return 0;
}
