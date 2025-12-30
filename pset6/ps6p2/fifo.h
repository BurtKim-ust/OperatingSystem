// sources: https://forum.arduino.cc/t/simple-fifo-library/45414

#ifndef _FIFO_H
#define _FIFO_H

#include "sem.h"

#define MYFIFO_BUFSIZ 4096

typedef struct myfifo {
    unsigned long buffer[MYFIFO_BUFSIZ];
    int head; 
    int tail;     
    sem slots;
    sem items;
    spinlock_t lock;  
} fifo;

void fifo_init(struct myfifo *f);
void fifo_wr(struct myfifo *f, unsigned long d);
unsigned long fifo_rd(struct myfifo *f);

#endif