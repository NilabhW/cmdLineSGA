#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#define MAX_CHILDREN 5
#define TIMEOUT_SEC 10    // kill child if it runs longer than this

pid_t child_pids[MAX_CHILDREN];
int child_count = 0;

// signal handler to reap zombie processes
void sigchld_handler(int sig) {
    int saved_errno = errno;
    pid_t pid;
    int status;
    
    // use WNOHANG to reap all terminated children without blocking
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            printf("[Parent] Child %d exited normally with status %d\n", pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("[Parent] Child %d killed by signal %d\n", pid, WTERMSIG(status));
        }
        
        // remove from our tracking array
        for (int i = 0; i < child_count; i++) {
            if (child_pids[i] == pid) {
                child_pids[i] = 0;  // mark as reaped
                break;
            }
        }
    }
    errno = saved_errno;
}

// signal handler for SIGALRM - used to check for unresponsive children
void sigalrm_handler(int sig) {
    printf("\n[Monitor] Checking for unresponsive processes...\n");
}

// simulate different child behaviors
void child_work(int child_num) {
    srand(time(NULL) + child_num);
    
    switch (child_num) {
        case 0:
            // normal child - finishes quickly
            printf("[Child %d (PID %d)] Processing request... done.\n", child_num, getpid());
            sleep(2);
            exit(0);
            break;
        case 1:
            // slightly slower child
            printf("[Child %d (PID %d)] Handling complex request...\n", child_num, getpid());
            sleep(4);
            printf("[Child %d (PID %d)] Finished.\n", child_num, getpid());
            exit(0);
            break;
        case 2:
            // simulates an unresponsive/hung process
            printf("[Child %d (PID %d)] Started... (will hang)\n", child_num, getpid());
            while(1) {
                sleep(1);  // infinite loop - simulates unresponsive process
            }
            break;
        case 3:
            // child that crashes
            printf("[Child %d (PID %d)] Processing... encountered error!\n", child_num, getpid());
            sleep(1);
            exit(1);  // exit with error code
            break;
        case 4:
            // another normal child
            printf("[Child %d (PID %d)] Quick task completed.\n", child_num, getpid());
            sleep(3);
            exit(0);
            break;
    }
}

int main() {
    printf("=== Web Server Process Manager ===\n");
    printf("Parent PID: %d\n\n", getpid());
    
    // set up SIGCHLD handler to prevent zombies
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction SIGCHLD");
        exit(1);
    }
    
    // set up SIGALRM handler
    struct sigaction sa_alarm;
    sa_alarm.sa_handler = sigalrm_handler;
    sigemptyset(&sa_alarm.sa_mask);
    sa_alarm.sa_flags = 0;
    sigaction(SIGALRM, &sa_alarm, NULL);
    
    // create child processes
    printf("Creating %d child processes...\n\n", MAX_CHILDREN);
    
    for (int i = 0; i < MAX_CHILDREN; i++) {
        pid_t pid = fork();
        
        if (pid < 0) {
            perror("fork failed");
            // kill existing children before exiting
            for (int j = 0; j < child_count; j++) {
                if (child_pids[j] > 0) {
                    kill(child_pids[j], SIGTERM);
                }
            }
            exit(1);
        }
        else if (pid == 0) {
            // child process
            child_work(i);
            exit(0);  // just in case
        }
        else {
            // parent process
            child_pids[child_count++] = pid;
            printf("[Parent] Created child %d with PID %d\n", i, pid);
        }
    }
    
    printf("\n[Parent] All children created. Monitoring...\n\n");
    
    // monitor children - wait for timeout, then check for unresponsive ones
    sleep(TIMEOUT_SEC);
    
    printf("\n[Monitor] Timeout reached (%d seconds). Checking for hung processes...\n", TIMEOUT_SEC);
    
    // check if any children are still running (potentially unresponsive)
    for (int i = 0; i < child_count; i++) {
        if (child_pids[i] > 0) {
            // check if process is still alive
            if (kill(child_pids[i], 0) == 0) {
                printf("[Monitor] Child PID %d is still running - sending SIGTERM\n", child_pids[i]);
                kill(child_pids[i], SIGTERM);
                
                // give it a moment to terminate gracefully
                sleep(2);
                
                // if still alive, force kill with SIGKILL
                if (kill(child_pids[i], 0) == 0) {
                    printf("[Monitor] Child PID %d didn't respond to SIGTERM - sending SIGKILL\n", child_pids[i]);
                    kill(child_pids[i], SIGKILL);
                }
            }
        }
    }
    
    // final wait to reap any remaining zombies
    sleep(2);
    
    printf("\n[Parent] All child processes handled. Exiting.\n");
    
    return 0;
}
