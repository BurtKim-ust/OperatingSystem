#include "spinlock.h"
#include <sched.h> 
#include "tas.h" 

void spin_lock(spinlock_t *lock){
    while(tas(&lock->lock)){ // if locked, keep trying.
        sched_yield(); // yield the CPU to allow other threads to run
    }
}

void spin_unlock(spinlock_t *lock)
{
    lock->lock = 0;
}