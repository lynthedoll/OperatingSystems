#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

int signal_received = 0;
int alarm_count = 0;
time_t start_time;

// SIGALRM handler
void handler(int signum) {
    alarm_count++;
    printf("Hello World!\n");
}

// SIGINT handler (Ctrl+C)
void sigint_handler(int signum) {
    time_t end_time = time(NULL);  // Get the current time when Ctrl+C is pressed
    printf("\nProgram executed for %ld seconds and received %d alarms.\n", end_time - start_time, alarm_count);
    exit(0);  // Exit the program
}

int main(int argc, char *argv[]) {
    signal(SIGALRM, handler);  // Register the signal handler for SIGALRM
    signal(SIGINT, sigint_handler);  // Register the signal handler for SIGINT

    start_time = time(NULL);  // Record the start time

    while (1) {
        alarm(1);  // Schedule a SIGALRM every 1 second
        pause();   // Wait for the signal to be delivered
    }

    return 0;
}