#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define FILENAME "employee_records.dat"
#define MAX_NAME 50
#define MAX_DEPT 30

// fixed-size record structure for easy seeking
struct Employee {
    int id;
    char name[MAX_NAME];
    char department[MAX_DEPT];
    float salary;
};

// function to display a single record
void display_record(struct Employee *emp) {
    printf("ID: %d\n", emp->id);
    printf("Name: %s\n", emp->name);
    printf("Department: %s\n", emp->department);
    printf("Salary: %.2f\n", emp->salary);
    printf("---\n");
}

// create the file and write initial employee records
int create_and_write_records() {
    int fd;
    
    // open file - create if doesnt exist, truncate if it does
    // permissions: owner read/write, group read, others read (0644)
    fd = open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Error creating file");
        return -1;
    }
    
    printf("File '%s' created successfully.\n\n", FILENAME);
    
    // create some employee records
    struct Employee employees[] = {
        {101, "Rahul Sharma", "Engineering", 75000.00},
        {102, "Priya Patel", "Marketing", 62000.00},
        {103, "Amit Kumar", "Engineering", 80000.50},
        {104, "Sneha Reddy", "HR", 58000.00},
        {105, "Vikram Singh", "Finance", 71000.75}
    };
    
    int num_records = sizeof(employees) / sizeof(employees[0]);
    
    // write records to file
    for (int i = 0; i < num_records; i++) {
        ssize_t bytes_written = write(fd, &employees[i], sizeof(struct Employee));
        if (bytes_written == -1) {
            perror("Error writing record");
            close(fd);
            return -1;
        }
        printf("Written record: ID=%d, Name=%s (%zd bytes)\n", 
               employees[i].id, employees[i].name, bytes_written);
    }
    
    printf("\nTotal %d records written.\n", num_records);
    close(fd);
    return num_records;
}

// read and display all records from the file
int read_all_records() {
    int fd = open(FILENAME, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file for reading");
        return -1;
    }
    
    printf("\n=== All Employee Records ===\n\n");
    
    struct Employee emp;
    int count = 0;
    ssize_t bytes_read;
    
    while ((bytes_read = read(fd, &emp, sizeof(struct Employee))) == sizeof(struct Employee)) {
        display_record(&emp);
        count++;
    }
    
    if (bytes_read == -1) {
        perror("Error reading file");
    }
    
    printf("Total records read: %d\n", count);
    close(fd);
    return count;
}

// update a specific record by its position (0-indexed) without rewriting entire file
int update_record(int record_num, struct Employee *new_data) {
    int fd = open(FILENAME, O_WRONLY);
    if (fd == -1) {
        perror("Error opening file for update");
        return -1;
    }
    
    // calculate offset using lseek
    // each record is same size so we can jump directly to any record
    off_t offset = record_num * sizeof(struct Employee);
    off_t result = lseek(fd, offset, SEEK_SET);
    
    if (result == -1) {
        perror("Error seeking to record position");
        close(fd);
        return -1;
    }
    
    printf("Seeked to offset %lld (record #%d)\n", (long long)result, record_num);
    
    // write the updated record at the current position
    ssize_t bytes = write(fd, new_data, sizeof(struct Employee));
    if (bytes == -1) {
        perror("Error writing updated record");
        close(fd);
        return -1;
    }
    
    printf("Record #%d updated successfully. (%zd bytes written)\n", record_num, bytes);
    close(fd);
    return 0;
}

// retrieve a specific record by position
int retrieve_record(int record_num) {
    int fd = open(FILENAME, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file for retrieval");
        return -1;
    }
    
    // seek to the exact position of the requested record
    off_t offset = record_num * sizeof(struct Employee);
    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("lseek failed");
        close(fd);
        return -1;
    }
    
    struct Employee emp;
    ssize_t bytes = read(fd, &emp, sizeof(struct Employee));
    
    if (bytes < (ssize_t)sizeof(struct Employee)) {
        printf("Error: Record #%d does not exist or is incomplete.\n", record_num);
        close(fd);
        return -1;
    }
    
    printf("\n=== Retrieved Record #%d ===\n", record_num);
    display_record(&emp);
    
    close(fd);
    return 0;
}

int main() {
    printf("=== Secure File Processing Utility ===\n");
    printf("Using Linux system calls (no stdio file operations)\n\n");
    
    // Step 1: Create file and write records
    printf("--- Step 1: Creating file and writing records ---\n");
    int total = create_and_write_records();
    if (total < 0) {
        fprintf(stderr, "Failed to create records. Exiting.\n");
        return 1;
    }
    
    // Step 2: Read all records
    printf("\n--- Step 2: Reading all records ---");
    read_all_records();
    
    // Step 3: Update a specific record (update record #2 - Amit Kumar)
    printf("\n--- Step 3: Updating record #2 (Amit Kumar -> promotion) ---\n");
    struct Employee updated = {103, "Amit Kumar", "Engineering Lead", 95000.00};
    update_record(2, &updated);
    
    // Step 4: Retrieve updated record to verify
    printf("\n--- Step 4: Retrieving updated record ---");
    retrieve_record(2);
    
    // Step 5: Retrieve first and last record to show random access
    printf("--- Step 5: Random access - retrieving first and last records ---");
    retrieve_record(0);
    retrieve_record(4);
    
    // Show all records after update
    printf("\n--- Final: All records after update ---");
    read_all_records();
    
    return 0;
}
