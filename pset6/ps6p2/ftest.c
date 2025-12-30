// ECE357 P6P2D 
// Burt Kim
// sources: https://man7.org/tlpi/code/online/dist/mmap/anon_mmap.c.html
//          previous psets

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include "fifo.h"

struct shared { 
    fifo f; 
};

void writer(struct shared *s, int id, int n) { 
    for (int i = 0; i < n; i++)
        fifo_wr(&s->f, ((unsigned long)id << 32) | i);
        /* Assuming a 64-bit unsigned long, the upper 32 bits store the writer ID and 
           the lower 32 bits store the item number.*/

   printf("Writer %d completed\n", id);
}

void reader(struct shared *s, int nwriters, int nitems) {
    int expected[nwriters];
    int errors = 0;
    int completed[nwriters];

    for (int i = 0; i < nwriters; i++){
        expected[i] = 0;
        completed[i] = 0; 
    }

    for (int i = 0; i < nwriters * nitems; i++) {
        unsigned long val = fifo_rd(&s->f);
        int id = (int)(val >> 32); // writer ID
        int seq = (int)(val & 0xFFFFFFFF); // item number
        
        if (seq != expected[id]) {
            printf("Detected out-of-sequence word %d:%d (expecting %d)\n", id, seq, expected[id]);
            errors++;
        }
        
        expected[id]++; // to track of the last sequence number seen from each writer.

        if (!completed[id] && expected[id] == nitems) {
            printf("Reader stream %d completed\n", id);
            completed[id] = 1;
        }
    }
    
    printf("All streams done\n");
    
    exit(errors > 0);
}

int main(int argc, char *argv[]) {
    int nwriters = 0;
    int nitems = 0;
    int opt;
    
    // nwriters and nitems must be specified in the command line
    if (nwriters || nitems) {
        fprintf(stderr, "Usage: %s [-w writers] [-n items]\n", argv[0]);
        exit(1);    
    }

    while ((opt = getopt(argc, argv, "w:n:")) != -1) {
        switch (opt) {
            case 'w': 
                nwriters = atoi(optarg); 
                break;
            case 'n': 
                nitems = atoi(optarg); 
                break;
            default:
                fprintf(stderr, "Usage: %s [-w writers] [-n items]\n", argv[0]);
                exit(1);
        }
    }
    
    struct shared *s = mmap(NULL, sizeof(*s), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    printf("Beginning test with %d writers, %d items each\n", nwriters, nitems);

    fifo_init(&s->f);
    
    // Test processes
    if (fork() == 0)
        reader(s, nwriters, nitems);
    
    for (int i = 0; i < nwriters; i++)
        if (fork() == 0) { 
            writer(s, i, nitems); 
            exit(0); 
        }
    
    // Wait for reader to finish
    int st;
    wait(&st);
    int reader_failed = WEXITSTATUS(st);
    
    printf("Waiting for writer children to die\n");
    
    for (int i = 0; i < nwriters; i++)
        wait(&st);
    
    printf("All children exited normally\n");
    
    munmap(s, sizeof(*s));
    return reader_failed;
}