# ECE 357 Probset3 Q3
# Burt (Jonghyeok) Kim
# Submitting files: simpe_sh.c, test.sh, ReadMe.txt, provided test files(test1.sh, test2.sh)

# Command Inputs
i. Read from std_in  
ii. Read from file.

# How to run the code
chmod +x test.sh
./test.sh
rm -rf test_directory testfile*.out dumpcore Read_File_Test.sh simple_sh

gcc -o simple_sh simple_sh.c
./simple_sh test1.sh
echo $? # expected to be 17
ls -l testfile*.out # Both files should be 18 bytes

./simple_sh test2.sh
echo $?  # expected to be 1

# What the test does
Read from std_in: 
- Sends multiple commands (followed example tests in the guideline)
- Tests directory navigation, I/O redirection (>), command execution, 
  error handling, and signal detection

Read from file:
- Creates Read_File_Test.sh
- It contains pwd, a failing ls command, and exit with code 2

Exit codes are verified at the end of the tests.

## Test Logs from running the test.sh I made.
[jonghyeok.kim@hakvmclass2023 ps3p3]$ ./test.sh
Compiling simple_sh.c...

===Read from Std_in Test===
Commands:
 cd test_directory
 pwd
 ls -l > ls.out
 cat ls.out
 ls non-existent-file
 ../dumpcore

Output:

/home/jonghyeok.kim/hwsub/ps3p3/test_directory
Child process id: 83632 exited normally
Real: 0.003s User: 0.001s Sys: 0.001s
total 0
-rw-r--r-- 1 jonghyeok.kim users 0 Oct 17 21:37 ls.out
Child process id: 83633 exited normally
Real: 0.002s User: 0.000s Sys: 0.002s
ls: cannot access 'non-existent-file': No such file or directory
Child process id: 83634. Last exit status: 2
Real: 0.002s User: 0.001s Sys: 0.001s
Child process id: 83635. exit signal: 11 (Segmentation fault)
Real: 0.001s User: 0.001s Sys: 0.000s

Read from Std_in Test Exit Code: 139

===Read from File Test===
Commands inside the Read_File_Test.sh:
 pwd
 ls nonexistent
 exit 2
/home/jonghyeok.kim/hwsub/ps3p3
ls: cannot access 'nonexistent': No such file or directory
Child process id: 83638. Last exit status: 2
Real: 0.002s User: 0.001s Sys: 0.001s
Read from File Test Exit Code: 2

# Provided Tests: test1, test2
rhdtn@MSI ~/pset3/q3
$ gcc -o simple_sh simple_sh.c

rhdtn@MSI ~/pset3/q3
$ ./simple_sh test1.sh
Child process id: 2254 exited normally
Real: 0.110s User: 0.000s Sys: 0.000s
Child process id: 2255 exited normally
Real: 0.031s User: 0.000s Sys: 0.000s
total 174
-rwxr-x--- 1 rhdtn rhdtn  2847 Oct 17 22:15 ReadMe.txt
-rw-r--r-- 1 rhdtn rhdtn    26 Oct 17 21:19 Read_File_Test.sh
-rw-r--r-- 1 rhdtn rhdtn    34 Oct 17 21:19 dumpcore.c
-rwxr-xr-x 1 rhdtn rhdtn 67256 Oct 17 21:19 dumpcore.exe
drwxr-xr-x 1 rhdtn rhdtn     0 Oct 12 10:30 error
-rw-r--r-- 1 rhdtn rhdtn    57 Oct 12 00:00 error.txt
-rw-r--r-- 1 rhdtn rhdtn    61 Oct 11 23:31 out.txt
-rw-r--r-- 1 rhdtn rhdtn    52 Oct 12 10:09 output.txt
-rwxr-x--- 1 rhdtn rhdtn  9292 Oct 17 22:15 simple_sh.c
-rwxr-xr-x 1 rhdtn rhdtn 77032 Oct 17 22:16 simple_sh.exe
-rwxr-x--x 1 rhdtn rhdtn  1245 Oct 17 22:15 test.sh
-rwxr-x--- 1 rhdtn rhdtn   307 Oct 17 22:15 test1.sh
-rwxr-x--- 1 rhdtn rhdtn   636 Oct 17 22:15 test2.sh
-rwxr-x--- 1 rhdtn rhdtn    30 Oct 17 21:19 test3.sh
drwxr-xr-x 1 rhdtn rhdtn     0 Oct 17 21:19 test_directory
-rw-r--r-- 1 rhdtn rhdtn    18 Oct 17 22:16 testfile.out
-rw-r--r-- 1 rhdtn rhdtn    18 Oct 17 22:16 testfile2.out
Child process id: 2256 exited normally
Real: 0.027s User: 0.000s Sys: 0.030s

