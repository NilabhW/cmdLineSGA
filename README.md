# Linux System Programming - Graded Lab Assignment (Modules 5-10)

**Name:** Nilabh  
**Repository:** cmdLineSGA

---

## Contents

| Folder | Question | Topic |
|---|---|---|
| [Q1_Duplicate_Submissions](./Q1_Duplicate_Submissions/) | Question 1 | Shell script for duplicate file detection & backup |
| [Q2_Process_Management](./Q2_Process_Management/) | Question 2 | C program for process creation, monitoring & signal handling |
| [Q3_File_Processing](./Q3_File_Processing/) | Question 3 | File processing using Linux system calls |
| [Q4_Log_Monitoring](./Q4_Log_Monitoring/) | Question 4 | Log monitoring pipeline using pipes & redirection |
| [Q5_Vi_Recovery](./Q5_Vi_Recovery/) | Question 5 | Vi editor recovery mechanisms evaluation |

---

Each folder contains:
- Source code / scripts
- README.md with commands, outputs, and explanations
- Output files generated during execution
- Screenshots of command execution

---

## How to Run

### Q1 - Duplicate Checker
```bash
cd Q1_Duplicate_Submissions
chmod +x duplicate_checker.sh
mkdir submissions
# add some test files to submissions/
./duplicate_checker.sh submissions
```

### Q2 - Process Manager
```bash
cd Q2_Process_Management
gcc -o process_manager process_manager.c -Wall
./process_manager
```

### Q3 - File Processor
```bash
cd Q3_File_Processing
gcc -o file_processor file_processor.c -Wall
./file_processor
```

### Q4 - Log Monitor
```bash
cd Q4_Log_Monitoring
chmod +x log_monitor.sh generate_logs.sh
# Terminal 1:
./log_monitor.sh test_server.log
# Terminal 2:
./generate_logs.sh
```

### Q5 - Vi Recovery
Refer to the README.md in Q5 folder for the detailed evaluation and commands.
