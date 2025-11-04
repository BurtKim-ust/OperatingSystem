// OS Pset4 2025
// Jonghyeok Kim
// Problem 3B - Launcher
// reference: https://stackoverflow.com/questions/39822729/piping-between-two-programs-in-c
//            Textbook Section 15.2

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/resource.h>

void close_pipes(int fd1[], int fd2[]) {
    close(fd1[0]);
    close(fd1[1]);
    close(fd2[0]);
    close(fd2[1]);
}

int main(int argc, char *argv[])
{
    int fd1[2], fd2[2];  // fd1: Pipe between wordgen and wordsearch
                         // fd2: Pipe between wordsearch and pager
    pid_t pid1, pid2, pid3;
    int status;
    struct rusage usage;
    
    // First pipe between wordgen and wordsearch
    if (pipe(fd1) < 0) {
        fprintf(stderr, "Pipe 1 Error: %s\n", strerror(errno));
        return 1;
    }
    
    // Second pipe between wordsearch and pager
    if (pipe(fd2) < 0) {
        fprintf(stderr, "Pipe 2 Error: %s\n", strerror(errno));
        return 1;
    }
    
    // wordgen
    if ((pid1 = fork()) < 0) {
        fprintf(stderr, "Error forking in wordgen: %s\n", strerror(errno));
        return 1;
    } else if (pid1 == 0) { 
        // write to pipe1
        if (dup2(fd1[1], 1) < 0) {
            fprintf(stderr, "Error duping stdout to pipe: %s\n", strerror(errno));
            exit(1);
        }
        
        close_pipes(fd1, fd2); // close all pipe ends (in child)
        
        // Execute wordgen 
        if (execlp("./wordgen", "wordgen", argv[1], (char *)NULL) < 0) {
            fprintf(stderr, "Error executing wordgen: %s\n", strerror(errno));
        }
        exit(1);
    }
    
    // wordsearch
    if ((pid2 = fork()) < 0) {
        fprintf(stderr, "Error forking in wordsearch: %s\n", strerror(errno));
        return 1;
    } else if (pid2 == 0) {
        // wordgen output to pipe1 read end
        if (dup2(fd1[0], 0) < 0) {
            fprintf(stderr, "Error duping stdin from pipe: %s\n", strerror(errno));
            exit(1);
        }
        
        // wordsearch output to pipe2 write end
        if (dup2(fd2[1], 1) < 0) {
            fprintf(stderr, "Error duping stdout to pipe: %s\n", strerror(errno));
            exit(1);
        }
        
        close_pipes(fd1, fd2);
        
        if (execlp("./wordsearch", "wordsearch", "words.txt", (char *)NULL) < 0) {
            fprintf(stderr, "Error executing wordsearch: %s\n", strerror(errno));
        }
        exit(1);
    }
    
    // pager
    if ((pid3 = fork()) < 0) {
        fprintf(stderr, "Error forking in pager: %s\n", strerror(errno));
        return 1;
    } else if (pid3 == 0) {
        // wordsearch output to pipe2 read end
        if (dup2(fd2[0], 0) < 0) {
            fprintf(stderr, "Error duping stdin from pipe: %s\n", strerror(errno));
            exit(1);
        }
        
        close_pipes(fd1, fd2);
        
        if (execlp("./pager", "pager", (char *)NULL) < 0) {
            fprintf(stderr, "Error executing pager: %s\n", strerror(errno));
        }
        exit(1);
    }
    
    close_pipes(fd1, fd2); // close all pipe ends (for parent)
    
    // Wait for all three children and report their exit statuses
    pid_t exited_pid;
    
    exited_pid = wait3(&status, 0, &usage);
    fprintf(stderr, "Child %d exited with %d\n", exited_pid, status);
    
    exited_pid = wait3(&status, 0, &usage);
    fprintf(stderr, "Child %d exited with %d\n", exited_pid, status);
    
    exited_pid = wait3(&status, 0, &usage);
    fprintf(stderr, "Child %d exited with %d\n", exited_pid, status);
    
    return 0;
}
