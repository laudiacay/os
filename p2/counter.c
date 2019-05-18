#include "counter.h"

void Counter_Init(counter_t *c, int value) {
    c->value = value;
    c->lock = 0;
}

int Counter_GetValue(counter_t *c) {
    spinlock_acquire(&c->lock);
    int ret = c->value;
    spinlock_release(&c->lock);
    return ret;
}

void Counter_Increment(counter_t *c) {
    spinlock_acquire(&c->lock);
    c->value += 1;
    spinlock_release(&c->lock);
}

void Counter_Decrement(counter_t *c) {
    spinlock_acquire(&c->lock);
    c->value -= 1;
    spinlock_release(&c->lock);
}
