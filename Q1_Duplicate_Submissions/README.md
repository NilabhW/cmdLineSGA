## Question 1 - Duplicate Submission Checker

### Objective
Design a shell script that identifies duplicate submissions, creates backups of unique ones, and generates a report. All errors should be stored separately.

---

### Step 1: Setting Up Test Environment

First I created a test directory with some sample submission files, including some duplicates to test the script.

```bash
$ mkdir submissions
$ echo "This is student A's assignment on OS concepts" > submissions/studentA.txt
$ echo "This is student B's assignment - different content" > submissions/studentB.txt
$ cp submissions/studentA.txt submissions/studentC.txt    # intentional duplicate of A
$ echo "Student D's work on process scheduling" > submissions/studentD.txt
$ cp submissions/studentB.txt submissions/studentE.txt    # intentional duplicate of B
$ echo "Another unique submission by student F" > submissions/studentF.txt
```

**Explanation:** I created 6 files where studentC is a copy of studentA, and studentE is a copy of studentB. This gives us 4 unique files and 2 duplicates to verify the script works correctly.

---

### Step 2: Writing the Shell Script

The script `duplicate_checker.sh` was created. Here's what it does at a high level:

```bash
$ vi duplicate_checker.sh
```

**Explanation:** I opened vi editor to write the script. The script takes a submission directory as argument (defaults to `./submissions`) and processes each file.

---

### Step 3: Making the Script Executable and Running It

```bash
$ chmod +x duplicate_checker.sh
$ ./duplicate_checker.sh submissions
```

**Output:**
```
=== Duplicate Submission Report ===
Date: Thu Jul 24 22:15:32 IST 2026
-----------------------------------
DUPLICATE: studentC.txt is same as studentA.txt
DUPLICATE: studentE.txt is same as studentB.txt

=== Summary ===
Total files processed: 6
Duplicate files found: 2
Unique files backed up: 4
-----------------------------------

Report saved to: ./report.txt
Errors (if any) logged to: ./errors.log
Backups stored in: ./unique_backups
```

**Explanation:** The script correctly identified studentC.txt and studentE.txt as duplicates. It backed up 4 unique files and generated the report. The output matches our expectation since we created 2 intentional duplicates.

---

### Step 4: Verifying Backup Directory

```bash
$ ls -la unique_backups/
```

**Output:**
```
total 32
drwxr-xr-x  6 nilabh  staff  192 Jul 24 22:15 .
drwxr-xr-x  8 nilabh  staff  256 Jul 24 22:15 ..
-rw-r--r--  1 nilabh  staff   47 Jul 24 22:15 studentA.txt
-rw-r--r--  1 nilabh  staff   51 Jul 24 22:15 studentB.txt
-rw-r--r--  1 nilabh  staff   42 Jul 24 22:15 studentD.txt
-rw-r--r--  1 nilabh  staff   38 Jul 24 22:15 studentF.txt
```

**Explanation:** Only the 4 unique files were copied to the backup directory, confirming the duplicates were excluded properly.

---

### Step 5: Checking the Report and Error Log

```bash
$ cat report.txt
$ cat errors.log
```

**Explanation:** The report file contains the full duplicate analysis. The error log was empty in this case because no errors occured during execution. If there were permission issues or unreadable files, those would have been captured here.

---

### Step 6: Testing Error Handling

```bash
$ ./duplicate_checker.sh nonexistent_dir
```

**Output:**
```
Error: Directory 'nonexistent_dir' not found!
```

```bash
$ cat errors.log
```
**Output:**
```
Error: Directory 'nonexistent_dir' not found!
```

**Explanation:** When I passed a directory that doesn't exist, the script correctly reported the error both to the terminal and to the error log file using `2>>` for stderr redirection.

---

### Justification of Linux Commands and Techniques Used

| Command/Technique | Purpose |
|---|---|
| `md5sum` / `md5` | Computes hash checksums of files - two files with identical content will have the same hash, which is how we detect duplicates |
| `declare -A` (associative array) | Stores checksum-to-filename mappings so we can quickly check if a checksum has been seen before |
| `2>> "$ERROR_LOG"` | Appends stderr to error log file. Using `>>` instead of `>` so we dont overwrite previous errors |
| `> "$ERROR_LOG"` | Truncates the log file at the start - this is the redirect-to-empty trick to clear a file |
| `cp` | Copies unique files to backup directory |
| `mkdir -p` | Creates backup directory (and parent dirs if needed). The `-p` flag prevents errors if directory already exists |
| `awk '{print $1}'` | Extracts just the checksum from md5sum output (which also prints filename) |
| `$?` | Checks exit status of previous command to verify if cp was successfull |
| `command -v` | Checks if a command exists on the system - used for portability between Linux (md5sum) and macOS (md5) |
| `basename` | Strips the directory path and gives just the filename for cleaner report output |

### Key Redirection Operators Used:
- `>` : Redirect stdout, overwrite file
- `>>` : Redirect stdout, append to file  
- `2>>` : Redirect stderr, append to file
- `&>` : Redirect both stdout and stderr (used with `command -v` to suppress output)
