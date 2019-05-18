#ifndef _COUNTER_H
#define _COUNTER_H

#include "spin.h"

typedef struct counter_struct {
    int value;
    spinlock_t lock;
} counter_t;

void Counter_Init(counter_t *c, int value);
int Counter_GetValue(counter_t *c);
void Counter_Increment(counter_t *c);
void Counter_Decrement(counter_t *c);

#endif
