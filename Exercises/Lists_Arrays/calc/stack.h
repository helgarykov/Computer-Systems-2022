#ifndef STACK_H
#define STACK_H

/* Create a dynamic structure STACK as a singly linked list: LIFO.*/
struct stack;

/* Return a new STACK. Initially empty.  NULL on error. */
struct stack* stack_new();

/* Deallocate memory in the end */
void stack_free(struct stack*);

/* Ckeck if STACK is empty. Returns 0 is STACK is empty. */
int stack_empty(struct stack*);

/* Show the top element in STACK */
void* stack_top(struct stack*);

/* Delete an item from the stack: delete from the TOP */
void* stack_pop(struct stack*);

/* Add a new item to the stack : add to TOP */
int stack_push(struct stack*, void*);


#endif
