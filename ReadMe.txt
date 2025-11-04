# OS Pset4 2025
# Jonghyeok Kim 

# Submitting documents: 
- wordgen.c
- wordsearch.c
- pager.c
- launcher.c
- ReadMe.txt

# Setup to run a test
gcc -o wordgen wordgen.c
gcc -o wordsearch wordsearch.c
gcc -o pager pager.c
gcc -o launcher launcher.c
To run launcher, there needs to be a txt file named words.txt existing in 
the same directory.

## Example test
# Infinite loop case (keep generating words)
- ./launcher 0
- ./launcher
root@MSI:/mnt/c/Github/OperatingSystem/pset4# ./launcher
Accepted 432 words, rejected 5
BOX
 --- more matching words ---
---Press RETURN for more---
q
*** Pager terminated by Q command ***
Child 203555 exited with 0
Matched 5120 words
Child 203553 exited with 13
Child 203554 exited with 0


# Non-infinite loop version
root@MSI:/mnt/c/Github/OperatingSystem/pset4# ./launcher 1000
Finished generating 1000 candidate words
Child 203549 exited with 0
Accepted 432 words, rejected 5
Matched 4 words
BOY
HOE
HIT
RID
Child 203551 exited with 0
Child 203550 exited with 0
