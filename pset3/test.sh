#!/bin/bash

# Jonghyeok Kim
# ECE 357 - Problem Set 3 - Q3
# test.sh
# useful link for bash scripting: https://stackoverflow.com/questions/2953081/how-can-i-write-a-heredoc-to-a-file-in-bash-script

echo "Compiling simple_sh.c..."
gcc -o simple_sh simple_sh.c

# Create test directory
rm -rf test_directory
mkdir test_directory

# Create segfault program for testing
cat > dumpcore.c << 'EOF'
int main() {
    *(int*)0 = 0; 
}
EOF
gcc -o dumpcore dumpcore.c 2>/dev/null # this hides compiler warnings 

echo -e "\n===Read from Std_in Test==="
echo "Commands:"
echo " cd test_directory"
echo " pwd"
echo " ls -l > ls.out"
echo " cat ls.out"
echo " ls non-existent-file"
echo " ../dumpcore"
echo -e "\nOutput:\n"

# Tried to copy the assignment example
cat << 'EOF' | ./simple_sh
cd test_directory
pwd
ls -l > ls.out
cat ls.out
ls non-existent-file
../dumpcore
exit
EOF

echo -e "\nRead from Std_in Test Exit Code: $?" # Exit code


# Read from File Test
echo -e "\n===Read from File Test==="
echo "Commands inside the Read_File_Test.sh:"
echo " pwd"
echo " ls nonexistent"
echo " exit 2"
echo -e "\nOutput:\n"

cat > Read_File_Test.sh << 'EOF'
pwd
ls nonexistent
exit 2
EOF

./simple_sh Read_File_Test.sh
echo "Read from File Test Exit Code: $?"