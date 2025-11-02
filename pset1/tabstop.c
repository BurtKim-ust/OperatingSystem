// ECE 357 - Fall 2025
// Jonghyeok (Burt) Kim
// Problem 4, 5
// Reference: https://www.educba.com/stderr-in-c/
//            https://www.linuxquestions.org/questions/programming-9/anyone-know-a-better-means-to-track-argc-in-case-using-getopt-4175610994/

#include "mylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

void usage() {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  tabstop [-b buffer_size] -o outfile infile\n");
    fprintf(stderr, "  tabstop [-b buffer_size] -o outfile\n");
    fprintf(stderr, "  tabstop [-b buffer_size] infile\n");
    fprintf(stderr, "  tabstop [-b buffer_size]\n");
    fprintf(stderr, "  Default buffer size is 8192 bytes\n");
}

int main(int argc, char *argv[]) {
    struct MYSTREAM *input = NULL;
    struct MYSTREAM *output = NULL;
    char *input_file = NULL;
    char *output_file = NULL;
    int buffer_size = 8192; // prob 5 applied
    int c;
    int i;
    
    // Parse
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-b") == 0) {
            // Next argument = buffer size
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: -b requires a buffer size\n");
                usage();
                return 255;
            }
            buffer_size = atoi(argv[++i]);
            if (buffer_size <= 0) {
                fprintf(stderr, "Error: Invalid buffer size '%s'\n", argv[i]);
                usage();
                return 255;
            }
        } else if (strcmp(argv[i], "-o") == 0) {
            // Next argument = output filename
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: -o requires a filename\n");
                usage();
                return 255;
            }
            output_file = argv[++i];
        } else { // else input filename
            if (input_file != NULL) {
                fprintf(stderr, "Error: Multiple input files specified\n");
                usage();
                return 255;
            }
            input_file = argv[i];
        }
    }
    
    // Input stream
    if (input_file != NULL) {
        // Input file case
        input = myfopen(input_file, "r", buffer_size);
        if (input == NULL) {
            perror("Error opening input file");
            return 255;
        }
    } else {
        // Standard input case
        input = myfdopen(0, "r", buffer_size);
        if (input == NULL) {
            perror("Error opening stdin");
            return 255;
        }
    }
    
    // Output stream
    if (output_file != NULL) {
        // Output file case
        output = myfopen(output_file, "w", buffer_size);
        if (output == NULL) {
            perror("Error opening output file");
            myfclose(input);
            return 255;
        }
    } else {
        // Standard output case
        output = myfdopen(1, "w", buffer_size);
        if (output == NULL) {
            perror("Error opening stdout");
            myfclose(input);
            return 255;
        }
    }
    
    // \t -> 4 spaces
    while ((c = myfgetc(input)) != EOF) {
        if (c == '\t') {
            for (int j = 0; j < 4; j++) {
                if (myfputc(' ', output) == -1) {
                    perror("Error writing space");
                    myfclose(input);
                    myfclose(output);
                    return 255;
                }
            }
        } else {
            // no change for the characters
            if (myfputc(c, output) == -1) {
                perror("Error writing character");
                myfclose(input);
                myfclose(output);
                return 255;
            }
        }
    }
    
    // Read error check
    if (errno != 0) {
        perror("Error reading input");
        myfclose(input);
        myfclose(output);
        return 255;
    }
    
    // Close streams
    if (myfclose(input) == -1) {
        perror("Error closing input");
        myfclose(output);
        return 255;
    }
    
    if (myfclose(output) == -1) {
        perror("Error closing output");
        return 255;
    }
    
    return 0;
}
