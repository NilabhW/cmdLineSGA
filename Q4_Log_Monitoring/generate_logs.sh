#!/bin/bash

# Helper script to generate fake log entries for testing
# Run this in a separate terminal while log_monitor.sh is running

LOG_FILE="test_server.log"

echo "Generating log entries to $LOG_FILE..."
echo "This simulates a web server writing logs"

# write some normal and error entries
echo "[$(date '+%Y-%m-%d %H:%M:%S')] INFO: Server started on port 8080" >> "$LOG_FILE"
sleep 1
echo "[$(date '+%Y-%m-%d %H:%M:%S')] INFO: Connection from 192.168.1.10" >> "$LOG_FILE"
sleep 1
echo "[$(date '+%Y-%m-%d %H:%M:%S')] ERROR: Database connection timeout after 30s" >> "$LOG_FILE"
sleep 1
echo "[$(date '+%Y-%m-%d %H:%M:%S')] INFO: Request GET /index.html - 200 OK" >> "$LOG_FILE"
sleep 1
echo "[$(date '+%Y-%m-%d %H:%M:%S')] WARNING: High memory usage detected (85%)" >> "$LOG_FILE"
sleep 1
echo "[$(date '+%Y-%m-%d %H:%M:%S')] ERROR: Failed to write to /tmp/cache - Permission denied" >> "$LOG_FILE"
sleep 1
echo "[$(date '+%Y-%m-%d %H:%M:%S')] INFO: Request POST /api/data - 201 Created" >> "$LOG_FILE"
sleep 1
echo "[$(date '+%Y-%m-%d %H:%M:%S')] ERROR: SSL certificate expired for domain api.example.com" >> "$LOG_FILE"
sleep 1
echo "[$(date '+%Y-%m-%d %H:%M:%S')] INFO: Connection closed from 192.168.1.10" >> "$LOG_FILE"
sleep 1
echo "[$(date '+%Y-%m-%d %H:%M:%S')] INFO: Backup completed successfully" >> "$LOG_FILE"

echo "Done generating log entries."
