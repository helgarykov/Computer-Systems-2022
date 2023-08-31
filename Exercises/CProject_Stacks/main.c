//
//  main.c
//  CProject_Stacks
//
//  Created by Helga on 10/08/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Create a dynamic structure STACK as a singly linked list: LIFO */
typedef struct node{
    
    struct node* lower;                    // a node prior to the top node in a linked list
    int value;
    
}nodeOfStack;

typedef struct{
    
    nodeOfStack* top;                   // pointer to the most recently added item (list->tail)
}stack;


/* Function to add new items to the stack : add to tail/TOP */
void push (int value, stack* stack){
    
    //create a new node (allocate dymanic memory on the heap)
    nodeOfStack* newNode=malloc(sizeof(nodeOfStack));
    newNode->value=value;
    
    //add a new node to the stack
    newNode->lower=stack->top;          // the neighbour down of the newNode is a TOP of a stack : = newNode->prev=list->tail
    stack->top=newNode;
}

/* Function to check whether the stack is empty or not */
int stackIsEmpty (stack* stack){
    if (stack->top==NULL) {
        printf("The stack is empty. \n");
        return 1;                       // returns true val 1 if the stack is empty
    }else{
        return 0;                       // returns true val 0 if the stack contains items
    }
}

/* Function to delete an item from the stack: delete from the tail/TOP */
int pop (stack* stack){
    
    if (stackIsEmpty(stack)) {
        return 0;
    }
    
    nodeOfStack* prevTop=stack->top;    // pointer to save the adress of the TOP
    stack->top=stack->top->lower;       // reassign the adress of the TOP to the item below it
    
    int result=prevTop->value;          // gets the value of the old TOP
    
    free(prevTop);
    
    return result;
}

/* Function to print the items : TOP on the left, Head on the right */
void showStack (stack* stack){
    
    nodeOfStack* iterator=stack->top;
    while (iterator!=NULL) {
        
        printf("%d \t", iterator->value);
        
        iterator=iterator->lower;
    }
    printf("\n");
}

int main()
{
    stack myStack;
    myStack.top = NULL;
    push(1, &myStack);  // 1
    showStack(&myStack);
    push(2, &myStack);  // 2  1
    showStack(&myStack);
    push(3, &myStack);  // 3  2  1
    showStack(&myStack);
    pop(&myStack);  // 2  1
    showStack(&myStack);
    pop(&myStack);  // 1
    showStack(&myStack);
    pop(&myStack);   // NULL
    showStack(&myStack);
    pop(&myStack);
}
