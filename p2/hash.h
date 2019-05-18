#ifndef _HASH_H
#define _HASH_H

#include "list.h"

typedef struct hash {
    list_t* bucket_table;
    int buckets;
} hash_t;

void Hash_Init(hash_t *hash, int buckets);
void Hash_Insert(hash_t *hash, void *element, unsigned int key);
void Hash_Delete(hash_t *hash, unsigned int key);
void *Hash_Lookup(hash_t *hash, unsigned int key);

#endif
