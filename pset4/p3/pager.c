// OS Pset4 2025
// Jonghyeok Kim 

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    FILE *term;
    int count = 0;
    char *word = NULL;
    size_t len = 0;
    ssize_t nread;
    char response[100]; // Assumption: max 100 characters for each response
    
    term = fopen("/dev/tty", "r");
    
    // Read and display lines from stdin
    while ((nread = getline(&word, &len, stdin)) != -1) {
        
        printf("%s", word);
        count++;
       
        if (count % 23 == 0) {
            
            printf("---Press RETURN for more---\n");
            fflush(stdout);
            // q or Q to quit
            if (fgets(response, sizeof(response), term)) {
                if (response[0] == 'q' || response[0] == 'Q') {
                    fprintf(stderr, "*** Pager terminated by Q command ***\n");
                    return 0;
                }
            }
            count = 0;  // resets counter for next page
        }
    }
    
    return 0;
}