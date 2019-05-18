#include "list.h"
#include <stdlib.h>
#include <stdio.h>

static struct list_elem* List_Elem_Init(void *element, unsigned int key) {
    struct list_elem* list_elem = malloc(sizeof(struct list_elem));
    list_elem->key = key;
    list_elem->element = element;
    list_elem->next = NULL;
    return list_elem;
}

void List_Init(list_t *list) {
    list->length = 0;
    list->first = NULL;
    list->lock = 0;
}

void List_Insert(list_t *list, void *element, unsigned int key) {
    struct list_elem* list_elem = List_Elem_Init(element, key);
    spinlock_acquire(&list->lock);
    list_elem->next = list->first;
    list->first = list_elem;
    list->length++;
    spinlock_release(&list->lock);
}

void List_Delete(list_t *list, unsigned int key) {
    spinlock_acquire(&list->lock);

    struct list_elem* to_delete;

    if (!list->first) {
        spinlock_release(&list->lock);
        return;
    }
    else if ((list->first)->key == key){
        to_delete = list->first;
        list->first = to_delete->next;
    } else {
        struct list_elem* last = list->first;

        while (last->next && (last->next)->key != key) last = last->next;

        if (!last->next) {
            spinlock_release(&list->lock);
            return;
        }
        to_delete = last->next;
        last->next = to_delete->next;
    }

    (list->length)--;
    spinlock_release(&list->lock);
    free(to_delete);
}

void *List_Lookup(list_t *list, unsigned int key) {
    spinlock_acquire(&list->lock);

    if (list->first == NULL) {
        spinlock_release(&list->lock);
        return NULL;
    }

    struct list_elem* cur_elem = list->first;

    while (cur_elem != NULL && cur_elem->key != key) {
        //printf("%p\n", cur_elem);
        cur_elem = cur_elem->next;
    }

    if (cur_elem == NULL) {
        spinlock_release(&list->lock);
        return NULL;
    }

    spinlock_release(&list->lock);
    return cur_elem->element;
}
