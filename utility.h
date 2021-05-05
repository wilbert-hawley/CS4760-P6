#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>

struct msg_info {
  int local_pid;
  int address;
  int page;
  char type;
  bool term;
};

struct msgbuf {
  long mtype;
  struct msg_info mi;
};

struct PTE {
  int frame;
  int address;
  int dirty;
  int valid;
};

struct PCB{
  pid_t pid;
  struct PTE ptable[32];
};

struct shmbuf {
  unsigned int sec;
  unsigned int nanosec;
  struct PCB ptable[18];
};


struct Queue {
    int front, 
        rear, 
        size;
    unsigned capacity;
    int* pcb_index;
};

struct Queue* createQueue(unsigned);
int isFull(struct Queue*);
int isEmpty(struct Queue*);
void enqueue(struct Queue*, int);
int dequeue(struct Queue*);
int front(struct Queue*);
int rear(struct Queue*);
bool empty_blocked_queue(int []);
