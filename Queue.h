#include <stdio.h>
#include <stdlib.h>
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
    int time;
}Command;

typedef struct Queue
{
        int capacity;
        int size;
        int front;
        int rear;
        Command **elements;
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
        if(Q->size!=0)
        {
                Q->size--;
                Q->front++;
                /* As we fill elements in circular fashion */
                if(Q->front==Q->capacity)
                {
                        Q->front=0;
                }
        }
        return;
}

void Enqueue(Queue *Q, char *element, int fd, int time)
{
        //char *p = (char *) malloc(strlen(element)+1);*/

        /* If the Queue is full, we cannot push an element into it as there is no space for it.*/
        if(Q->size == Q->capacity)
        {
                printf("Queue is Full\n");
        }
        else
        {
                Q->size++;
                Q->rear = Q->rear + 1;
                /* As we fill the queue in circular fashion */
                if(Q->rear == Q->capacity)
                {
                        Q->rear = 0;
                }
                /* Insert the element in its rear side */ 
                strcpy(Q->elements[Q->], element);
        }
        return;
}