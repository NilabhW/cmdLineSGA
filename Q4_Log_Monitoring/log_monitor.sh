#!/bin/bash

# Log Monitoring Tool
# Continuously watches a log file, extracts ERROR messages, 
# and maintains a separate error report

LOG_FILE="${1:-/var/log/syslog}"   # default log file
ERROR_REPORT="error_report.txt"

echo "=== Log Monitoring Tool ==="
echo "Monitoring: $LOG_FILE"
echo "Error report: $ERROR_REPORT"
echo "Press Ctrl+C to stop"
echo "=========================="
echo ""

# Initialize the error report
echo "=== Error Report ===" > "$ERROR_REPORT"
echo "Started: $(date)" >> "$ERROR_REPORT"
echo "Monitoring: $LOG_FILE" >> "$ERROR_REPORT"
echo "---" >> "$ERROR_REPORT"

# Main monitoring pipeline
# tail -f  : follow the log file in real time, showing new lines as they appear
# grep     : filter for ERROR messages (case insensitive)
# tee -a   : write to error report AND pass through to stdout
# The 2>/dev/null suppresses any error from tail if file doesnt exist momentarily

tail -f "$LOG_FILE" 2>/dev/null | grep --line-buffered -i "ERROR" | tee -a "$ERROR_REPORT"
