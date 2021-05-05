// Wilbert Hawley
// Project 6
// child_proc.c

#include "utility.h"

int main(int argc, char** argv) {
  // connect to shared memory
  key_t sharedMemoryKey;
  if ((sharedMemoryKey = ftok("./utility.h", 0)) == ((key_t) - 1))
  { 
    fprintf(stderr, "%s: ", argv[0]);
    perror("Error: Failed to derive key from utility.h\n");
    exit(EXIT_FAILURE);
  }
  int shmid;
  if ((shmid = shmget(sharedMemoryKey, sizeof(struct shmbuf), IPC_CREAT | 0600)) == -1)
  { 
    fprintf(stderr, "%s: ", argv[0]);
    perror("Error: Failed to create shared mem with key\n");
    exit(EXIT_FAILURE);
  }

  struct shmbuf *shmp = (struct shmbuf *)shmat(shmid, NULL, 0);
  
  // set up message queue
  key_t messageKey1;
  if ((messageKey1 = ftok("./README", 0)) == ((key_t) - 1))
  {
    fprintf(stderr, "%s: ", argv[0]);
    perror("Error: Failed to derive key from README\n");
    exit(EXIT_FAILURE);
  }
  int msqid1;
  if ((msqid1 = msgget(messageKey1, IPC_CREAT | 0600 )) == -1)
  {
    perror("Error: Failed to create message queue\n");
    return EXIT_FAILURE;
  }
  // get process number
  int proc_num = atoi(argv[1]);
  // messages from child to parent
  struct msgbuf msg1;
  
  int count = 0;
  srand(time(0) * getpid());
  // set up message to send back to main procc
  msg1.mtype = getppid();
  msg1.mi.address = rand() % 32000;
  msg1.mi.page = msg1.mi.address >> 10;
  msg1.mi.local_pid = proc_num;
  msg1.mi.term = false;
  // determine whether it's read or write
  if(getpid() % 2 == 0)
    msg1.mi.type = 'r';
  else
    msg1.mi.type = 'w';
  msgsnd(msqid1, &msg1, sizeof(msg1), 0);
  while(count < 5) {
    msgrcv(msqid1, &msg1, sizeof(msg1), getpid(), 0);
    srand(time(0) * getpid());
    msg1.mtype = getppid();
    msg1.mi.address = rand() % 32000;
    msg1.mi.page = msg1.mi.address >> 10;
    if(getpid() % 2 == 0)
      msg1.mi.type = 'r';
    else
      msg1.mi.type = 'w';
    if(count >= 4)
      msg1.mi.term = true;
    msgsnd(msqid1, &msg1, sizeof(msg1), 0);
    count++;
  }
  // detach from schared memory
  shmdt(shmp);
  return 0;
}

