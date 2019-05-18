#ifndef _SPIN_H
#define _SPIN_H

typedef volatile unsigned int spinlock_t;

void spinlock_acquire(spinlock_t *lock);
void spinlock_release(spinlock_t *lock);

#endif
