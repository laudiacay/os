#ifndef LIST
#define LIST

struct list_elem {
    void* data;
    struct list_elem* next;
};

struct list {
    int length;
    struct list_elem* first;
};

struct list_elem* init_list_elem(void* data);
struct list* init_list(void);
void free_list(struct list* list);
void* get_element(struct list* list, int index);
void* delete_element(struct list* list, int index);
void add_elem(struct list* list, int index, void* data);

#endif
