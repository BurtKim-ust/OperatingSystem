## Jonghyeok (Burt) Kim ECE 357 Probset 2
# Intro
Submitting files:
- simplefind.c: It implements a simplified version of the Unix find command
- test_env.sh: It runs the program with different flag combinations, compares 
               the test output against Linux find command, and runs additional
               testings for special files (owner, group owner = root).
- run_test.sh: It runs the differ test, 
- ReadMe.txt: It contains instructions for running the program, test results,
              and a short write-up.


## How to use: run uncommented lines in order

gcc -o simplefind simplefind.c

# Make all scripts executable and make a test environment
chmod +x test_env.sh
./test_env.sh

# Run test: comparison between simplefind and linux find commands
./run_test.sh


## Output
[jonghyeok.kim@hakvmclass2023 ps2p3]$ gcc -o simplefind simplefind.c
[jonghyeok.kim@hakvmclass2023 ps2p3]$ chmod +x test_env.sh
[jonghyeok.kim@hakvmclass2023 ps2p3]$ ./test_env.sh
Test directory created
total 16
drwxr-xr-x 3 jonghyeok.kim users 4096 Oct  3 09:44 .
drwx------ 3 jonghyeok.kim users 4096 Oct  3 09:44 ..
drwxr-xr-x 2 jonghyeok.kim users 4096 Oct  3 09:44 d2
-rw-r--r-- 1 jonghyeok.kim users    5 Oct  3 09:44 f1.c
lrwxrwxrwx 1 jonghyeok.kim users    4 Oct  3 09:44 f5.c -> f1.c
-rw-r--r-- 1 jonghyeok.kim users    0 Oct  3 09:44 .hidden
-rw-r--r-- 1 jonghyeok.kim users    0 Oct  3 09:44 not_c_file.txt
lrwxrwxrwx 1 jonghyeok.kim users   17 Oct  3 09:44 testlink -> ../../testlinktgt
[jonghyeok.kim@hakvmclass2023 ps2p3]$ ./run_test.sh
Testing ./simplefind -n '*.c' testdir
testdir/d2/f2.c
testdir/d2/f4.c
testdir/d2/.f3.c
testdir/f1.c
testdir/f5.c
Done

Testing Diff
1,5c1,5
< 3276842   52 -rw-r--r--   1 jonghyeok.kim users       50000 Oct  3 09:44 testdir/d2/f2.c
< 3276843    4 -rw-r--r--   2 jonghyeok.kim users           6 Oct  3 09:44 testdir/d2/f4.c
< 3276843    4 -rw-r--r--   2 jonghyeok.kim users           6 Oct  3 09:44 testdir/d2/.f3.c
< 3276841    4 -rw-r--r--   1 jonghyeok.kim users           5 Oct  3 09:44 testdir/f1.c
< 3276845    0 lrwxrwxrwx   1 jonghyeok.kim users           4 Oct  3 09:44 testdir/f5.c -> f1.c
---
>   3276842     52 -rw-r--r--   1  jonghyeok.kim users       50000 Oct  3 09:44 testdir/d2/f2.c
>   3276843      4 -rw-r--r--   2  jonghyeok.kim users           6 Oct  3 09:44 testdir/d2/f4.c
>   3276843      4 -rw-r--r--   2  jonghyeok.kim users           6 Oct  3 09:44 testdir/d2/.f3.c
>   3276841      4 -rw-r--r--   1  jonghyeok.kim users           5 Oct  3 09:44 testdir/f1.c
>   3276845      0 lrwxrwxrwx   1  jonghyeok.kim users           4 Oct  3 09:44 testdir/f5.c -> f1.c
Done

Additional Testings - Simple Find
      4    0 crw-rw-rw-   1 root     root       1,    3 Sep 27 22:19 /dev/null
1578332  140 -rwxr-xr-x   1 root     root       143328 Apr  3  2023 /bin/ls

Additional Testings - Linux
        4      0 crw-rw-rw-   1  root     root       1,   3 Sep 27 22:19 /dev/null
  1578332    140 -rwxr-xr-x   1  root     root       143328 Apr  3  2023 /bin/ls

All tests done

# Write-up
At the very beginning, I tested the ./run_test.sh command to show what files 
and directories exist in the testdir directory.

Then, I ran three test sections that display whether the output of 
simplefind.c and the output of the Linux find function differ or not. 
I intended to include tests for symlinks, hard links, hidden files, and 
regular files. The test ran successfully as everything except spacing matches, 
which is why the diff function returned output showing every line differs.

I ran additional tests to check whether the same results occur when examining 
files that have root as the owner and group owner.

Overall, everything ran successfully.