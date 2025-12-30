#ifndef _SEM_H
#define _SEM_H

#include "spinlock.h"
#include <sys/types.h>

#define N_PROC 64

typedef struct sem{
    int count; // semaphore count
    spinlock_t lock;
    pid_t waiting[N_PROC]; // PIDs of waiting processes; this way will be used instead of my_procnum.
    int num_waiting; // number of waiting processes
} sem;

void sem_init(struct sem *s, int count);
int sem_try(struct sem *s);
void sem_wait(struct sem *s);
void sem_inc(struct sem *s);

#endif