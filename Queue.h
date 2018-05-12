#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
/*Queue has five properties.

capacity stands for the maximum number of elements Queue can hold.
  Size stands for the current size of the Queue and elements is the array of elements.
  front is the index of first element (the index at which we remove the element)
  rear is the index of last element (the index at which we insert the element) */
typedef struct Command
{
    int fd;
    char cmd[1024];
    int Time;
}Command;

typedef struct Queue
{
        int capacity;
        int size;
        int front;
        int rear;
        Command *elements;
}Queue;


/* crateQueue function takes argument the maximum number of elements the Queue can hold, creates
   a Queue according to it and returns a pointer to the Queue. */
Queue * createQueue(int maxElements)
{
        /* Create a Queue */
        Queue *Q;
        Q = (Queue *)malloc(sizeof(Queue));
        /* Initialise its properties */
        Q->elements = malloc(sizeof(Command)*maxElements);
        Q->size = 0;
        Q->capacity = maxElements;
        Q->front = 0;
        Q->rear = -1;
        /* Return the pointer */
        return Q;
}

void Dequeue(Queue *Q)
{
        Command C;
        if(Q->size!=0)
        {
                Q->size--;
                Q->front++;
                memset(C.cmd, 0, sizeof(C.cmd));
                int fd, Time;
                strcpy(C.cmd, Q->elements->cmd);
                C.fd = Q->elements->fd;
                C.Time = Q->elements->Time;
        }
        return;
}

char *front(Queue *Q)
{
        if(Q->size != 0)
        {
                return Q->elements[Q->front].cmd;
        } 
        return NULL; 

}

void Enqueue(Queue *Q, char *command)
{
        /* If the Queue is full, No space.*/
        if(Q->size == Q->capacity)
        {
                printf("Queue Cannot take anymore commands\n");
        }
        else
        {
                Q->size++;
                Q->rear++;
                int len = strlen(command);
                command[len-1] = '\0';
                memset(Q->elements[Q->rear].cmd, 0, sizeof(Q->elements[Q->rear].cmd));
                strcpy(Q->elements[Q->rear].cmd, command);
                //struct timeval tv;
                //gettimeofday(&tv, NULL);
                //Q->elements[Q->rear].Time = tv.tv_sec * 1000 + tv.tv_usec/1000;


        }
        return;
}
int Qlength(Queue *Q)
{
        return Q->size;
}