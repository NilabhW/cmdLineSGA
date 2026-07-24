#!/bin/bash

# Script to find duplicate assignment submissions, backup unique ones, and generate report
# Author: Nilabh
# Usage: ./duplicate_checker.sh <submissions_directory>

SUBMISSION_DIR="${1:-.submissions}"    # default to ./submissions if no arg given
BACKUP_DIR="./unique_backups"
REPORT_FILE="./report.txt"
ERROR_LOG="./errors.log"

# clear previous logs
> "$ERROR_LOG"
> "$REPORT_FILE"

# check if submission directory exsists
if [ ! -d "$SUBMISSION_DIR" ]; then
    echo "Error: Directory '$SUBMISSION_DIR' not found!" 2>> "$ERROR_LOG"
    echo "Error: Directory '$SUBMISSION_DIR' not found!"
    exit 1
fi

# create backup directory if it doesnt exist
mkdir -p "$BACKUP_DIR" 2>> "$ERROR_LOG"

total_files=0
duplicate_count=0
backed_up=0

# we use md5 checksums to detect duplicates
declare -A checksum_map

echo "=== Duplicate Submission Report ===" >> "$REPORT_FILE"
echo "Date: $(date)" >> "$REPORT_FILE"
echo "-----------------------------------" >> "$REPORT_FILE"

# loop through all files in the submissions directory
for file in "$SUBMISSION_DIR"/*; do
    # skip if its not a regular file
    if [ ! -f "$file" ]; then
        continue
    fi

    total_files=$((total_files + 1))

    # calculate md5 checksum of the file
    # using md5sum (linux) - on mac it would be md5
    if command -v md5sum &> /dev/null; then
        checksum=$(md5sum "$file" 2>> "$ERROR_LOG" | awk '{print $1}')
    else
        checksum=$(md5 -q "$file" 2>> "$ERROR_LOG")
    fi

    if [ -z "$checksum" ]; then
        echo "Warning: Could not compute checksum for $file" >> "$ERROR_LOG"
        continue
    fi

    # check if this checksum already exists (meaning its a duplicate)
    if [ -n "${checksum_map[$checksum]}" ]; then
        duplicate_count=$((duplicate_count + 1))
        echo "DUPLICATE: $(basename "$file") is same as ${checksum_map[$checksum]}" >> "$REPORT_FILE"
    else
        # not a duplicate, so store checksum and backup the file
        checksum_map[$checksum]=$(basename "$file")
        cp "$file" "$BACKUP_DIR/" 2>> "$ERROR_LOG"
        if [ $? -eq 0 ]; then
            backed_up=$((backed_up + 1))
        else
            echo "Error: Failed to backup $file" >> "$ERROR_LOG"
        fi
    fi
done

# write summary to report
echo "" >> "$REPORT_FILE"
echo "=== Summary ===" >> "$REPORT_FILE"
echo "Total files processed: $total_files" >> "$REPORT_FILE"
echo "Duplicate files found: $duplicate_count" >> "$REPORT_FILE"
echo "Unique files backed up: $backed_up" >> "$REPORT_FILE"
echo "-----------------------------------" >> "$REPORT_FILE"

# also print summary to terminal
cat "$REPORT_FILE"

echo ""
echo "Report saved to: $REPORT_FILE"
echo "Errors (if any) logged to: $ERROR_LOG"
echo "Backups stored in: $BACKUP_DIR"
