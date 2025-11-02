#!/bin/bash

# Create a large test file > 1MB
echo "Creating large test file with tabs..."

# Remove existing file if it exists
rm -f large_test.txt

# Content
for i in {1..30000}; do
    echo -e "Line$i\tThis\tis\ta\ttest\tline\twith\tmany\ttabs\tfor\tperformance\ttesting" >> large_test.txt
done
echo "File content created successfully!"

# Check the file size > 1MB
echo "File size:"
ls -lh large_test.txt

# Count total characters, lines, and tabs
echo "Total characters: $(wc -c < large_test.txt)"
echo "Total lines: $(wc -l < large_test.txt)"
echo "Total tabs: $(grep -o $'\t' large_test.txt | wc -l)"