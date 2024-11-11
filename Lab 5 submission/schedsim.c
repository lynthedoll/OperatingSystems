// C program for Simulation Implementation
#include<stdio.h> 
#include<limits.h>
#include<stdlib.h>
#include "process.h"
#include "util.h"

// Calculates waiting time for Round Robin scheduling
void findWaitingTimeRR(ProcessType plist[], int n, int quantum) 
{
  int *rem_bt = (int*)malloc(n * sizeof(int));  // Track remaining burst times
  for(int i = 0; i < n; i++) rem_bt[i] = plist[i].bt;
  for(int i = 0; i < n; i++) plist[i].wt = 0;  // Initialize waiting times
  int t = 0;
  int done;
  do {
    done = 1;
    for(int i = 0; i < n; i++) {
      if(rem_bt[i] > 0) {
        done = 0;
        if(rem_bt[i] > quantum) {
          t += quantum;
          rem_bt[i] -= quantum;
        } else {
          t += rem_bt[i];
          plist[i].wt = t - plist[i].bt;
          rem_bt[i] = 0;
        }
      }
    }
  } while(!done); 
  free(rem_bt);
} 

// Calculates waiting time for Shortest Job First (SJF) scheduling
void findWaitingTimeSJF(ProcessType plist[], int n)
{
  int *rem_bt = (int*)malloc(n * sizeof(int));
  int *completion_time = (int*)malloc(n * sizeof(int));
  int complete = 0, current_time = 0;
  for(int i = 0; i < n; i++) rem_bt[i] = plist[i].bt;
  while(complete != n) {
    int min_bt = INT_MAX, shortest = -1;
    for(int i = 0; i < n; i++) {
      if(rem_bt[i] > 0 && rem_bt[i] < min_bt) {
        min_bt = rem_bt[i];
        shortest = i;
      }
    }
    if(shortest == -1) {
      current_time++;
      continue;
    }
    rem_bt[shortest]--;
    if(rem_bt[shortest] == 0) {
      complete++;
      completion_time[shortest] = current_time + 1;
      plist[shortest].wt = completion_time[shortest] - plist[shortest].art - plist[shortest].bt;
      if(plist[shortest].wt < 0) plist[shortest].wt = 0;
    }
    current_time++;
  }
  free(rem_bt);
  free(completion_time);
} 

// Calculates waiting time for First Come First Serve (FCFS)
void findWaitingTime(ProcessType plist[], int n)
{ 
    plist[0].wt = plist[0].art;
    for (int i = 1; i < n; i++) 
        plist[i].wt = plist[i-1].bt + plist[i-1].wt; 
} 
  
// Calculates turnaround time
void findTurnAroundTime(ProcessType plist[], int n)
{ 
    for (int i = 0; i < n; i++) 
        plist[i].tat = plist[i].bt + plist[i].wt; 
} 

// Comparison function for priority scheduling
int my_comparer(const void *this, const void *that)
{ 
    ProcessType *process1 = (ProcessType *)this;
    ProcessType *process2 = (ProcessType *)that;
    if (process1->pri > process2->pri) return -1;
    if (process1->pri < process2->pri) return 1;
    return 0;
} 

// Calculate average time for FCFS scheduling
void findavgTimeFCFS(ProcessType plist[], int n) 
{ 
    findWaitingTime(plist, n); 
    findTurnAroundTime(plist, n); 
    printf("\n*********\nFCFS\n");
}

// Calculate average time for SJF scheduling
void findavgTimeSJF(ProcessType plist[], int n) 
{ 
    findWaitingTimeSJF(plist, n); 
    findTurnAroundTime(plist, n); 
    printf("\n*********\nSJF\n");
}

// Calculate average time for Round Robin scheduling
void findavgTimeRR(ProcessType plist[], int n, int quantum) 
{ 
    findWaitingTimeRR(plist, n, quantum); 
    findTurnAroundTime(plist, n); 
    printf("\n*********\nRR Quantum = %d\n", quantum);
}

// Calculate average time for Priority scheduling
void findavgTimePriority(ProcessType plist[], int n) 
{ 
  qsort(plist, n, sizeof(ProcessType), my_comparer);
  plist[0].wt = plist[0].art;
  for (int i = 1; i < n; i++) {
    plist[i].wt = plist[i-1].bt + plist[i-1].wt;
    if (plist[i].art > plist[i].wt) plist[i].wt = plist[i].art;
  }
  findTurnAroundTime(plist, n);
  printf("\n*********\nPriority\n");
}

// Print metrics for each process
void printMetrics(ProcessType plist[], int n)
{
    int total_wt = 0, total_tat = 0; 
    float awt, att;
    
    printf("\tProcesses\tBurst time\tWaiting time\tTurn around time\n"); 
    for (int i = 0; i < n; i++) { 
        total_wt += plist[i].wt; 
        total_tat += plist[i].tat; 
        printf("\t%d\t\t%d\t\t%d\t\t%d\n", plist[i].pid, plist[i].bt, plist[i].wt, plist[i].tat); 
    } 
  
    awt = (float)total_wt / n;
    att = (float)total_tat / n;
    
    printf("\nAverage waiting time = %.2f", awt); 
    printf("\nAverage turn around time = %.2f\n", att); 
} 

// Initialize processes from file
ProcessType * initProc(char *filename, int *n) 
{
    FILE *input_file = fopen(filename, "r");
    if (!input_file) {
        fprintf(stderr, "Error: Invalid filepath\n");
        exit(0);
    }
    ProcessType *plist = parse_file(input_file, n);
    fclose(input_file);
    return plist;
}
  
// Main driver function
int main(int argc, char *argv[]) 
{ 
    int n; 
    int quantum = 2;
    ProcessType *proc_list;
  
    if (argc < 2) {
        fprintf(stderr, "Usage: ./schedsim <input-file-path>\n");
        return 1;
    }
    
    // FCFS
    n = 0;
    proc_list = initProc(argv[1], &n);
    findavgTimeFCFS(proc_list, n);
    printMetrics(proc_list, n);
  
    // SJF
    n = 0;
    proc_list = initProc(argv[1], &n);
    findavgTimeSJF(proc_list, n); 
    printMetrics(proc_list, n);
  
    // Priority
    n = 0; 
    proc_list = initProc(argv[1], &n);
    findavgTimePriority(proc_list, n); 
    printMetrics(proc_list, n);
    
    // RR
    n = 0;
    proc_list = initProc(argv[1], &n);
    findavgTimeRR(proc_list, n, quantum); 
    printMetrics(proc_list, n);
    
    return 0; 
} 
