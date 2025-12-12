#!/bin/bash

# 1. Check if an argument was provided
if [ -z "$1" ]; then
    echo "Error: No file provided."
    echo "Usage: ./analyze.sh filename.cpp"
    exit 1
fi

SOURCE_FILE="$1"
BASENAME=$(basename "$SOURCE_FILE" .cpp)

# 2. Check if the file actually exists
if [ ! -f "$SOURCE_FILE" ]; then
    echo "Error: File '$SOURCE_FILE' not found."
    exit 1
fi

echo "--- Analyzing $SOURCE_FILE ---"

# 4. Generate the Binary LLVM Bitcode (.bc)
# We use -c for object file output (binary)
echo "Generating ${BASENAME}.bc (Binary)..."
clang++ -emit-llvm -c "$SOURCE_FILE" -o "${BASENAME}.bc"

if [ $? -ne 0 ]; then
    echo "Compilation failed during .bc generation."
    exit 1
fi

echo "--- Success! ---"
echo "Created:"
ls -lh "${BASENAME}.bc"

../build/lib/analyzer "${BASENAME}.bc"