/*
 * C Program to Implement various Queue Functions using Dynamic Memory Allocation
 */
#include <stdio.h>
#include <stdlib.h>

struct node
{
    int data;
    struct node *link;
};

struct queue
{
    struct node* front;
    struct node* rear;
};

// to insert elements in queue
void push(struct queue *q, int a)
{
    struct node *temp;
 
    temp = (struct node*)malloc(sizeof(struct node));
    //printf("Enter value to be inserted \n");
    temp->data = a;
    temp->link = NULL;
    if (q->rear  ==  NULL)
    {
        q->front = q->rear = temp;
    }
    else
    {
        q->rear->link = temp;
        q->rear = temp;
    }    
}
 
// delete elements from queue
int pop(struct queue *q)
{
    struct node *temp;
 
    temp = q->front;
    if (q->front == NULL)
    {
        //printf("queue is empty \n");
        q->front = q->rear = NULL;
        return -1;
    }
    else
    {    
        //printf("deleted element is %d\n", q->front->data);
        q->front = q->front->link;
        free(temp);
        return 0;
    }
}
 /*
// check if queue is empty or not
void check(struct queue *q)
{
    if (q->front == NULL)
        printf("\nQueue is empty\n");
    else
        printf("*************** Elements are present in the queue **************\n");
}
 */
// returns first element of queue
int front(struct queue *q)
{
    if (q->front == NULL)
    {
        //printf("**************** The queue is empty ****************\n");
        return -1;
    }
    else
        //printf("**************** The front element is %d ***********\n", q->front->data);    
        return q->front->data;    
}
 
// returns number of entries and displays the elements in queue
int size(struct queue *q)
{
    struct node *temp;
 
    temp = q->front;
    int cnt = 0;
    if (q->front  ==  NULL)
    {
        //printf(" queue empty \n");
        return 0;
    }
    while (temp)
    {
        //printf("%d ", temp->data);
        temp = temp->link;
        cnt++;
    }
    return cnt;
}

int delete(struct queue *q, int del){
    struct node *temp;
 
    temp = q->front;
    if (q->front  ==  NULL)
    {
        //printf(" queue empty \n");
        return -1;
    }
    if(q->front->data == del){
        q->front = q->front->link;
        if(q->rear->data == del) q->rear = q->rear->link;
        return 1;
    }
    while (temp)
    {
        //printf("%d  ", temp->data);
        if(temp->link){
            if(temp->link->data == del) {
                temp->link = temp->link->link;
                if(q->rear->data == del) q->rear=temp;

                return 1;
            }
        }
        temp = temp->link;
    }
    return -1;
}
/*
int main()
{
    //int choice;
    struct queue *q = (struct queue*) malloc(sizeof(struct queue));
 
    push(q,1);
    push(q,2);
    push(q,3);
    push(q,4);
    push(q,5);
    push(q,6);
    printf("\n%d\n\n", size(q));
    /*
    delete(q,1);
    pop(q);
    push(q,6);
    printf("\n%d\n", size(q));
    
    return 0;
}
*/