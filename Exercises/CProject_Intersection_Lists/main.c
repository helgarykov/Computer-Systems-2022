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
///Store info in dynamic memory
///Write funs Sort(), DeleteDublicates(), FindIntersectionOfLists(), FindUnionOfLists(),
///FindComplementOfList(), FindSymmetricDifferenceOfLists() */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/* A linked list node */
typedef struct nodee{
    struct nodee* next;
    struct nodee* prev;
    int val;
}node;


/* A linked list */
typedef struct {
    node* head;
    node* tail;
}list;

/* Function to add nodes in a given linked list */
void addNode (node* leftNode, int value, list* list) {
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
        
        printf("%d \t", iterator->val);
        
        iterator=iterator->next;
    }
    printf("\n");
}


/* LIST implementation of the BUBBLE-SORT (increasing order) */
//The light/smaller items "bubble" up (column) or move to the left (head) of the list.
//The heavy/larger items "bubble" down (column) or more towards the tail of the list

void sortList (list* list){
    
    int count=0;                                                            //create var to get the nr of elements in the list
    int shifts=1;                                                           //create var to terminate the outer loop and the fun, when swapping doesn't take place
    
    node* iteratorNewList=list->head;                                       //start sort from list HEAD
    while (iteratorNewList!=NULL) {                                         //get nr of items in the list
        
        count++;
        iteratorNewList=iteratorNewList->next;
    }
    for (int i=0; i<count; i++) {                                           //begins iteration with HEAD (item a[0]) and ends with the last item in a list
        
        iteratorNewList=list->head->next;                                   //move the current pointer to head->next
        if (shifts==0) {
            break;                                                          //return when no swaps have been done in the inner-loop
        }
        
        shifts=0;
        for (int j=1; j<count; j++) {
            
            if (iteratorNewList->prev->val > iteratorNewList->val) {        // 7 0 10 1 8 9 10 12 (after 12, it jumps out of the j-loop and up into the i-loop: i=1 (here 7) and repeats the procedure)
                
                int temporary=iteratorNewList->prev->val;                   // temp=7
                iteratorNewList->prev->val=iteratorNewList->val;            // prev=0
                iteratorNewList->val=temporary;                             // current iterator=7
                shifts++;                                                   // swaps=1
            }
            iteratorNewList=iteratorNewList->next;                          //pointer moves to current iterator->next=10(corresponds to j=2 or 2nd iterat of the j-loop)
        }                                                                   //after 1.round: 0 7 1 8 9 10 10 12
    
    } //showList(list);
    
}

/* Function to delete dublicates in the list */
void removeDublicates (list* list){
    
    node* iteratorNewList=list->head;
    while (iteratorNewList!=NULL && iteratorNewList->next!=NULL) {
        
         if (iteratorNewList->val==iteratorNewList->next->val) {
            
            iteratorNewList=iteratorNewList->next;                      //save the adress of the next element in the iterator pointer
            deleteNode(iteratorNewList->prev, list);                    //now delete the previous to iterator element
         }else{
             
            iteratorNewList=iteratorNewList->next;
             
         }
    }
}



/* Function to find intersection of two lists */
//Before running this fun, the two lists should be
//sorted and cleaned up of the dublicates.
void listsIntersect (list* listOne, list* listTwo){
    
    //allocate dynamic memory to newList
    list* newList = malloc(sizeof(list));
    
    
    int valueToInsert;
    node* iteratorListOne=listOne->head;
    while (iteratorListOne!=NULL) {
       
        node* iteratorListTwo=listTwo->head;
        while (iteratorListTwo!=NULL) {
            
            if (iteratorListTwo->val==iteratorListOne->val) {
                valueToInsert=iteratorListOne->val;
                addNode(newList->head, valueToInsert, newList);
            }
            iteratorListTwo=iteratorListTwo->next;
        }
        iteratorListOne=iteratorListOne->next;
    }
    sortList(newList);
    showList(newList);
}


/* Function to find the union of two lists */
void getUnionOfLists(list* listOne, list* listTwo){
    
    
    list* newList=malloc(sizeof(list));
    
    node* iteratorListOne=listOne->head;
    node* iteratorListTwo=listTwo->head;
    
    while (iteratorListOne!=NULL) {
        addNode(newList->head, iteratorListOne->val, newList);
        iteratorListOne=iteratorListOne->next;
    }
    while (iteratorListTwo!=NULL) {
        addNode(newList->tail, iteratorListTwo->val, newList);
        iteratorListTwo=iteratorListTwo->next;
    }
    
    sortList(newList);
    removeDublicates(newList);
    showList(newList);
}

/*Function to delete all nodes equal to this node in a list*/
void deleteAllNodes(int valToDel, list* list){
    
    node* iterator=list->head;
  
    while (iterator!=NULL) {
        if (iterator->val==valToDel) {
            iterator=iterator->next;
            free(iterator->prev);
        }else{
            iterator=iterator->next;
        }
    }
}

