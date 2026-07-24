## Question 3 - Secure File Processing Utility Using System Calls

### Objective
Build a program using Linux system calls (not standard library functions like fopen/fwrite) to create a file, write employee records, update specific records without rewriting the whole file, and retrieve records from any position.

---

### Step 1: Writing the Program

```bash
$ vi file_processor.c
```

**Explanation:** I created the C source file. The key design decision was to use a fixed-size `struct Employee` for all records. This is important because with fixed-size records, we can calculate the exact byte offset of any record using `record_number * sizeof(struct Employee)`, which makes random access possible with lseek().

---

### Step 2: Compiling

```bash
$ gcc -o file_processor file_processor.c -Wall
```

**Output:**
```
(no output - compiled successfully)
```

**Explanation:** No errors or warnings. The program compiled cleanly. We're using only system calls like `open()`, `read()`, `write()`, `lseek()`, and `close()` instead of the standard C library file functions.

---

### Step 3: Running the Program

```bash
$ ./file_processor
```

**Output:**
```
=== Secure File Processing Utility ===
Using Linux system calls (no stdio file operations)

--- Step 1: Creating file and writing records ---
File 'employee_records.dat' created successfully.

Written record: ID=101, Name=Rahul Sharma (92 bytes)
Written record: ID=102, Name=Priya Patel (92 bytes)
Written record: ID=103, Name=Amit Kumar (92 bytes)
Written record: ID=104, Name=Sneha Reddy (92 bytes)
Written record: ID=105, Name=Vikram Singh (92 bytes)

Total 5 records written.

--- Step 2: Reading all records ---
=== All Employee Records ===

ID: 101
Name: Rahul Sharma
Department: Engineering
Salary: 75000.00
---
ID: 102
Name: Priya Patel
Department: Marketing
Salary: 62000.00
---
ID: 103
Name: Amit Kumar
Department: Engineering
Salary: 80000.50
---
ID: 104
Name: Sneha Reddy
Department: HR
Salary: 58000.00
---
ID: 105
Name: Vikram Singh
Department: Finance
Salary: 71000.75
---
Total records read: 5

--- Step 3: Updating record #2 (Amit Kumar -> promotion) ---
Seeked to offset 184 (record #2)
Record #2 updated successfully. (92 bytes written)

--- Step 4: Retrieving updated record ---
=== Retrieved Record #2 ===
ID: 103
Name: Amit Kumar
Department: Engineering Lead
Salary: 95000.00
---
--- Step 5: Random access - retrieving first and last records ---
=== Retrieved Record #0 ===
ID: 101
Name: Rahul Sharma
Department: Engineering
Salary: 75000.00
---

=== Retrieved Record #4 ===
ID: 105
Name: Vikram Singh
Department: Finance
Salary: 71000.75
---

--- Final: All records after update ---
=== All Employee Records ===

ID: 101
Name: Rahul Sharma
Department: Engineering
Salary: 75000.00
---
ID: 102
Name: Priya Patel
Department: Marketing
Salary: 62000.00
---
ID: 103
Name: Amit Kumar
Department: Engineering Lead
Salary: 95000.00
---
ID: 104
Name: Sneha Reddy
Department: HR
Salary: 58000.00
---
ID: 105
Name: Vikram Singh
Department: Finance
Salary: 71000.75
---
Total records read: 5
```

**Explanation:** The program ran through all steps successfully. Record #2 (Amit Kumar) was updated from "Engineering" to "Engineering Lead" with a salary increase, and the update happened in-place without touching any other records. The offset of 184 bytes makes sense because record #2 is at position 2 × 92 = 184 bytes from the start of the file.

---

### Step 4: Verifying the Binary File

```bash
$ ls -la employee_records.dat
```

**Output:**
```
-rw-r--r--  1 nilabh  staff  460 Jul 24 22:20 employee_records.dat
```

**Explanation:** File size is 460 bytes which is exactly 5 records × 92 bytes each = 460. This confirms all records were written correctly with the fixed size structure.

```bash
$ hexdump -C employee_records.dat | head -20
```

**Explanation:** I used hexdump to peek at the raw binary content. You can see the text strings (names, departments) stored within the fixed-size blocks. The null bytes between strings are the padding in our fixed-size char arrays.

---

### How Each System Call Contributes

#### open()
```c
fd = open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
```
- Opens or creates the file and returns a file descriptor (an integer)
- `O_WRONLY` = write only mode, `O_CREAT` = create if it doesn't exist, `O_TRUNC` = clear file if it exists
- `0644` sets permissions (owner rw, group r, others r)
- Unlike fopen(), open() is a direct system call that gives us low-level control over file access flags

#### write()
```c
ssize_t bytes = write(fd, &employees[i], sizeof(struct Employee));
```
- Writes raw bytes from memory directly to the file
- Returns the number of bytes actually written (important to check for errors)
- No buffering like fprintf() - data goes straight to the kernel buffer
- We write entire struct at once, which keeps records aligned at fixed sizes

#### lseek()
```c
off_t result = lseek(fd, offset, SEEK_SET);
```
- Moves the file position pointer to a specific byte offset
- `SEEK_SET` = offset from beginning of file, `SEEK_CUR` = from current position, `SEEK_END` = from end
- This is what enables random access - we can jump to any record without reading all previous records
- Combined with fixed-size records, offset = record_number × record_size

#### read()
```c
ssize_t bytes = read(fd, &emp, sizeof(struct Employee));
```
- Reads raw bytes from file into memory
- Returns number of bytes read (0 means EOF, -1 means error)
- We read exactly sizeof(struct Employee) bytes to fill one record
- Works with lseek() to read any specific record by position

#### close()
```c
close(fd);
```
- Releases the file descriptor back to the OS
- Important to call this to free up system resources
- Also ensures any pending data is flushed to disk
- Forgetting to close files can lead to file descriptor leaks, which is a common bug in server programs

### Why System Calls Instead of Standard Library?

Using system calls (open/read/write/lseek/close) instead of standard library functions (fopen/fread/fwrite/fseek/fclose) gives us:
1. **Direct kernel interaction** - no user-space buffering overhead
2. **Fine-grained control** over file flags and permissions
3. **Better for binary data** - no character translation issues
4. **More secure** - we control exactly what happens at each step
5. **Required for certain operations** like file locking, non-blocking I/O, etc.
