#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

// Function to be executed by each child process
void childProcess() {
    int iterations = random() % 30 + 1;  // Random number of iterations (1 to 30)
    int i;  // Declare the variable outside of the for loop

    for (i = 0; i < iterations; i++) {
        printf("Child Pid: %d is going to sleep!\n", getpid());

        // Random sleep time between 1 and 10 seconds
        int sleepTime = random() % 10 + 1;
        sleep(sleepTime);

        printf("Child Pid: %d is awake! Where is my Parent: %d?\n", getpid(), getppid());
    }
}

int main() {
    pid_t pid1, pid2;   // To store process IDs for two children
    int status;         // To store status when waiting for child processes

    // Seed the random number generator using the current time
    srandom(time(NULL));

    // Create the first child process
    pid1 = fork();
    if (pid1 == 0) {
        // Inside the first child process
        childProcess();
        exit(0);  // Exit the child process after it's done
    }

    // Create the second child process
    pid2 = fork();
    if (pid2 == 0) {
        // Inside the second child process
        childProcess();
        exit(0);  // Exit the child process after it's done
    }

    // Parent process waits for both child processes to finish
    waitpid(pid1, &status, 0);
    printf("Child Pid: %d has completed\n", pid1);

    waitpid(pid2, &status, 0);
    printf("Child Pid: %d has completed\n", pid2);

    return 0;
}