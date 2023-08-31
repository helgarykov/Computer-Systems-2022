//
//  main.c
//  CProject_Singly_LInked_Lists
//
//  Created by Helga on 27/08/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Create a structure singly linked list that allows
operations addNode() search() and deleteNode(). */

/* A singly linked node */
typedef struct nodee{
    
    struct nodee* next;
    int val;
}node;


/* A singly linked list */
typedef struct {
    node* head;
    node* tail;
}list;

/* Function to add a node to a list */
void addNode(node* leftNode, int value, list* list){
    
    //allocate memory to newNode
    node* newNode=malloc(sizeof(node));
    newNode->val=value;
    
    //Add to an empty list
    if (list->head==NULL && list->tail==NULL) {
        
        //From new node to old node
        newNode->next=list->head;
        
        //Update head and tail
        list->head=newNode;
        list->tail=newNode;

    }
    //Add a new head
    else if(leftNode==NULL){
        
        //From new node to old node
        newNode->next=list->head;
        
        //From old node to new node
        list->head=newNode;
    }
    //Add a node between old nodes
    else if (leftNode!=NULL){
        
        if (leftNode->next!=NULL) {
            //From new node to old node
            newNode->next=leftNode->next;
            
            //From old node to new node
            leftNode->next=newNode;
        }
        //Add a new tail
        else if (leftNode->next==NULL){
            
            //From old node to new node
            leftNode->next=newNode;
            
            //Update the tail
            list->tail=newNode;
            
            //From new node to old node
            newNode->next=NULL;
        }
    }
}


/* Function to delete a node */
void deleteNode(node* deletable, list* list){
    
    if (deletable==NULL) {
        printf("Node is NULL\n");
        return;
    }else{
        
        // If HEAD is to be deleted
        
        if (list->head==deletable) {
            
            node* delNode=list->head;
            list->head=list->head->next;
            free(delNode);
            //free(&delNode);
            return;
        }
        // If only ONE node is left
        if (list->head->next==NULL) {
            if (list->head==deletable) {
                free(list->head);
                list->head=NULL;
                return;
            }
        }
        // Else loop through the list and search for the node to delete
        node* iterator=list->head;
        while (iterator->next!=NULL) {
            
            if (iterator->next==deletable) {
                node* temp=iterator->next->next;
                free(iterator->next);
                iterator->next=temp;
                return;
            }
            iterator=iterator->next;
        }
    }
}


/* Function to delete a specific value */
void deleteValue(int value, list* list){
    
    
    //If HEAD is to be deleted
    if (list->head->val == value) {
        
        node* delNode=list->head;
        list->head=list->head->next;
        free(delNode);
        return;
    }
    
    //If there is only ONE element in the list
    if (list->head->next == NULL) {
        // If the head is to be deleated. Assign NULL to the head.
        if (list->head->val==value) {
            free(list->head);                //pass the adress of deletable node from list->head to free() and delete the node
            list->head=NULL;                //pointer points to nothing, so we have to NULL it
            return;
        }
        printf("Value is not found!\n");
        return;
    }
    
    // Else loop over the list and search for the node to delete
    node* iterator=list->head;
    while (iterator->next!=NULL) {
        
        // When node is found, delete the node and modify the pointers.
        if (iterator->next->val==value) {
            
            node* temp_iterator=iterator->next->next;
            free(iterator->next);
            iterator->next= temp_iterator;
            return;
        }
        iterator=iterator->next;
    }
    printf("Value not found\n");
}


/* Function to search a specific value */
int search(int value, list* list){
     
    node* iterator = list->head;
     while(iterator != NULL){
         if (iterator->val == value){
             printf("Value %d is found\n", iterator->val);
           return 1;
         }
         iterator = iterator->next;
     }
     printf("Value was not found\n");
     return 0;
}


/* Function to print the list */
void showList(list* list){
    
    node* iterator=list->head;
    while (iterator!=NULL) {
        printf("\nData: %d \t", iterator->val);
        
        iterator=iterator->next;
    }
    printf("\n");
}


int main() {
    
    list singleLinkedList;
    singleLinkedList.head=NULL;
    singleLinkedList.tail=NULL;
    
    addNode(singleLinkedList.head, 1, &singleLinkedList);                   // 1 is added
    addNode(singleLinkedList.head, 2, &singleLinkedList);                   // 1  2
    addNode(NULL, 0, &singleLinkedList);                                    // 0  1  2
    addNode(singleLinkedList.head->next, 3, &singleLinkedList);             // 0  1  3  2
    addNode(singleLinkedList.tail, 4, &singleLinkedList);                   // 0  1  3  2  4
    addNode(singleLinkedList.tail, 5, &singleLinkedList);                   // 0  1  3  2  4  5
    
    showList(&singleLinkedList);
    
    search(9, &singleLinkedList);                                           // Value was not found
    search(5, &singleLinkedList);                                           // Value 5 is found
    
    /*deleteValue(0, &singleLinkedList);                                       // 0 is deleted
    deleteValue(2, &singleLinkedList);                                       // 2 is deleted
    deleteValue(5, &singleLinkedList);                                       // 5 is deleted
    deleteValue(9, &singleLinkedList);                                       // Value not found
    deleteValue(3, &singleLinkedList);                                       // 3 is deleted
    
    showList(&singleLinkedList);*/
    
    
     deleteNode(singleLinkedList.head, &singleLinkedList);                   // 0 is deleted
    
     showList(&singleLinkedList);                                            // 1  3  2  4  5
    
     deleteNode(singleLinkedList.tail, &singleLinkedList);                   // 5 is deleted
    
     showList(&singleLinkedList);                                            // 1  3  2  4
    
    
     deleteNode(singleLinkedList.head->next, &singleLinkedList);             // 3 is deleted
    
     showList(&singleLinkedList);                                            // 1  2  4
    
     deleteNode(singleLinkedList.head->next, &singleLinkedList);             // 2 is deleted
    
     showList(&singleLinkedList);                                            // 1  4
    
     deleteNode(NULL, &singleLinkedList);                                    // Node is NULL
    
     showList(&singleLinkedList);                                            // 1  4  /// 1  4
   
    return 0;
}
