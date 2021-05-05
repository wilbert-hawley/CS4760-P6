#include "utility.h"

struct shmbuf *shmp;
int shmid;

int main(int argc, char** argv) {

  int user_num = 0;
  char options;


  while (true) {
    options = getopt(argc, argv, ":p:");
    
    if((options == -1) || argc < 2)
      break;

    switch(options) {
      case 'p':
        printf("\nOption -%c: %s\n", options, optarg); 
        user_num = atoi(optarg);
        break;
      default:
         printf("Invalid options\n");
         exit(1);
     }
  }

  printf("user_num = %d\n", user_num);
  
  key_t sharedMemoryKey;
  if ((sharedMemoryKey = ftok("./utility.h", 0)) == ((key_t) - 1))
  {
    fprintf(stderr, "%s: ", argv[0]);
    perror("Error: Failed to derive key from utility.h\n");
    exit(EXIT_FAILURE);
  }
  
  if ((shmid = shmget(sharedMemoryKey, sizeof(struct shmbuf), IPC_CREAT | 0600)) == -1)
  {
    fprintf(stderr, "%s: ", argv[0]);
    perror("Error: Failed to create shared mem with key\n");
    exit(EXIT_FAILURE);
  }

  shmp = (struct shmbuf *)shmat(shmid, NULL, 0);

  shmp->sec = 0;
  shmp->nanosec = 0;
 
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
  
  int i = 0;
  struct PTE table[256];
  for(; i < 256; i++) {
    table[i].frame = -1;
    table[i].address = 0;
    table[i].dirty = 0;
    table[i].valid = 0;
  }
  i = 0;
  unsigned int next_proc_sec = 1;
  while(true) {

    shmp->nanosec += 1000000000;
    if(shmp->nanosec > 1000000000) {
      shmp->sec += 1;
      shmp->nanosec -= 1000000000;
    }  

    if(shmp->sec >= next_proc_sec) {
      next_proc_sec++;
      int child = fork();
        switch(child) {
          case -1:
            printf("Failed to fork\n");
            exit(1);
          case 0: ;
            char num2[50];
            sprintf(num2, "%d", i);
            execl("./child_proc", "./child_proc", num2, NULL);
          default:
            msgrcv(msqid1, &msg1, sizeof(msg1), getpid(), 0);
            if(msg1.mi.type == 'r')
              printf("Master: Child %d requesting read of address %d at time %ld:%ld\n", i, msg1.mi.address, shmp->sec, shmp->nanosec);
            else
              printf("Master: Child %d requesting write of address %d at time %ld:%ld\n", i, msg1.mi.address, shmp->sec, shmp->nanosec);
            shmp->nanosec += 500000;
            if(shmp->nanosec > 1000000000) {
              shmp->sec += 1;
              shmp->nanosec -= 1000000000;
            }
        }
    }

    bool fault = true;
    int j = 0;
    for(; j < 256; j++) {
      if(table[j].frame == msg1.mi.page) {
        fault = false;
        table[j].address = msg1.mi.address;
        table[j].valid = 1;
        printf("Master: Address %d in frame %d, giving datat to Child %d at time %d:%d\n", msg1.mi.address, j, i, shmp->sec, shmp->nanosec);
      }
    }

    if(fault) {
      printf("Master: Address %d is not in a frame, pagefault.\n", msg1.mi.address);
      int j = 0;
      for(; j < 256; j++) {
        if(table[j].valid == 0) {
          printf("Master: Clearing frame %d and swapping Child %d page %d\n", j, i, msg1.mi.page);
          table[j].frame = msg1.mi.page;
          table[j].address = msg1.mi.address;
          table[j].valid = 1;
          table[j].dirty = 1;
          if(msg1.mi.type == 'w') {
            printf("Master: Dirty bit of frame %d set, adding additional time to the clock\n", j);
            printf("Master: Indicating to Child %d that write has happened to address %d\n", i, msg1.mi.address);
          }
          break;
       }
     }
   }
  

    i++;
    if(i >= 19)
      break;

  }

  msgctl(msqid1, IPC_RMID, NULL);
  msgctl(msqid2, IPC_RMID, NULL);
  shmctl(shmid, IPC_RMID, NULL);
  return 0;
} 
