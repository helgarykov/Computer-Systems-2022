#include <stdlib.h>
#include <stdio.h> 
#include <assert.h>
#include "stack.h"



typedef struct node{
    
    struct node* lower;                    // a node prior to the top node in a linked list
    void* data;
    
}nodeOfStack;

typedef struct stack{
    
    nodeOfStack* top;                     // pointer to the most recently added item (list->tail)

}stack;


struct stack* stack_new() {
 stack* stack = malloc(sizeof(stack));
 stack->top = NULL;
 return stack;
}

void stack_free(struct stack* stack) {

  if (stack == NULL)                     // nothing to free
    return;

  while (stack->top != NULL)             // free nodes in list, if any
  {
    nodeOfStack* prevTop = stack->top;
    stack->top = stack->top->lower; 
    free(prevTop);
  }
  free(stack);
}

int stack_empty(struct stack* stack) {
  if (stack->top == NULL) return 1;          // true: signal comes                     
  return 0;                                  // false: signal doesn't come              
}

void* stack_top(struct stack* stack) {
  if (stack_empty(stack)) printf("STACK is EMPTY\n");
  nodeOfStack* top = stack->top;
  void* value = top->data;
  return value;
}

void* stack_pop(struct stack* stack) {
  if((stack_empty(stack))) 
    printf("Nothing to pop - STACK is empty\n");
  nodeOfStack* prevTop = stack->top;
  stack->top = stack->top->lower;
  void* value = prevTop->data;
  free(prevTop);
  return value;
}

int stack_push(struct stack* stack, void* data) {
  nodeOfStack* newTop = malloc(sizeof(nodeOfStack));
  newTop->data = data;
  
  newTop->lower = stack->top;
  stack->top = newTop;
  return 0;
}
