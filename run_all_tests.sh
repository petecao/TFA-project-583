#!/bin/bash

ANALYZER="./build/lib/analyzer"
SEARCH_DIR="tests"

if [ ! -f "$ANALYZER" ]; then
    echo "Error: Analyzer not found at $ANALYZER"
    exit 1
fi


find "$SEARCH_DIR" -type f \( -name "*.bc" -o -name "*.o" \) | while read -r file; do
    
    OUTPUT_FILE="${file}.o"
    
    echo "Processing: $file"
    echo "Saving to:  $OUTPUT_FILE"

    "$ANALYZER" "$file" &> "$OUTPUT_FILE"
    
done

echo "Done."