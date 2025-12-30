// source: p6p1

#include "sem.h"
#include <signal.h>
#include <unistd.h>
#include <string.h>

static void sigusr1_handler(int sig) {}

/* Initialize the semaphore *s with the initial count. Initialize any underlying data structures. 
   Called once in the program (per semaphore).*/
void sem_init(struct sem *s, int count) {
    s->count = count;
    s->lock.lock = 0;
    s->num_waiting = 0;
    memset(s->waiting, 0, sizeof(s->waiting));
    
    signal(SIGUSR1, sigusr1_handler);
}

/* Attempt to perform the "P" operation (atomically decrement the semaphore). 
   If this operation would block, return 0, otherwise return 1. */
int sem_try(struct sem *s) {
    spin_lock(&s->lock);
    
    if (s->count > 0) {
        s->count--;
        spin_unlock(&s->lock);
        return 1;  // success
    }
    
    spin_unlock(&s->lock);
    return 0;  // blocked
}

// Perform the P operation, blocking until successful.
void sem_wait(struct sem *s) {
    sigset_t new_mask, old_mask;
    
    // Block signal SIGUSR1 
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &new_mask, &old_mask);
    
    while (1) {
        spin_lock(&s->lock);
        
        if (s->count > 0) {
            s->count--;
            spin_unlock(&s->lock);
            sigprocmask(SIG_SETMASK, &old_mask, NULL);
            return;
        }
        
        s->waiting[s->num_waiting++] = getpid(); // add to waiting list
        spin_unlock(&s->lock);
        
        sigsuspend(&old_mask); // wait for SIGUSR1
        
    }
}

/* Perform the V operation. If any other tasks were sleeping on this semaphore, wake them by sending a SIGUSR1 to
their process id (which is not the same as the virtual processor number). If there are multiple sleepers (this would
happen if multiple virtual processors attempt the P operation while the count is <1) then all must be sent the wakeup
signal. */
void sem_inc(struct sem *s) {
    spin_lock(&s->lock);
    s->count++;
    
     if (s->num_waiting > 0) {
        for (int i = 0; i < s->num_waiting; i++) {
            kill(s->waiting[i], SIGUSR1);
        }
        s->num_waiting = 0;
    }

    spin_unlock(&s->lock);
}