#include "hash.h"
#include <stdlib.h>
#include <assert.h>

void Hash_Init(hash_t *hash, int buckets) {
    assert(buckets > 0);
    hash->buckets = buckets;
    hash->bucket_table = malloc(buckets * sizeof(list_t));
    for (int i = 0; i < buckets; i++) {
        List_Init(&hash->bucket_table[i]);
    }
}

void Hash_Insert(hash_t *hash, void *element, unsigned int key) {
    int bucket = key % hash->buckets;
    List_Insert(&hash->bucket_table[bucket], element, key);
}
void Hash_Delete(hash_t *hash, unsigned int key) {
    int bucket = key % hash->buckets;
    List_Delete(&hash->bucket_table[bucket], key);
}

void *Hash_Lookup(hash_t *hash, unsigned int key) {
    int bucket = key % hash->buckets;
    return List_Lookup(&hash->bucket_table[bucket], key);
}
