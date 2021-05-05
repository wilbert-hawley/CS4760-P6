#include "utility.h"

// This queue structure was taken (and slightly altered) from:
// // https://www.geeksforgeeks.org/queue-set-1introduction-and-array-implementation/
struct Queue* createQueue(unsigned capacity)
{
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
 
    queue->rear = capacity - 1;
    queue->pcb_index = (int*)malloc(queue->capacity * sizeof(int));
    return queue;
}

int isFull(struct Queue* queue)
{
    return queue->size == queue->capacity;
}
 

int isEmpty(struct Queue* queue)
{
    return queue->size == 0;
}

void enqueue(struct Queue* queue, int item)
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->pcb_index[queue->rear] = item;
    queue->size += 1;
}
 

int dequeue(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    int pcbi = queue->pcb_index[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size -= 1;
    return pcbi;
}

int front(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->pcb_index[queue->front];
}
 

int rear(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->pcb_index[queue->rear];
}
