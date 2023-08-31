//
//  main.c
//  CProject_Queues
//
//  Created by Helga on 10/08/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Create a dynamic structure QUEUE as a singly linked list: FIFO */

typedef struct node{
    
    struct node* next;                                      // only one pointer is required for singly linked lists
    int value;
    int priority;                                           // used for structuring the items in a queue
}nodeOfQueue;

typedef struct{
    
    nodeOfQueue* tail;                                      // queues have both a head and a tail
    nodeOfQueue* head;
}queue;

/* Function to check if the queue is empty */
int queueIsEmpty (queue* queue){
    if (queue->tail==NULL) {
        return 1;
    } else {
        return 0;
    }
}


/* Function to add an item to the queue : add to TAIL */
void enqueue (int value, queue* queue){
    
    //create a node to add
    nodeOfQueue* newNode=malloc(sizeof(nodeOfQueue));
    newNode->value=value;
    newNode->next=NULL;                                     // set its right neighbour to NULL!
    
    //Var 1. Add a node to an empty list
    if (queue->head==NULL) {           //OR (queueIsEmpty(quegue))
        queue->head=newNode;                                //update head
        queue->tail=newNode;                                //update tail
     
    //Var 2. Add a note to NOT EMPTY list
    }else{
    
    queue->tail->next=newNode;
    queue->tail=newNode;
    }
}

/* Function to add items to QUEUE according to their priority field */
//Items with lower priority are added before the items with higher priority.
//Fun is used for structuring the values in a given priority.
void enqueuePriority(int priority, int val, queue* queue){
    
    nodeOfQueue* newNode=malloc(sizeof(nodeOfQueue));
    newNode->value=val;
    newNode->next=NULL;
    
    newNode->priority=priority;
    
    nodeOfQueue* iterator=queue->head;                                      //need the iterator with lists
    
    //Var 1. Add to an empty QUEUE
    if (queue->head==NULL) {
        queue->head=newNode;
        queue->tail=newNode;
    }else{
        
        //Var 2. Head priority is larger than the priority of the added item
        if (queue->head->priority > newNode->priority) {
            newNode->next=queue->head;
            queue->head=newNode;
        }
        
        //Var 3. Tail priority is lower than or equal to the priority of the added item
        else if (queue->tail->priority <= newNode->priority){
            queue->tail->next=newNode;
            queue->tail=newNode;
            
        //Var 4. Add the item between two items in a QUEUE
        }else{
            
            while (iterator!=NULL && iterator->next!=NULL) {

                
                if (iterator->next->priority > newNode->priority) {
                    newNode->next=iterator->next;
                    iterator->next=newNode;
                    break;
                }
                iterator=iterator->next;
            }
            
        }
    }
}


/* Function to delete an item from the QUEUE : delete from HEAD */
int dequeue (queue* queue){
    if (queueIsEmpty(queue)) {
        return 0;
    }
    
    nodeOfQueue* nodeToDel=queue->head;                           //create the pointer of the node to delete and set it to the head
    int result=nodeToDel->value;
    
    //Var 1. Delete an item, when it's the only item in a QUEUE
    if (queue->head==queue->tail || queue->head->next==NULL) {    //each condition is enough to check if this item is the last one
        
        queue->head=NULL;                                         //update the pointer of the head
        queue->tail=NULL;                                         //update the pointer of the tail
        
        free(nodeToDel);                                          //delete the node
        return result;
    
    //Var 2. Delete an item, when there're more than ONE item in a QUEUE
    }else{

        queue->head=queue->head->next;                            //update the pointer of the head
        
        free(nodeToDel);                                          //delete the node (the old head)
        
        //printf("My QUEUE after dequeue() \n");
        return result;
    }
}


void showQueue(queue* queue){
    nodeOfQueue* iterator=queue->head;
    while (iterator!=NULL) {
        printf("%d \t", iterator->value);
        iterator=iterator->next;
    }
    printf("\n");
}

int main() {
    queue myQueue;
    myQueue.head=NULL;
    myQueue.tail=NULL;
    /* enqueue(3, &myQueue);
    showQueue(&myQueue);     // 3
    enqueue(5, &myQueue);
    showQueue(&myQueue);     // 3  5
    enqueue(7, &myQueue);
    showQueue(&myQueue);     // 3  5  7
    dequeue(&myQueue);
    //showQueue(&myQueue);     // 5  7
    //dequeue(&myQueue);
    //showQueue(&myQueue);     // 7
    //dequeue(&myQueue);
    //showQueue(&myQueue);     //
    //enqueue(19, &myQueue);
    //showQueue(&myQueue);     // 19 */
    
    enqueuePriority(1, 2, &myQueue);
    enqueuePriority(0, 2, &myQueue);
    enqueuePriority(5, 6, &myQueue);
    enqueuePriority(1, 9, &myQueue);
    enqueuePriority(7, 3, &myQueue);
    
    showQueue(&myQueue);                    // 2 2 9 6 3
    
}

