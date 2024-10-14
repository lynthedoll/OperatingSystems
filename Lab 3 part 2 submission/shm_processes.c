#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

void  ParentProcess(int []);
void  ClientProcess(int []);
void  depositMoney(int []);
int childIterations;
int parentIterations;
int  main(int  argc, char *argv[])
{
    int    ShmID;
    int    *ShmPTR;
    pid_t  pid;
    int    status;
    int bankAccount = 0;
    int turn = 0;


    ShmID = shmget(IPC_PRIVATE, 2*sizeof(int), IPC_CREAT | 0666);
    if (ShmID < 0) {
        printf("*** shmget error (server) ***\n");
        exit(1);
    }
    printf("Process has received a shared memory of two integers...\n");

    ShmPTR = (int *) shmat(ShmID, NULL, 0);
    if (*ShmPTR == -1) {
        printf("*** shmat error (server) ***\n");
        exit(1);
    }
    printf("Process has attached the shared memory...\n");

    ShmPTR[0] = bankAccount;
    ShmPTR[1] = turn;

    printf("Original Bank Account Amount = %d\n", ShmPTR[0]);

    pid = fork();

    if (pid < 0) {
        printf("*** fork error (server) ***\n");
        exit(1);
    }

    else if (pid == 0) {
      for (int j = 0 ; j < 25  ; j ++){
        sleep(rand() % 6);
        ClientProcess(ShmPTR);
      }
      exit(0);
    }

  else{
    for (int i = 0; i < 25 ;  i ++ ){

    sleep(rand() % 6);
    ParentProcess(ShmPTR);
  }
  }

     wait(&status);
     printf("Process has detected the completion of its child...\n");
     shmdt((void *) ShmPTR);
     printf("Process has detached its shared memory...\n");
     shmctl(ShmID, IPC_RMID, NULL);
     printf("Process has removed its shared memory...\n");
     printf("Process exits...\n");
     exit(0);
}
void ParentProcess(int SharedMem[]){
    while (SharedMem[1] != 0){ 
    }   
    int account = SharedMem[0]; 

    if (account <= 100){
          depositMoney(SharedMem);
        }
        else{
          printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
          SharedMem[1] = 1;
        }
}
void  ClientProcess(int  SharedMem[]){  
    while (SharedMem[1] != 1){
    }
    int account = SharedMem[0]; 

    int amountStudentNeed = rand() % 51; 
    printf("Poor Student needs $%d\n", amountStudentNeed);

    if (amountStudentNeed <= account){
      account -= amountStudentNeed; 
      printf("Poor Student: Withdraws $%d / Account Balance = $%d\n", amountStudentNeed, account);
      }
    else{
      printf("Poor Student: Not Enough Cash ($%d)\n", account );
    }
    SharedMem[0] = account;
    SharedMem[1] = 0; 
}
void  depositMoney(int  SharedMem[]){
  int account = SharedMem[0];
  int depositAmount = rand() % 101; 
  if (depositAmount % 2 == 0) { //even

    account += depositAmount; 
    printf("Dear old Dad: Deposits $%d / Account Balance = $%d\n", depositAmount, account);
  }
  else{ //odd
    printf("Dear old Dad: Doesn't have any money to give\n");
  }
  SharedMem[0] = account; 
  SharedMem[1] = 1; 
}