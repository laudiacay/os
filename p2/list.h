#ifndef _LIST_H
#define _LIST_H

#include "spin.h"

struct list_elem {
    unsigned int key;
    void* element;
    struct list_elem* next;
};

typedef struct list {
    int length;
    struct list_elem* first;
    spinlock_t lock;
} list_t;

void List_Init(list_t *list);
void List_Insert(list_t *list, void *element, unsigned int key);
void List_Delete(list_t *list, unsigned int key);
void *List_Lookup(list_t *list, unsigned int key);

#endif
