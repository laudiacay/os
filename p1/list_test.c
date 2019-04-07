#include <stdio.h>
#include "lib/list.h"

int main() {
    int one = 1;
    int two = 2;
    int three = 3;
    int four = 4;
    int five = 5;

    struct list* list = init_list();

    add_elem(list, 0, &one);
    add_elem(list, 1, &two);
    add_elem(list, 2, &three);
    add_elem(list, 3, &four);
    add_elem(list, 4, &five);

    printf("1: %d\n", *(int*)(get_element(list, 0)));
    printf("2: %d\n", *(int*)(get_element(list, 1)));
    printf("3: %d\n", *(int*)(get_element(list, 2)));
    printf("4: %d\n", *(int*)(get_element(list, 3)));
    printf("5: %d\n", *(int*)(get_element(list, 4)));

    // todo: test deletion

    delete_element(list, 4);

    printf("1: %d\n", *(int*)(get_element(list, 0)));
    printf("2: %d\n", *(int*)(get_element(list, 1)));
    printf("3: %d\n", *(int*)(get_element(list, 2)));
    printf("4: %d\n", *(int*)(get_element(list, 3)));

    free_list(list);
}
