#!/usr/bin/env python3
import sys
import os

def count_lines():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} file1 [file2 ...]")
        sys.exit(1)

    total_lines = 0

    for file in sys.argv[1:]:
        if os.path.isfile(file):
            try:
                with open(file, 'r') as f:
                    lines_in_file = sum(1 for _ in f)
                total_lines += lines_in_file
            except OSError as e:
                print(f"Warning: Could not read '{file}': {e}")
        else:
            print(f"Warning: '{file}' is not a valid file or is not readable.")

    if len(sys.argv) == 2:
        print(f"Total lines: {total_lines}")
    else:
        print(f"Total lines across all files: {total_lines}")

count_lines()
