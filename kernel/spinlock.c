#include <kernel/spinlock.h>

#include <stdbool.h>

void init_lock(spinlock_t* l) {
     l->locked = 0;
}

static inline unsigned long save_irqdisable(void)
{
    unsigned long flags;
    asm volatile ("pushf\n\tcli\n\tpop %0" : "=r"(flags) : : "memory");
}

static inline void irqrestore(unsigned long flags)
{
    asm ("push %0\n\tpopf" : : "rm"(flags) : "memory","cc");
}

bool try_acquire_lock(spinlock_t* l) {
     bool retval = !__atomic_test_and_set(&l->locked,__ATOMIC_ACQUIRE);
     if(retval) l->f = save_irqdisable();
     return retval;
}

void acquire_lock(spinlock_t* l) {
     while(try_acquire_lock(l));
}

void release_lock(spinlock_t* l) {
     irqrestore(l->f);
     __atomic_clear(&l->locked,__ATOMIC_RELEASE);
}
