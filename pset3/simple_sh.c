/*
    ECE 357 Probset 3 - Q3 
    Burt Kim
    Compile: gcc -o simple_sh simple_sh.c
    Usage: ./simple_sh [script_file]
    References: 
    1. https://stackoverflow.com/questions/2125219/how-to-get-the-running-of-time-of-my-program-with-gettimeofday
    2. https://support.sas.com/documentation/onlinedoc/sasc/doc750/html/lr2/zid-7807.htm#:~:text=The%20getrusage%20function%20returns%20resource,rusage%20structure%20contains%20two%20fields:
    3. https://pubs.opengroup.org/onlinepubs/9699919799/functions/wait.html
    4. https://www.geeksforgeeks.org/c/wait-system-call-c/
    5. https://stackoverflow.com/questions/1101957/are-there-any-standard-exit-status-codes-in-linux
    6. https://www.gnu.org/software/c-intro-and-ref/manual/html_node/Environment-Variables.html#:~:text=Environment%20Variables%20(GNU%20C%20Language%20Manual)  
    7. https://www.qualys.com/2017/06/19/stack-clash/solaris_rsh.c
    8. https://stackoverflow.com/questions/33485011/chdirgetenvhome-prompts-error-no-such-file-or-directory
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

// Default last exit status = 0
int last_exit_status = 0;

// Redirect given file to given file descriptor with specified flags
void redirect(char *file, int fd, int flags) {
    int new_fd = open(file, flags, 0644);
    if (new_fd < 0) {
        fprintf(stderr, "Error: Cannot open file '%s' for redirection: %s\n", file, strerror(errno));
        exit(1);
    }
    if (dup2(new_fd, fd) < 0) {
        fprintf(stderr, "Error: Failed to redirect to fd %d: %s\n", fd, strerror(errno));
        exit(1);
    }
    close(new_fd);
}

// Close all file descriptors except 0, 1, and 2
void close_extra_fds(void) {
    int maxfd = sysconf(_SC_OPEN_MAX);
    if (maxfd < 0) maxfd = 100; // Assuming max 100 for simplicity
    
    for (int fd = 3; fd < maxfd; fd++) {
        close(fd);
    }
}
    
// Parse
void process_line(char *line) {
    // Skip empty lines and comments
    if (line[0] == '\0' || line[0] == '\n' || line[0] == '#') return;
    
    char *args[100];
    char *stdin_file = NULL;
    char *stdout_file = NULL;
    char *stderr_file = NULL;
    int stdout_flags = 0;
    int stderr_flags = 0;
    int arg_index = 0;
    
    // Strip all \r characters from the line first
    char cleaned_line[1024];
    int j = 0;
    for (int i = 0; line[i] && j < 1023; i++) {
        if (line[i] != '\r') {
            cleaned_line[j++] = line[i];
        }
    }
    cleaned_line[j] = '\0';
    
    // Parse the cleaned line
    char *token = strtok(cleaned_line, " \t\n");
    
    while (token != NULL) {
        int handled = 0;
        
        // Check for operators
        if (token[0] == '<' && token[1] != '\0') { // <file
            stdin_file = token + 1;
            handled = 1;
        }
        else if (strcmp(token, "<") == 0) { // < file
            token = strtok(NULL, " \t\n");
            if (token) stdin_file = token;
            handled = 1;
        }
        else if (token[0] == '>' && token[1] == '>' && token[2] != '\0') { // >>file
            stdout_file = token + 2;
            stdout_flags = O_WRONLY | O_CREAT | O_APPEND;
            handled = 1;
        }
        else if (strcmp(token, ">>") == 0) { // >> file
            token = strtok(NULL, " \t\n");
            if (token) {
                stdout_file = token;
                stdout_flags = O_WRONLY | O_CREAT | O_APPEND;
            }
            handled = 1;
        }
        else if (token[0] == '>' && token[1] != '>' && token[1] != '\0') { // >file
            stdout_file = token + 1;
            stdout_flags = O_WRONLY | O_CREAT | O_TRUNC;
            handled = 1;
        }
        else if (strcmp(token, ">") == 0) { // > file
            token = strtok(NULL, " \t\n");
            if (token) {
                stdout_file = token;
                stdout_flags = O_WRONLY | O_CREAT | O_TRUNC;
            }
            handled = 1;
        }
        else if (strncmp(token, "2>>", 3) == 0 && token[3] != '\0') { // 2>>file
            stderr_file = token + 3;
            stderr_flags = O_WRONLY | O_CREAT | O_APPEND;
            handled = 1;
        }
        else if (strcmp(token, "2>>") == 0) { // 2>> file
            token = strtok(NULL, " \t\n");
            if (token) {
                stderr_file = token;
                stderr_flags = O_WRONLY | O_CREAT | O_APPEND;
            }
            handled = 1;
        }
        else if (strncmp(token, "2>", 2) == 0 && token[2] != '\0') { // 2>file
            stderr_file = token + 2;
            stderr_flags = O_WRONLY | O_CREAT | O_TRUNC;
            handled = 1;
        }
        else if (strcmp(token, "2>") == 0) { // 2> file
            token = strtok(NULL, " \t\n");
            if (token) {
                stderr_file = token;
                stderr_flags = O_WRONLY | O_CREAT | O_TRUNC;
            }
            handled = 1;
        }
        
        if (!handled) {
            args[arg_index++] = token;
        }
        
        token = strtok(NULL, " \t\n");
    }

    args[arg_index] = NULL; // because execvp() requires null-terminated array

    if (arg_index == 0) return; // if nothing is parsed, return

    // Built-in commands
    if (strcmp(args[0], "cd") == 0) {
        char *dir = args[1]; // target directory
        if (dir == NULL) {
            dir = getenv("HOME");
        }
        if (dir == NULL) {
            fprintf(stderr, "Error: HOME not set\n");
            last_exit_status = 1;
        } else if (chdir(dir) < 0) { // changes the current directory
            fprintf(stderr, "Error: Cannot change directory to '%s': %s\n", dir, strerror(errno));
            last_exit_status = 1;
        } else {
            last_exit_status = 0;
        }
        return;
    }
    
    if (strcmp(args[0], "pwd") == 0) {
        char cwd[1024]; // assuming max path length 1024
        if (getcwd(cwd, sizeof(cwd)) != NULL) { // gets the current directory
            printf("%s\n", cwd);
            last_exit_status = 0;
        } else {
            fprintf(stderr, "Error: Cannot get current directory: %s\n", strerror(errno));
            last_exit_status = 1;
        }
        return;
    }
    
    if (strcmp(args[0], "exit") == 0) {
        int exit_code = last_exit_status;
        if (args[1] != NULL) {
            exit_code = atoi(args[1]); // conversion from string to int is needed here
        }
        exit(exit_code);
    }

    struct timeval start, end;
    struct rusage usage;
    gettimeofday(&start, NULL); 
    
    // External command execution
    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Error: Fork failed: %s\n", strerror(errno));
        last_exit_status = 1;
        return;
    }
        
    // Child process
    if (pid == 0) {
        if (stdin_file) redirect(stdin_file, STDIN_FILENO, O_RDONLY);
        if (stdout_file) redirect(stdout_file, STDOUT_FILENO, stdout_flags);
        if (stderr_file) redirect(stderr_file, STDERR_FILENO, stderr_flags);
        
        close_extra_fds(); // keeps only 0, 1, 2 open
        
        execvp(args[0], args);
        fprintf(stderr, "Error: Command '%s' not found: %s\n", args[0], strerror(errno));
        exit(127);
    }
    
    // Parent process
    int status;
    wait3(&status, 0, &usage); // waits for child and get resource usage
    gettimeofday(&end, NULL);
    
    // process time calculations
    double real_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    double user_time = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1000000.0;
    double sys_time = usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1000000.0;
    
    if (WIFEXITED(status)) { // Child terminated normally
        last_exit_status = WEXITSTATUS(status); // exit status of the child
        if (last_exit_status == 0) { 
            fprintf(stderr, "Child process id: %d exited normally\n", pid);
        } else {
            fprintf(stderr, "Child process id: %d. Last exit status: %d\n", pid, last_exit_status);
        }
    } else if (WIFSIGNALED(status)) { // the child process was terminated by a signal
        int sig = WTERMSIG(status); // Error signal number
        last_exit_status = 128 + sig;
        fprintf(stderr, "Child process id: %d. exit signal: %d (%s)\n", pid, sig, strsignal(sig));
    }
    
    fprintf(stderr, "Real: %.3fs User: %.3fs Sys: %.3fs\n", real_time, user_time, sys_time);
}


// Read_stdin mode
void read_stdin(void) {
    char line[1024];
    
    while (fgets(line, sizeof(line), stdin) != NULL) {
        process_line(line);
        if (feof(stdin)) {
            break;
        }
    }
    
    fprintf(stderr, "end of file read. Last exit status: %d\n", last_exit_status);
}


// Script file mode
void script_interpret(const char *filename) {
    FILE *script = fopen(filename, "r");
    if (script == NULL) {
        fprintf(stderr, "Error: Cannot open script file '%s': %s\n", filename, strerror(errno));
        exit(1);
    }
    
    char line[1024]; // assuming max line length 1024
    while (fgets(line, sizeof(line), script) != NULL) {
        process_line(line);
    }
    
    fclose(script);
    fprintf(stderr, "end of file read. Last exit status:  %d\n", last_exit_status);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        read_stdin(); // Reads from stdin
    } else if (argc == 2) {
        script_interpret(argv[1]); // Reads from file
    } else {
        fprintf(stderr, "Error: Too many arguments. Max one argument allowed. \n");
        exit(1);
    }
    
    exit(last_exit_status); // 0: success, 1: failure, 127: cmd not found
                            // For signals: WTERMSIG (error signal number)
}