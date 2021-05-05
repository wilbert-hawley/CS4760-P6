#include "utility.h"

int main(int argc, char** argv) {

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

  key_t messageKey2;
  if ((messageKey2 = ftok("./utility.h", 0)) == ((key_t) - 1))
  {
    fprintf(stderr, "%s: ", argv[0]);
    perror("Error: Failed to derive key from README\n");
    exit(EXIT_FAILURE);
  }
  int msqid2;
  if ((msqid2 = msgget(messageKey1, IPC_CREAT | 0600 )) == -1)
  {
    perror("Error: Failed to create message queue\n");
    return EXIT_FAILURE;
  }

  // messages from child to parent
  struct msgbuf msg1;
  // messages from parent to child
  struct msgbuf msg2;
  srand(time(0) * getpid());
  msg1.mtype = getppid();
  msg1.mi.address = rand() % 32000;
  msg1.mi.page = msg1.mi.address >> 10;
  if(getpid() % 2 == 0)
    msg1.mi.type = 'r';
  else
    msg1.mi.type = 'w';
  msgsnd(msqid1, &msg1, sizeof(msg1), 0);
 

  shmdt(shmp);
  return 0;
}

