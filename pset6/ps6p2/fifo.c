#include "fifo.h"
// sources: https://csresources.github.io/SystemProgrammingWiki/SystemProgramming/Synchronization,-Part-8:-Ring-Buffer-Example/

// Initialize the shared memory FIFO *f including any required underlying initializations.
void fifo_init(struct myfifo *f) {
    sem_init(&f->slots, MYFIFO_BUFSIZ);
    sem_init(&f->items, 0);  
}

// Enqueue the data word d into the FIFO, blocking unless and until the FIFO has room to accept it.
void fifo_wr(struct myfifo *f, unsigned long d) {
    sem_wait(&f->slots);   
    spin_lock(&f->lock); // remove to break the sync

    f->buffer[f->tail] = d; // store data
    f->tail = (f->tail + 1) % MYFIFO_BUFSIZ; // move to next position

    spin_unlock(&f->lock); // remove to break the sync
    sem_inc(&f->items);           
}

/* Dequeue the next data word from the FIFO and return it. 
   Block unless and until there are available words queued in the FIFO.*/
unsigned long fifo_rd(struct myfifo *f) {
    sem_wait(&f->items);               
    spin_lock(&f->lock); // remove to break the sync

    unsigned long d = f->buffer[f->head]; // read data
    f->head = (f->head + 1) % MYFIFO_BUFSIZ; // move to next position

    spin_unlock(&f->lock); // remove to break the sync
    sem_inc(&f->slots);         

    return d;
}