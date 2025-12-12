#!/bin/bash

ANALYZER="./build/lib/analyzer"
SEARCH_DIR="tests"
OUTPUT_DIR="./new_output_no_refine"

# Create output directory if it doesn't exist
mkdir -p "$OUTPUT_DIR"

if [ ! -f "$ANALYZER" ]; then
    echo "Error: Analyzer not found at $ANALYZER"
    exit 1
fi

find "$SEARCH_DIR" -type f \( -name "*.bc" -o -name "*.o" \) | while read -r file; do
    
    # Extract the filename without the path
    base_name=$(basename "$file")
    # Replace extension with .output
    output_file="$OUTPUT_DIR/${base_name%.*}.output"
    OUTPUT_FILE="${file}.output"
    
    echo "Processing: $file"
    echo "Saving to:  $output_file"

    "$ANALYZER" "$file" &> "$output_file"
    
done

echo "Done."
