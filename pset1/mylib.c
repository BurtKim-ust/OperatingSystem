// ECE 357 - Fall 2025
// Jonghyeok (Burt) Kim
// Pset 1 - Problem 5 (Extra Credit)
// Reference: https://www.netlib.org/crc/oldmasterslave.c
//            https://man7.org/linux/man-pages/man3/errno.3.html
//            https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-read-read-from-file-socket
//            https://linux.die.net/man/2/write#:~:text=write(2)%20%2D%20Linux%20man,also%20pipe(7).)

#include "mylib.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

static struct MYSTREAM *init_stream(int fd, const char *mode, int buf_size) {
    struct MYSTREAM *stream = malloc(sizeof(struct MYSTREAM));
    if (stream == NULL) {
        return NULL;
    }
    
    stream->buffer = malloc(buf_size);
    if (stream->buffer == NULL) {
        free(stream);
        return NULL;
    }
    
    stream->fd = fd;
    stream->buf_size = buf_size;
    stream->buf_pos = 0;
    stream->buf_len = 0;
    stream->mode = (mode[0] == 'r') ? 'r' : 'w';
    stream->eof = 0;
    
    return stream;
}

struct MYSTREAM *myfopen(const char *pathname, const char *mode, int buf_size) {
    if (mode == NULL || (strcmp(mode, "r") != 0 && strcmp(mode, "w") != 0)) {
        errno = EINVAL;
        return NULL;
    }
    
    if (buf_size <= 0) {
        errno = EINVAL;
        return NULL;
    }
    
    int flags;
    if (mode[0] == 'r') {
        flags = O_RDONLY;
    } else { // mode[0] == 'w'
        flags = O_WRONLY | O_CREAT | O_TRUNC;
    }
    
    int fd = open(pathname, flags, 0666);
    if (fd == -1) {
        return NULL;
    }
    
    struct MYSTREAM *stream = init_stream(fd, mode, buf_size);
    if (stream == NULL) {
        close(fd);
        errno = ENOMEM;
        return NULL;
    }
    
    return stream;
}

struct MYSTREAM *myfdopen(int filedesc, const char *mode, int buf_size) {
    if (mode == NULL || (strcmp(mode, "r") != 0 && strcmp(mode, "w") != 0)) {
        errno = EINVAL;
        return NULL;
    }
    
    if (buf_size <= 0) {
        errno = EINVAL;
        return NULL;
    }
    
    struct MYSTREAM *stream = init_stream(filedesc, mode, buf_size);
    if (stream == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    
    return stream;
}

int myfgetc(struct MYSTREAM *stream) {
    if (stream == NULL || stream->mode != 'r') {
        errno = EINVAL;
        return EOF;
    }
    
    if (stream->eof) {
        errno = 0;
        return EOF;
    }
    
    if (stream->buf_pos >= stream->buf_len) {
        ssize_t bytes_read = read(stream->fd, stream->buffer, stream->buf_size);
        
        if (bytes_read == -1) {
            return EOF;
        }
        
        if (bytes_read == 0) {
            stream->eof = 1;
            errno = 0;
            return EOF;
        }
        
        stream->buf_len = bytes_read;
        stream->buf_pos = 0;
    }
    
    unsigned char c = stream->buffer[stream->buf_pos];
    stream->buf_pos++;
    return (int)c;
}

int myfputc(int c, struct MYSTREAM *stream) {
    if (stream == NULL || stream->mode != 'w') {
        errno = EINVAL;
        return -1;
    }
    
    stream->buffer[stream->buf_pos] = (char)c;
    stream->buf_pos++;
    
    if (stream->buf_pos >= stream->buf_size) {
        ssize_t bytes_written = write(stream->fd, stream->buffer, stream->buf_pos);
        
        if (bytes_written == -1) {
            return -1;
        }
        
        if (bytes_written == 0) {
            errno = EIO;
            return -1;
        }
        
        if (bytes_written != (ssize_t)stream->buf_pos) {
            errno = EIO;
            return -1;
        }

        stream->buf_pos = 0;
    }
    
    return c;
}

int myfclose(struct MYSTREAM *stream) {
    if (stream == NULL) {
        errno = EINVAL;
        return -1;
    }
    
    int result = 0;
    
    if (stream->mode == 'w' && stream->buf_pos > 0) {
        ssize_t bytes_written = write(stream->fd, stream->buffer, stream->buf_pos);
        
        if (bytes_written == -1 || bytes_written != (ssize_t)stream->buf_pos) {
            result = -1;
        }
    }
    
    if (close(stream->fd) == -1) {
        result = -1;
    }
    
    free(stream->buffer);
    free(stream);
    
    return result;
}