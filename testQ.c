#include "Queue.h"


void main(void)
{
    Queue *Q = createQueue(5);
    int fd = 1;
    char a[1024] = "Accept\n";
    char b[1024] = "Attack\n";
    Enqueue(Q, a);
    printf("Front element is %s\n", front(Q));
    Enqueue(Q, b);
    printf("Front element is %s\n", front(Q));
    Command d;
    Dequeue(Q);
    printf("Front element is %s\n", front(Q));
}