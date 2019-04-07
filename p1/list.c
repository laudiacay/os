#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "lib/list.h"

struct list_elem* init_list_elem(void* data) {
    struct list_elem* list_elem = malloc(sizeof(struct list_elem));
    list_elem->data = data;
    list_elem->next = NULL;
    return list_elem;
}

struct list* init_list(void) {
    struct list* list = malloc(sizeof(struct list));
    list->length = 0;
    list->first = NULL;
    return list;
}

// THIS DOES NOT FREE THE DATA IN THE LIST!!!!!!!!!
void free_list(struct list* list) {
    int len = list->length;
    for (int i = 0; i < len; i++) {
        delete_element(list, 0);
    }
    free(list);
}

// returns payload of selected list_elem
void* get_element(struct list* list, int index) {
    assert(index < list->length);
    assert(index >= 0);

    struct list_elem* cur_elem = list->first;
    int cur_ind = 0;

    while (cur_ind < index) {
        cur_elem = cur_elem->next;
        cur_ind++;
    }
    return cur_elem->data;
}

// returns payload of deleted list_elem
void* delete_element(struct list* list, int index) {
    assert(index < list->length);
    assert(index >= 0);

    struct list_elem* to_delete;

    if (index == 0) {
        to_delete = list->first;
        list->first = to_delete->next;
    } else {
        struct list_elem* last = list->first;
        int cur_ind = 1;

        while (cur_ind < index) {
            last = last->next;
            cur_ind++;
        }

        to_delete = last->next;
        last->next = to_delete->next;
    }

    void* ret = to_delete->data;
    free(to_delete);
    (list->length)--;
    return ret;
}

// adds data to list at index ind
// aka, before the old thing at index ind
void add_elem(struct list* list, int index, void* data) {
    assert(index <= list->length);
    assert(index >= 0);

    struct list_elem* list_elem = init_list_elem(data);

    if (index == 0) {
        list_elem->next = list->first;
        list->first = list_elem;
        (list->length)++;
        return;
    }

    struct list_elem* last_elem = list->first;
    int cur_ind = 1;

    while (cur_ind < index) {
        last_elem = last_elem->next;
        cur_ind++;
    }

    list_elem->next = last_elem->next;
    last_elem->next = list_elem;
    (list->length)++;
}
