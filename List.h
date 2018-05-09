#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct listdata
{
    int fd;
    struct listdata *next;
};
typedef struct listdata Listdata;

struct list {
    Listdata *head;
};
typedef struct list List;

void initlist(List *ilist)
{  
    ilist->head = 0;
}

void append(List *ilist, int val)
{
    Listdata *newitem;
    newitem = (Listdata *)malloc(sizeof(Listdata));
    newitem->next = ilist->head;
    newitem->fd = val;
    ilist->head = newitem;
}

int contains(List *ilist, int num)
{
    Listdata *ptr;
    if(!ilist->head)
    {
        return 0;
    }
    ptr = ilist->head;
    while(ptr)
    {
        if (num == ptr->fd)
        {
            return 1;
        }
        ptr = ptr->next;
    }
    return 0;
}
void destroy(List *ilist)
{
    Listdata *ptr1, *ptr2;
    if (!ilist->head) return;
    ptr1 = ilist->head;
    while(ptr1)
    {
        ptr2 = ptr1;
        ptr1 = ptr1->next;
        free(ptr2);
    }
    ilist->head = 0;
}