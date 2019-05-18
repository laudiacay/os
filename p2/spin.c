#include <stdlib.h>
#include "spin.h"

#define BLOCKED 1

// a memory barrier for my spinlock
// https://stackoverflow.com/questions/14950614/working-of-asm-volatile-memory
static void memory_barrier() {
    asm volatile("": : :"memory");
}

// CPU pause/yield for my spinlock
// https://stackoverflow.com/questions/4725676/how-does-x86-pause-instruction-work-in-spinlock-and-can-it-be-used-in-other-sc
static void pause_loop() {
    asm volatile("pause\n": : :"memory");
}

// from instructor code
static inline uint xchg(volatile unsigned int *addr, unsigned int newval) {
    uint result;
    asm volatile("lock; xchgl %0, %1" : "+m" (*addr), "=a" (result) : "1" (newval) : "cc");
    return result;
}

void spinlock_acquire(spinlock_t *lock) {
    while (xchg(lock, BLOCKED) == BLOCKED) pause_loop();
}

void spinlock_release(spinlock_t *lock) {
    memory_barrier();
    xchg(lock, !BLOCKED);
}
