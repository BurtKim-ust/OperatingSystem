// OS Pset4 2025
// Jonghyeok Kim 
// reference: https://stackoverflow.com/questions/35181913/converting-char-to-uppercase-in-c
//            https://www.geeksforgeeks.org/cpp/isalpha-isdigit-functions-c-example/
//            https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>

// Assumption: Max 2M lines & Max 100 characters per word
char dictionary[2000000][100];
int word_count = 0;
int matched = 0;

void sigpipe_handler(int signum) {
    fprintf(stderr, "Matched %d words\n", matched);
    exit(0);
}

// To reject words that have characters other than English alphabets
int only_alphabets(char *word) {
    for (int i = 0; word[i] && word[i] != '\n'; i++) {
        if (!isalpha(word[i])) {
            return 0;
        }
    }
    return 1;
}

// Linear search
int lin_search(char *word) {
    for (int i = 0; i < word_count; i++) {
        if (strcmp(word, dictionary[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char **argv)
{
    FILE *input_file;
    char word[100];
    int accepted = 0, rejected = 0;
    
    signal(SIGPIPE, sigpipe_handler);
    
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    
    // Input file
    input_file = fopen(argv[1], "r");
    if (!input_file) {
        fprintf(stderr, "Error: cannot open file %s\n", argv[1]);
        return 1;
    }

    while (fgets(word, sizeof(word), input_file) && word_count < 2000000) {
        word[strcspn(word, "\r\n")] = 0; // remove newline
        
        // Reject words that have non-alphabet characters
        if (only_alphabets(word)) {
            // Convert to uppercase letters
            for (int i = 0; word[i]; i++) {
                word[i] = toupper(word[i]);
            }
            strcpy(dictionary[word_count], word);
            word_count++;
            accepted++;
        } else {
            rejected++;
        }
    }
    fclose(input_file);
    
    fprintf(stderr, "Accepted %d words, rejected %d\n", accepted, rejected);
    
    // Read from stdin and count matched words
    while (fgets(word, sizeof(word), stdin)) {
        word[strcspn(word, "\r\n")] = 0; 
        
        if (lin_search(word)) {
            printf("%s\n", word);
            matched++;
        }
    }
    
    fflush(stdout);
    // When I initially flushed inside the loop, but it caused child exit line
    // earlier than expected. By placing it here, it is flushed after all matches
    // are printed.
    fprintf(stderr, "Matched %d words\n", matched);
    return 0;
}