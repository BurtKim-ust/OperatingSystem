/*
ECE 357 - Fall 2025
Pset 5 Problem 3
Jonghyeok Kim
Sources = https://www.reddit.com/r/cprogramming/comments/18rtmtv/whats_going_on_here_with_the_output_of_printf/
          https://stackoverflow.com/questions/51000843/sigaction-doesnt-catch-sigint-signal
          https://github.com/tmbinc/bgrep/blob/master/bgrep.c (getopt part only) 
          https://stackoverflow.com/questions/47866913/how-to-correctly-handle-sigbus-so-i-can-continue-to-search-an-address
          https://stackoverflow.com/questions/10330419/using-siglongjmp-effectively
          https://forums.anandtech.com/threads/c-getopt-not-working-properly-and-probably-other-things-later.2468133/
          https://www.qnx.com/developers/docs/8.0/com.qnx.doc.neutrino.lib_ref/topic/s/siglongjmp.html
          https://www.lemoda.net/c/mmap-example/
          https://stackoverflow.com/questions/7222164/mmap-an-entire-large-file
          https://man7.org/linux/man-pages/man2/mmap.2.html
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // optarg, optind
#include <fcntl.h>
#include <sys/mman.h> 
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h> // isprint
#include <setjmp.h> // sigsetjmp/siglongjmp to recover from SIGBUS

static sigjmp_buf env;
static char *cur_file = NULL; // current file name

void sigbus_handler(int sig) {
    siglongjmp(env, 1); // returning 1
}

void print_context(unsigned char *data, size_t match_pos, size_t pat_len, size_t file_size, int context_bytes) { // pat_len: pattern length
    size_t context_start = (match_pos >= (size_t)context_bytes) ? match_pos - context_bytes : 0;
    size_t context_end = (match_pos + pat_len + context_bytes <= file_size) ? match_pos + pat_len + context_bytes : file_size;
    
    // Print characters and non-printable (='?')
    for (size_t i = context_start; i < context_end; i++) {
        if (isprint(data[i])) {
            printf("%c ", data[i]);
        } else {
            printf("? ");
        }
    }
    
    printf("   ");
    
    // Hex values
    for (size_t i = context_start; i < context_end; i++) {
        printf("%02X ", data[i]);
    }
    
    printf("\n");
}

// When processing a file
int process_file(char *filename, unsigned char *pattern, size_t pat_len, int context_bytes, int *found_check) {
    int fd = -1;
    void *map = MAP_FAILED;
    struct stat sb;
    
    // If no files are named then bgrep's sole input file is standard input.
    if (strcmp(filename, "<standard input>") == 0) {
        fd = STDIN_FILENO;
    } else {
        fd = open(filename, O_RDONLY);
        if (fd == -1) {
            fprintf(stderr, "Can't open %s for reading:%s\n", filename, strerror(errno));
            return -1;
        }
    }
    
    if (fstat(fd, &sb) == -1) {
        fprintf(stderr, "Can't stat %s: %s\n", filename, strerror(errno));
        if (strcmp(filename, "<standard input>") != 0) close(fd);
        return -1;
    }
    
    
    // Memory mapping
    map = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) {
        fprintf(stderr, "Can't mmap %s:%s\n", filename, strerror(errno));
        if (strcmp(filename, "<standard input>") != 0) close(fd);
        return -1;
    }
    
    unsigned char *data = (unsigned char *)map; 
    
    if ((size_t)sb.st_size < pat_len) {
        // 'file size < pattern size' => unmap and close
        munmap(map, sb.st_size);
        if (strcmp(filename, "<standard input>") != 0) close(fd);
        return 0;
    }
    
    // Check for pattern matches
    for (size_t i = 0; i <= sb.st_size - pat_len; i++) {
        if (memcmp(data + i, pattern, pat_len) == 0) {
            *found_check = 1; // match found
            printf("%s:%zu ", filename, i); // location of the match
            
            if (context_bytes > 0) {
                print_context(data, i, pat_len, sb.st_size, context_bytes);
            } else {
                printf("\n");
            }
        }
    }

    munmap(map, sb.st_size); // Unmap is necessary to avoid using all virtual memory.
    if (strcmp(filename, "<standard input>") != 0) {
        close(fd);
    }
    
    return 0;
}

// Read pattern from a file
unsigned char *read_pattern_file(char *filename, size_t *pat_len) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Can't open pattern file %s:%s\n", filename, strerror(errno));
        return NULL;
    }
    
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        fprintf(stderr, "Can't stat pattern file %s:%s\n", filename, strerror(errno));
        close(fd);
        return NULL;
    }
    
    unsigned char *pattern = malloc(sb.st_size);
    ssize_t bytes_read = read(fd, pattern, sb.st_size);

    // Normal case
    *pat_len = sb.st_size;
    close(fd);
    return pattern;
}

int main(int argc, char *argv[]) {
    int opt;
    char *pattern_file = NULL;
    int context_bytes = 0;
    unsigned char *pattern = NULL;
    size_t pat_len = 0;
    int found_check = 0;
    int error_occurred = 0;
    
    // SIGBUS handling
    struct sigaction sa;
    sa.sa_handler = sigbus_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGBUS, &sa, NULL) < 0) {
        fprintf(stderr, "Error creating sigaction for SIGBUS handler\n");
        return 255;
    }
    
    // Parse options
    while ((opt = getopt(argc, argv, "p:c:")) != -1) {
        switch (opt) {
            case 'p':
                pattern_file = optarg; // pattern file name
                break;
            case 'c':
                context_bytes = atoi(optarg); // the number of context bytes
                if (context_bytes < 0) {
                    fprintf(stderr, "Invalid context bytes: %s\n", optarg);
                    return 255;
                }
                break;
            default:
                fprintf(stderr, "Usage: \n");
                fprintf(stderr, "   bgrep {OPTIONS} -p pattern_file {file1 {file 2} ...}\n");
                fprintf(stderr, "   bgrep {OPTIONS} pattern {file1 {file2} ...}\n");
                fprintf(stderr, "Options: [-p pattern_file], [-c context_bytes]\n");
                return 255;
        }
    }
    
    // If pattern_file exists, read pattern from file
    // Case A: bgrep {OPTIONS} -p pattern_file {file1 {file 2} ...}
    if (pattern_file) {
        pattern = read_pattern_file(pattern_file, &pat_len);
        if (!pattern) {
            return 255;
        }
    } else {
        // Case B: bgrep {OPTIONS} pattern {file1 {file2} ...}
        if (optind >= argc) { // optind = index of next argument
            fprintf(stderr, "No pattern specified\n");
            fprintf(stderr, "Usage: \n");
            fprintf(stderr, "   bgrep {OPTIONS} -p pattern_file {file1 {file 2} ...}\n");
            fprintf(stderr, "   bgrep {OPTIONS} pattern {file1 {file2} ...}\n");
            return 255;
        }
        pattern = (unsigned char *)argv[optind];
        pat_len = strlen(argv[optind]);
        optind++;
    }
    
    if (optind >= argc) {
        cur_file = "<standard input>";
        
        if (sigsetjmp(env, 1) == 0) {
            // Normal execution
            int result = process_file(cur_file, pattern, pat_len, context_bytes, &found_check);
            if (result < 0) {
                error_occurred = 1;
            }
        } else {
            // Returned from siglongjmp - SIGBUS occurred
            fprintf(stderr, "SIGBUS received while processing file %s\n", cur_file);
            error_occurred = 1;
        }
    } else {
        // Process each file
        for (int i = optind; i < argc; i++) {
            cur_file = argv[i];
            
            if (sigsetjmp(env, 1) == 0) { 
                // Normal execution
                int result = process_file(argv[i], pattern, pat_len, context_bytes, &found_check);
                if (result < 0) {
                    error_occurred = 1;
                }
            } else {
                // Return from siglongjmp when SIGBUS occurs
                fprintf(stderr, "SIGBUS received while processing file %s\n", cur_file);
                error_occurred = 1;
            }
        }
    }
    
    // This ensures that the execution returns exit code after all files are processed.
    if (error_occurred) {
        return 255;
    } else if (found_check) {
        return 0;
    } else {
        return 1;
    }
}