/* Function to check if the list contains the item */
int listContains(int val, list* list){
    node* iterator=list->head;
    while (iterator!=NULL) {
        if (iterator->val==val) {
            //printf("The list contains number");
            return 1;
        }else{
            iterator=iterator->next;
        }
    }
    return 0;
}

/* Function to find the COMPLEMENT of a list */
//Fun gets the complement of the list passed to fun as the 2nd param.
void getComplementOfList(list* listOne, list* listTwo){
    
    list* newList=malloc(sizeof(list));
    
    node* iteratorListOne=listOne->head;
    
    while (iteratorListOne!=NULL) {
        
        
        if (!listContains(iteratorListOne->val, listTwo)) {
            addNode(newList->head, iteratorListOne->val, newList);
        }
        iteratorListOne=iteratorListOne->next;
    }
    sortList(newList);
    removeDublicates(newList);
    showList(newList);
}

/* Function to find symmetric difference of the two lists */
void getSymmetricDifferenceOfLists(list* listOne, list* listTwo){
    
    list* newList=malloc(sizeof(list));
    
    node* iteratorListOne=listOne->head;
    while (iteratorListOne!=NULL) {
        
        if (!listContains(iteratorListOne->val, listTwo)) {
            addNode(newList->head, iteratorListOne->val, newList);
        }
        iteratorListOne=iteratorListOne->next;
    }
    
    node* iteratorListTwo=listTwo->head;
    while (iteratorListTwo!=NULL) {
        
        if (!listContains(iteratorListTwo->val, listOne)) {
            addNode(newList->tail, iteratorListTwo->val, newList);
        }
        iteratorListTwo=iteratorListTwo->next;
    }
    sortList(newList);
    showList(newList);
}


int main() {
    list myList;
    list myList2;
    myList.head=NULL;
    myList.tail=NULL;
    myList2.head=NULL;
    myList2.tail=NULL;
    
    // add nodes to myList
    addNode(myList.head, 1, &myList);               // 1
    addNode(NULL, 0, &myList);                      // 0 1
    addNode(myList.tail, 8, &myList);               // 0 1 8
    addNode(myList.head->next, 6, &myList);         // 0 1 6 8
    showList(&myList);                              // 0 1 6 8
    deleteNode(myList.head->next->next, &myList);   // 6 is deleted
    showList(&myList);                              // 0 1 8
    addNode(myList.tail, 9, &myList);
    showList(&myList);                              // 0 1 8 9
    addNode(NULL, 7, &myList);
    showList(&myList);                              // 7 0 1 8 9
    
    // add nodes to myList2
    addNode(myList2.head, 1, &myList2);               // 1
    addNode(NULL, 7, &myList2);                       // 7 1
    addNode(myList2.tail, 5, &myList2);               // 7 1 5
    addNode(myList2.head->next, 6, &myList2);         // 7 1 6 5
    showList(&myList2);                               // 7 1 6 5
    deleteNode(myList2.tail, &myList2);               // 5 is deleted
    showList(&myList2);                               // 7 1 6
    addNode(myList2.tail, 8, &myList2);
    showList(&myList2);                               // 7 1 6 8
    
    // two lists intersect
    listsIntersect(&myList, &myList2);                // 7 8 1
    
    // add more items to both lists
    addNode(myList.tail, 10, &myList);                // 7 0 1 8 9 10
    addNode(myList.tail, 12, &myList);                // 7 0 1 8 9 10 12
    addNode(myList.head->next, 10, &myList);          // 7 0 10 1 8 9 10 12
    showList(&myList);
    
    
    addNode(myList2.tail, 10, &myList2);                // 7 1 6 8 10
    addNode(myList2.tail, 13, &myList2);                // 7 1 6 8 10 13
    addNode(myList2.head->next, 10, &myList2);          // 7 1 10 6 8 10 13
    showList(&myList2);
    
    
    //Sort lists
    sortList(&myList);
    showList(&myList);                                  // 0 1 7 8 9 10 10 12
    sortList(&myList2);
    showList(&myList2);                                 // 1 6 7 8 10 10 13
    
    //Delete dublicates
    removeDublicates(&myList);
    showList(&myList);                                  // 0 1 7 8 9 10 12
    removeDublicates(&myList2);
    showList(&myList2);                                 // 1 6 7 8 10 13
    
    //two lists intersect
    listsIntersect(&myList, &myList2);                  // 1 7 8 10
    
    //two lists are united
    getUnionOfLists(&myList, &myList2);                 // 0 1 6 7 8 9 10 12 13
    
    //the complement of the 2nd list
    getComplementOfList(&myList, &myList2);             // 0 9 12
    getComplementOfList(&myList2, &myList);             // 6 13
    
    //the symmetric difference of two lists
    getSymmetricDifferenceOfLists(&myList, &myList2);   // 0 6 9 12 13
    
    //list contains
    //listContains(13, &myList2);
}

