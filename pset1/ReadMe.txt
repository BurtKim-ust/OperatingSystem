ECE 357 Pset 1 - Fall 2025
Jonghyeok (Burt) Kim
Submitting files: large_test.txt, simple_test.sh, mylib.c, mylib.h, tabstop.c, ReadMe.txt

## Pset 1 - 3 
# A simple test - myfopen() write and read, myfputc(), myfgetc(), myfdopen(), and myfclose()
gcc -o app app.c mylib.c
./app

## Pset 1 - 4,5
# You can skip to J1 since large_test.txt already exists
# If large_test.txt exists,
rm large_test.txt

# Generate large_test.txt and then count the filesize
for i in $(seq 1 70000); do echo -e "Line$i\ttabs\ttabs\ttabs" >> large_test.txt; done
ls -lh large_test.txt

# J1: Compile tabstop (problem 5 applied)
gcc -o tabstop tabstop.c mylib.c

# Test all 4 usage patterns
./tabstop -o output.txt large_test.txt    # outfile infile
./tabstop -o output2.txt < large_test.txt # outfile
./tabstop large_test.txt | head -5        # infile
./tabstop < large_test.txt | head -5
 
# Verify \t -> 4 spaces
cat -T large_test.txt | head -1 # Show tabs as ^I
cat -T output.txt | head -1 # Show spaces instead of ^I's

# Make a table for problem 5
# Run the performance test
chmod +x simple_test.sh
./simple_test.sh # you should be able to see the real time taken to do tasks with different buffer sizes.