rhdtn@MSI ~/pset3/q3
$ echo $?
17

rhdtn@MSI ~/pset3/q3
$ ls -l testfile*.out
-rw-r--r-- 1 rhdtn rhdtn 18 Oct 17 22:16 testfile.out
-rw-r--r-- 1 rhdtn rhdtn 18 Oct 17 22:16 testfile2.out

rhdtn@MSI ~/pset3/q3
$ ./simple_sh test2.sh
/tmp
/home/rhdtn
Child process id: 2259 exited normally
Real: 0.025s User: 0.015s Sys: 0.000s
Error: Command 'lssssssss' not found: No such file or directory
Child process id: 2260. Last exit status: 127
Real: 0.012s User: 0.000s Sys: 0.000s
Child process id: 2261 exited normally
Real: 0.025s User: 0.000s Sys: 0.015s
Error: Cannot open file 'testfile.out' for redirection: Permission denied
Child process id: 2262. Last exit status: 1
Real: 0.010s User: 0.000s Sys: 0.000s
end of file read. Last exit status:  1

rhdtn@MSI ~/pset3/q3
$ echo $?
1

# I changed some of my code to make test1.sh work because it wasn't working. 
# Then, test2.sh fell into an infinite loop.
# Infinite loop:
/home/jonghyeok.kim
Error: Cannot open file 'testfile.out' for redirection: Permission denied
Child process id: 117686. Last exit status: 1
Real: 0.000s User: 0.000s Sys: 0.000s
Error: Command 'lssssssss' not found: No such file or directory
Child process id: 117687. Last exit status: 127
Real: 0.001s User: 0.001s Sys: 0.000s
Child process id: 117688 exited normally
Real: 0.002s User: 0.001s Sys: 0.001s
Error: Cannot open file 'testfile.out' for redirection: Permission denied
Child process id: 117689. Last exit status: 1
Real: 0.000s User: 0.000s Sys: 0.000s
Error: Command 'r' not found: No such file or directory
Child process id: 117690. Last exit status: 127
Real: 0.001s User: 0.001s Sys: 0.000s
/tmp
/home/jonghyeok.kim
Error: Cannot open file 'testfile.out' for redirection: Permission denied
Child process id: 117691. Last exit status: 1
Real: 0.000s User: 0.000s Sys: 0.000s
Error: Command 'lssssssss' not found: No such file or directory
Child process id: 117692. Last exit status: 127
Real: 0.001s User: 0.001s Sys: 0.000s
Child process id: 117693 exited normally
Real: 0.002s User: 0.002s Sys: 0.000s
Error: Cannot open file 'testfile.out' for redirection: Permission denied
Child process id: 117694. Last exit status: 1
Real: 0.000s User: 0.000s Sys: 0.000s
Error: Command 'r' not found: No such file or directory
Child process id: 117695. Last exit status: 127
Real: 0.001s User: 0.001s Sys: 0.000s


# Comment
I found that you provided the test1.sh and test2.sh files after creating and running the test.sh file.
I truly appreciate your feedback. 
Thank you, Professor Hakner. Have a nice day!