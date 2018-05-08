#include "Queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void main(void)
{
    Queue *Q = createQueue(5);
    int fd = 1;
    char a[1024] = "Accept\n";
    char b[1024] = "Attack\n";
    Enqueue(Q, a, 1);
    printf("Front element is %s\n", front(Q));
    Enqueue(Q, b, 1);
    printf("Front element is %s\n", front(Q));
}