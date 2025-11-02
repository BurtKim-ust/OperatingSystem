#!/bin/bash

rm -rf testdir

# Diff Test Setup
mkdir -p testdir/d2
echo "test" > testdir/f1.c
for i in {1..10000}; do echo "test" >> testdir/d2/f2.c; done # Larger file
echo "test3" > testdir/d2/.f3.c
ln testdir/d2/.f3.c testdir/d2/f4.c  # Hard link
ln -s ../../testlinktgt testdir/testlink  # Symlink
ln -s f1.c testdir/f5.c # Symlink to file
touch testdir/.hidden
touch testdir/not_c_file.txt  # Test pattern matching

echo "Test directory created"
ls -la testdir/