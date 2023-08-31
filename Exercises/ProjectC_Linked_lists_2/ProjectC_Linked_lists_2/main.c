//
//  main.c
//  ProjectC_Linked_lists_2
//
//  Created by Helga on 05/08/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct celll{
    struct celll* previous;
    struct celll* next;
    int value;
}cell;


typedef struct{
    cell* head;
    cell* tail;
}list;


void addCell(cell* leftCell, int value, list* list) {
   
    // create a cell
    cell* newCell = malloc(sizeof(cell));
    newCell->value = value;
    
    // 1) add a cell to an empty list
    if (list->head==NULL && list->tail==NULL) {
        list->head = newCell;
        list->tail = newCell;
        newCell->next = NULL;
        newCell->previous = NULL;
    }
   
    // 2) add a cell in front of the head
    else if (leftCell==NULL) {
        newCell->previous=NULL;
        newCell->next = list->head;
        list->head = newCell;             // tell the list about the new head
        newCell->next->previous = newCell;
    }
    
    // 3.a) add a cell after the cell, whose next is NULL: new tail
    else if (leftCell!=NULL) {
        if (leftCell->next==NULL) {
            leftCell->next= newCell;     // tell the old cells about new neighbour
            newCell->previous = leftCell; // tell the new cell about its neighbours
            newCell->next = NULL;        // tell the new cell about its neighbours
            list->tail=newCell;          // tell the list about the new
            
    // 3.b) add a cell after the cell, whose next is NOT NULL
        } else if (leftCell->next!=NULL) {
            newCell->previous = leftCell;   // tell the new cell about its neighbours
            newCell->next = leftCell->next; // tell the new cell about its neighbours
            newCell->next->previous = newCell; // tell the old cells about new neighbour
            leftCell->next = newCell;       // tell the old cells about new neighbour
        }
    }
}

void showList(list* list){
    
    cell* iterator=list->head;
    while(iterator!=NULL){
        
        printf("%d \t", iterator->value);
        
        iterator=iterator->next;
    }
    printf("\n");
    iterator = list->tail;
    while (iterator!=NULL) {
        
        printf("%d \t", iterator->value);
        
        iterator = iterator->previous;
    }
    printf("\n");
}

int main() {
    list myList;
    myList.head=NULL;
    myList.tail=NULL;
    addCell(myList.head, 1, &myList); // 1
    addCell(NULL, 0, &myList); // 0 1
    addCell(myList.head, 9, &myList); // 0 9 1
    addCell(myList.tail, 6, &myList); // 0 9 1 6
    addCell(myList.head->next, 2, &myList); // 0 9 2 1 6
    showList(&myList);
}

