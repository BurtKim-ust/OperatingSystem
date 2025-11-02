// ECE 357 - Fall 2025
// Jonghyeok (Burt) Kim
// Pset 1 - 3
// Reference: https://www.programiz.com/c-programming/c-structures
//            https://developer.unigine.com/en/docs/2.13/code/uniginescript/language/preprocessor?implementationLanguage=cpp

#ifndef MYLIB_H
#define MYLIB_H
#include <stdio.h>

struct MYSTREAM {
    int fd;
    char *buffer;
    int mode; // 'r' or 'w'
    int eof;
    size_t buf_size;
    size_t buf_pos; 
    size_t buf_len;  
};

struct MYSTREAM *myfopen(const char *pathname, const char *mode, int bufsiz);
struct MYSTREAM *myfdopen(int filedesc, const char *mode, int bufsiz);
int myfgetc(struct MYSTREAM *stream);
int myfputc(int c, struct MYSTREAM *stream);
int myfclose(struct MYSTREAM *stream);

#endif