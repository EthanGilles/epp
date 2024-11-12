#!/bin/bash

if [ "$#" -eq 0 ]; then
  echo "Usage: $0 file1 [file2 ...]"
  exit 1
fi

total_lines=0

for file in "$@"; do
  if [ -f "$file" ]; then
    lines_in_file=$(wc -l < "$file")
    total_lines=$((total_lines + lines_in_file))
  else
    echo "Warning: '$file' is not a valid file or is not readable."
  fi
done

if [ "$#" -eq 1 ]; then
  echo "Total lines: $total_lines"
else
  echo "Total lines across all files: $total_lines"
fi
