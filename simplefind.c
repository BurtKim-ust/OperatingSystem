// Jonghyeok (Burt) Kim 
// ECE 357 Probset 2 P3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fnmatch.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <errno.h>
#include <sys/sysmacros.h>

// References: 
// 1. https://stackoverflow.com/questions/10323060/printing-file-permissions-like-ls-l-using-stat2-in-c
// 2. https://codeforwin.org/c-programming/c-program-to-list-all-files-in-a-directory-recursively
// 3. https://www.geeksforgeeks.org/cpp/strftime-function-in-c/
// 4. https://stackoverflow.com/questions/1551597/using-strftime-in-c-how-can-i-format-time-exactly-like-a-unix-timestamp
// 5. https://stackoverflow.com/questions/31736476/majorstat-st-rdev-and-minorstat-st-rdev-on-non-device-files
// 6. https://stackoverflow.com/questions/4068136/regarding-checking-for-file-or-directory
// 7. https://stackoverflow.com/questions/31768830/getting-full-file-path-during-recursive-file-walk-c
// 8. https://stackoverflow.com/questions/43173142/getpwuid-and-getgrgid-causes-segfault-when-user-does-not-exist-for-given-uid

// Default values
int l_flag = 0;
int x_flag = 0;
char *pattern = NULL;
dev_t start_device;

char filetypeletter(int mode) {
    switch (mode & S_IFMT) {
        case S_IFREG: return '-';
        case S_IFDIR: return 'd';
        case S_IFCHR: return 'c';
        case S_IFBLK: return 'b';
        case S_IFLNK: return 'l';
        case S_IFSOCK: return 's';
        case S_IFIFO: return 'p';
        default: return '?';
    }
}

void get_permissions(mode_t mode, char *bit) {
    static const char *rwx[] = {"---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx"};
    
    bit[0] = filetypeletter(mode);
    strcpy(&bit[1], rwx[(mode >> 6) & 7]); // user permission
    strcpy(&bit[4], rwx[(mode >> 3) & 7]); // group permission
    strcpy(&bit[7], rwx[mode & 7]); // other permission
    
    // s = setuid and user can execute
    // S = setuid but user can't execute 
    if (mode & S_ISUID)
        bit[3] = (mode & S_IXUSR) ? 's' : 'S';

    // s = setgid and group can execute
    // S = setgid but group can't execute
    if (mode & S_ISGID)
        bit[6] = (mode & S_IXGRP) ? 's' : 'S';

    // t = sticky and others can execute
    // T = sticky but others can't execute
    if (mode & S_ISVTX)
        bit[9] = (mode & S_IXOTH) ? 't' : 'T';
    
    bit[10] = '\0'; // null-termination
}

void print_file(char *path, struct stat *st) {
    if (!l_flag) {
        printf("%s\n", path);
        return;
    }
    
    // The inode number and the space consumption in blocks
    // st_blocks is in 512-byte blocks, so this converts it to 1K blocks(rounded up).
    printf("%7lu %4ld ", st->st_ino, (st->st_blocks * 512 + 1023) / 1024);

    // Permissions
    char bit[11];
    get_permissions(st->st_mode, bit); // st_mode =  file type + permissions
    printf("%s %3lu ", bit, st->st_nlink);
    
    // Owner and group owner
    struct passwd *pw = getpwuid(st->st_uid);
    struct group *gr = getgrgid(st->st_gid);
    if (pw)
        printf("%-8s ", pw->pw_name); // User name
    else
        printf("%-8d ", st->st_uid); // If NULL, print uid number
    if (gr)
        printf("%-8s ", gr->gr_name); // Group name
    else
        printf("%-8d ", st->st_gid); // If NULL, print gid number
    
    // Print size or st_rdev field - represetned as major/minor numbers
    if (S_ISCHR(st->st_mode) || S_ISBLK(st->st_mode)) {
        int maj = major(st->st_rdev);
        int min = minor(st->st_rdev);
        printf("%3d, %4d ", maj, min);
    } else {
        printf("%8ld ", st->st_size);
    }
    
    // mtime
    char timebuf[20];
    struct tm *tm = localtime(&st->st_mtime);
    time_t now = time(NULL);
    time_t six_months_ago = now - (6 * 30 * 24 * 60 * 60); // roughly 6 months

    if (st->st_mtime < six_months_ago) {
        // If older than 6 months, show year
        strftime(timebuf, sizeof(timebuf), "%b %e  %Y", tm); 
    } else {
        // Otherwise, show hour and minute
        strftime(timebuf, sizeof(timebuf), "%b %e %H:%M", tm); 
    }
    printf("%s ", timebuf); 
    
    // full pathname
    printf("%s", path);
    
    // "-> target" if symlink
    if (S_ISLNK(st->st_mode)) {
        char linkbuf[1024];
        int n = readlink(path, linkbuf, sizeof(linkbuf) - 1);
        if (n > 0) {
            linkbuf[n] = '\0';
            printf(" -> %s", linkbuf);
        }
    }
    printf("\n");
}

void recurse(char *path) {
    struct stat st;
    
    // Load path info to st. If error, return.
    if (lstat(path, &st) < 0) {
        fprintf(stderr, "simplefind: cannot stat '%s': %s\n", path, strerror(errno));
        return;
    }
    
    // Check x_flag: If x_flag is set and device is different, return.
    if (x_flag && st.st_dev != start_device) {
        fprintf(stderr, "simplefind: not crossing mount point at '%s'\n", path);
        return;
    }
    
    // For -n case: Check if directory name matches pattern
    char *basename = strrchr(path, '/');
    if (!basename)
        basename = path;
    else
        basename++;
    
    if (!pattern || fnmatch(pattern, basename, 0) == 0)
        print_file(path, &st);
    
    // If not a directory, return. No error code because it is normal behavior.
    if (!S_ISDIR(st.st_mode))
        return;
    
    DIR *dir = opendir(path);
    // No error code because it is normal behavior.
    if (!dir)
        return;
    
    struct dirent *dp;
    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;
        
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, dp->d_name);
        
        // Load path info to st. If error, return.
        if (lstat(fullpath, &st) < 0) {
            fprintf(stderr, "simplefind: cannot stat '%s': %s\n", 
                    fullpath, strerror(errno));
            continue;
        }
        
        // Check x_flag: If x_flag is set and device is different, return.
        if (x_flag && st.st_dev != start_device) {
            fprintf(stderr, "simplefind: not crossing mount point at '%s'\n", 
                    fullpath);
            continue;
        }
        
        // Check if filename matches pattern
        if (!pattern || fnmatch(pattern, dp->d_name, 0) == 0)
            print_file(fullpath, &st);
        
        // Recurse if it's a directory
        if (S_ISDIR(st.st_mode))
            recurse(fullpath);
    }
    
    closedir(dir);
}

int main(int argc, char *argv[]) {
    char *start_path = ".";
    
    // This parses command line arguments
    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0) {
            l_flag = 1;
        } else if (strcmp(argv[i], "-x") == 0) {
            x_flag = 1;
        } else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            pattern = argv[++i];
        } else {
            start_path = argv[i];
        }
    }
    
    // x flag high = : Don't recurse into parts of the filesystem which are on a 
    //                 different mounted volume from starting_path.
    if (x_flag) {
        struct stat st;
        if (stat(start_path, &st) < 0) {
            fprintf(stderr, "simplefind: cannot stat '%s': %s\n", start_path, strerror(errno));
            exit(1);
        }
        start_device = st.st_dev;
    }
    
    // recursion starts here
    recurse(start_path);
    
    return 0;
}