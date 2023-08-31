//
//  main.c
//  CProject_Linked_Lists_Train
//
//  Created by Helga on 07/08/2022.
///* Create a node structure and a list structure.
///Create a linked list and add nodes to it.
///Display the list on the screen,
///Enlarge the list, so that for each pair of neighbour nodes we create a new node and add it between them.
///Its value should equal the arythm mean of those two neighbour nodes (2 4 6)->(2 3 4 5 6).
///Store info in dynamic memory */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/* A linked list node */
typedef struct nodee{
    struct nodee* next;
    struct nodee* prev;
    double val;
}node;


/* A linked list */
typedef struct {
    node* head;
    node* tail;
}list;

/* Function to add nodes in a given linked list */
void addNode(node* leftNode, double value, list* list){
    
    // create a new node
    node* newNode = malloc(sizeof(node));
    newNode->val = value;
    
    // add a node to an empty list
    if (list->head==NULL && list->tail==NULL) {
        newNode->next = NULL; // tell neighbours about yourself
        newNode->prev = NULL; // tell neighbours about yourself
        list->head = newNode; // update the head of the list
        list->tail = newNode; // update the tail of the list
    }
    
    // 1. add a node in front of the head
    else if (leftNode==NULL) {
        newNode->next = list->head;    // tell your right neigbour
        newNode->prev = NULL;          // tell your left neigbour
        list->head = newNode;          // update the head
        newNode->next->prev = newNode; // create the right-left link from the old el to the new one
    }
    
    // 2. add a node
    else if (leftNode!=NULL) {
        
        // 2.a. add a node behind the tail
        if (leftNode->next==NULL) {  // OR list->tail=leftNode
            leftNode->next=newNode;  // tell leftNode about its new neighbour to the right
            newNode->next=NULL;      // tell your right neighbour
            newNode->prev=leftNode;  // tell your left neighbour
            list->tail=newNode;      // update the tail
        }
        // 2.b. add a node between two old nodes
        else if (leftNode->next!=NULL){
            newNode->next=leftNode->next; // tell your right neighbour
            newNode->prev=leftNode;       // tell your left neighbour
            leftNode->next=newNode;       // update the old left-right link
            newNode->next->prev=newNode;  // update the old right-left link
        }
    }
}


/* Function to add nodes in a given linked list: With a right node */
void addNodeTwo (node* leftNode, double value, list* list) {
    // printf("%d \n", val);
    // checks the node to the right
    node* rightNode;
    if (leftNode!=NULL) {
        rightNode = leftNode->next;
    }else{
        rightNode = list->head;
    }
    
    
    //create a new node
    node* newNode = malloc(sizeof(node));
    newNode->val=value;
    
    //Var 1. Set up connections FROM the new node TO the existing nodes
    newNode->next=rightNode;
    newNode->prev=leftNode;
        
    //Var 2. Set up connections FROM the old nodes TO the new one
    
    //from leftNode to newNode
    if (leftNode!=NULL) {
        leftNode->next=newNode;
    }
    
    //from rightNode to newNode
    if (rightNode!=NULL) {
        rightNode->prev=newNode;
    }
    
    //Var 3. Set up connections FROM head/tail pointers TO the list
    if (newNode->next==NULL) {
        list->tail=newNode;
    }
    if (newNode->prev==NULL) {
        list->head=newNode;
    }
}


/* Function to enlarge a given linked list with an arythm. mean of each pair of neighbour nodes */
void findMean (list* list) {
    
    double mean = 0;
    node* iterator=list->head;
    while (iterator!=NULL && iterator->next!=NULL) {
        
        mean=(iterator->val + iterator->next->val)/2;
       
        addNode(iterator, mean, list);
        
        iterator=iterator->next->next;
    }
    printf("\n");
}

/* Function to delete a node from a given linked list */
void deleteNode(node* nodeToDelete, list* list) {
    if (nodeToDelete==NULL) {               // checks if the obj to delete exists
        return;
    }
    node* leftNode = nodeToDelete->prev;    // sets the pointer for the left neighbour
    node* rightNode = nodeToDelete->next;   // sets the pointer for the right neighbour
    free(nodeToDelete);                     // dynamic memory is alloctated back to heap
    
    if (leftNode!=NULL) {
        leftNode->next=rightNode;
    }else{
        list->head=rightNode;
    }
    if (rightNode!=NULL) {
        rightNode->prev=leftNode;
    }else{
        list->tail=leftNode;
    }
}

/* Function to print nodes in a given linked list */
void showList(list* list){
    node* iterator = list->head;
    while (iterator!=NULL) {
        
        printf("%2.1f \t", iterator->val);
        
        iterator=iterator->next;
    }
    printf("\n");
    
    iterator=list->tail;
    while (iterator!=NULL) {
        
        printf("%2.1f \t", iterator->val);
        
        iterator=iterator->prev;
    }
    printf("\n");
}

int main() {
    list myList;
    myList.head=NULL;
    myList.tail=NULL;
    addNode(myList.head, 1, &myList); // 1
    addNode(NULL, 0, &myList); // 0 1
    addNode(myList.tail, 8, &myList); // 0 1 8
    addNode(myList.head->next, 6, &myList);  // 0 1 6 8
    
    //showList(&myList); // 0.0  1.0   6.0  8.0
    findMean(&myList);  //
    showList(&myList); //  0.0    0.5   1.0   3.5   6.0   7.0   8.0 // 8.0     7.0     6.0     3.5     1.0     0.5     0.0
    deleteNode(myList.head->next->next, &myList); // 1.0 is deleted
    showList(&myList); // 0.0     0.5     3.5     6.0     7.0     8.0 // 8.0     7.0     6.0     3.5     0.5     0.0
    
    
    addNodeTwo(myList.head, 10, &myList);
    showList(&myList); // 0.0     10.0     0.5     3.5     6.0     7.0     8.0 // 8.0     7.0     6.0     3.5     0.5     10.0     0.0
}
