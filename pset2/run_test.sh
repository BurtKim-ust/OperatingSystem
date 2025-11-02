#!/bin/bash
# Burt Kim ECE 357 Fall 2025

# Searches for and lists all files ending with .c
echo "Testing ./simplefind -n '*.c' testdir"
./simplefind -n '*.c' testdir
echo "Done"
echo ""

# Diff test
echo "Testing Diff"
# My simplefind's output gets stored in /tmp/f1
./simplefind -l -x -n '*.c' testdir >/tmp/f1
# Linux find's output gets stored in /tmp/f2
find testdir -xdev -name '*.c' -ls >/tmp/f2
# Compare them
diff /tmp/f1 /tmp/f2
echo "Done"
echo ""

# Additional tests
# major, minor test
# root owner/group test
echo "Additional Testings - Simple Find"
./simplefind -l /dev/null
./simplefind -l /bin/ls
echo ""
echo "Additional Testings - Linux"
find /dev/null -ls
find /bin/ls -ls
echo ""
echo "All tests done"