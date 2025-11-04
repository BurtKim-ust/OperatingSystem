// OS Pset4 2025
// Jonghyeok Kim 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
// Reference: https://stackoverflow.com/questions/23146971/seeding-random-number-generator-for-a-batch-of-programs

int main(int argc, char **argv)
{
    int limit = 0;
    int count = 0;
    char c;
    
    if (argc > 1) {
        limit = atoi(argv[1]);
    }
    
    // Seed random number generator with the current time and the pid
    srand(time(0) ^ getpid());
    
    while (limit == 0 || count < limit) { // infinte loop if the argument is underfined or 0
        // 3 =< Word length =< 10
        int length = 3 + (rand() % 8);
        
        for (int i = 0; i < length; i++) {
            c = 'A' + (rand() % 26); // uppercase letters only
            write(1, &c, 1);
        }
        c = '\n';
        write(1, &c, 1);
        count++;
    }
    
    fprintf(stderr, "Finished generating %d candidate words\n", count);
    return 0;
}