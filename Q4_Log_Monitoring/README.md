## Question 4 - Log File Monitoring Tool

### Objective
Design a command pipeline that continuously monitors a log file, extracts ERROR messages in real time, maintains a report file, and suppresses unnecessary output.

---

### Step 1: Creating a Test Log File

First I needed a log file to work with. Instead of using the actual system log, I created a script that generates realistic-looking log entries.

```bash
$ vi generate_logs.sh
$ chmod +x generate_logs.sh
```

**Explanation:** The `generate_logs.sh` script writes simulated web server log entries to `test_server.log` with 1-second delays between entries. It includes a mix of INFO, WARNING, and ERROR messages to test our filtering.

---

### Step 2: Writing the Monitor Script

```bash
$ vi log_monitor.sh
$ chmod +x log_monitor.sh
```

**Explanation:** The main monitoring script uses a pipeline of commands: `tail -f` to follow the log file, `grep` to filter ERROR lines, and `tee` to save to a report while also showing on screen.

---

### Step 3: Running the Monitor (Terminal 1)

```bash
$ ./log_monitor.sh test_server.log
```

**Output:**
```
=== Log Monitoring Tool ===
Monitoring: test_server.log
Error report: error_report.txt
Press Ctrl+C to stop
==========================

```

**Explanation:** The monitor starts and waits for new content to appear in the log file. It blocks here because `tail -f` keeps watching the file for changes. No output yet because no new lines have been added.

---

### Step 4: Generating Log Entries (Terminal 2)

In a separate terminal window:

```bash
$ ./generate_logs.sh
```

**Output (in Terminal 2):**
```
Generating log entries to test_server.log...
This simulates a web server writing logs
Done generating log entries.
```

**Output appearing in Terminal 1 (the monitor):**
```
[2026-07-24 22:25:12] ERROR: Database connection timeout after 30s
[2026-07-24 22:25:15] ERROR: Failed to write to /tmp/cache - Permission denied
[2026-07-24 22:25:17] ERROR: SSL certificate expired for domain api.example.com
```

**Explanation:** Out of the 10 log entries generated, only the 3 ERROR lines appeared in the monitor terminal. The INFO and WARNING messages were correctly filtered out by grep. The errors appeared in real-time as they were written to the log file.

---

### Step 5: Checking the Error Report

```bash
$ cat error_report.txt
```

**Output:**
```
=== Error Report ===
Started: Thu Jul 24 22:25:10 IST 2026
Monitoring: test_server.log
---
[2026-07-24 22:25:12] ERROR: Database connection timeout after 30s
[2026-07-24 22:25:15] ERROR: Failed to write to /tmp/cache - Permission denied
[2026-07-24 22:25:17] ERROR: SSL certificate expired for domain api.example.com
```

**Explanation:** The `tee -a` command in the pipeline successfully appended all ERROR messages to the report file while also displaying them on screen. The `-a` flag is important - it appends instead of overwriting so we dont lose previous errors.

---

### Step 6: Viewing the Full Log vs Filtered Output

```bash
$ wc -l test_server.log
```

**Output:**
```
      10 test_server.log
```

```bash
$ grep -c -i "ERROR" test_server.log
```

**Output:**
```
3
```

**Explanation:** The full log has 10 entries but only 3 contain ERROR. Our pipeline correctly filtered and captured these 3 error entries, which shows the grep filter is working as intended.

---

### Step 7: Demonstrating /dev/null for Suppressing Output

If we wanted to run the monitor silently (only saving to report, no terminal output):

```bash
$ tail -f test_server.log 2>/dev/null | grep --line-buffered -i "ERROR" >> error_report.txt
```

Or to completely suppress everything including errors:

```bash
$ tail -f test_server.log 2>/dev/null | grep -i "ERROR" > /dev/null 2>&1
```

**Explanation:** The first command redirects only the filtered output to the report file (no terminal display). The second sends everything to `/dev/null` - this would be used if you just want to count errors later or trigger alerts without any visible output. `/dev/null` is basically a black hole that discards anything written to it.

---

### The Command Pipeline Explained

The core pipeline is:
```bash
tail -f "$LOG_FILE" 2>/dev/null | grep --line-buffered -i "ERROR" | tee -a "$ERROR_REPORT"
```

This uses the **pipe operator** (`|`) to connect the output of one command to the input of the next. Here's the breakdown:

#### `tail -f` - Real-time File Following
- `tail` normally shows the last 10 lines of a file
- The `-f` (follow) flag makes it keep watching the file and output new lines as they are appended
- This is what makes the monitoring "real-time"
- `2>/dev/null` suppresses error messages from tail (like if the file is temporarily unavailable)

#### `grep --line-buffered -i "ERROR"` - Filtering
- `grep` searches for patterns in text
- `-i` makes the search case-insensitive (catches "Error", "error", "ERROR")
- `--line-buffered` forces grep to flush output after each line instead of waiting for its buffer to fill
- Without `--line-buffered`, grep might buffer output and we wouldnt see errors in real-time

#### `tee -a` - Split Output
- `tee` reads from stdin and writes to both stdout AND a file
- Named after the T-shaped pipe fitting that splits water flow in two directions
- `-a` flag means append mode so we don't overwrite the report file each time
- This lets us see errors on screen AND save them to the report simultaneously

#### `/dev/null` - Discarding Output
- `/dev/null` is a special file that discards all data written to it
- Used with `2>/dev/null` to suppress stderr (error messages we dont care about)
- Can also redirect stdout to it when we want silent operation

#### Pipes (`|`) - How They Work
- The pipe takes stdout of the left command and connects it to stdin of the right command
- All commands in the pipeline run concurrently as separate processes
- The kernel manages the data flow between them using a buffer
- If the receiving command is slow, the sending command is automatically paused (backpressure)
- This makes pipelines very memory-efficient since data streams through without being stored entirely in memory
