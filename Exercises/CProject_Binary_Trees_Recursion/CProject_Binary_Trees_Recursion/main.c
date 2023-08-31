//
//  main.c
//  CProject_Binary_Trees_Recursion
//
//  Created by Helga on 29/08/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Algorithms for binary search, insertion and deletion are more efficient
running time wise than those for linear search (with arrays and lists) */

/* A binary tree node */
typedef struct node{
    struct node* left;
    struct node* right;
    int value;
}node;

/* Function to create a node */
node* createNode(int val){
    node* newNode=malloc(sizeof(node));
    newNode->value=val;
    newNode->left=NULL;                     // a newly created node's left child is NULL
    newNode->right=NULL;                    // a newly created node's right child is NULL
    
    return newNode;
}


/* Function to add nodes to a tree */
node* addNode(node* node, int key){
    
    if (node == NULL) {
        
        node = createNode(key);
    }
    else {
        
        if(node->value > key){
            
            node->left=addNode(node->left, key);
        }
        else {
            
            node->right=addNode(node->right, key);
        }
    }
    return node;
}

/* Function to insert a value without taking advantage of return value (pointer to pointer solution).
1st param: pointer to pointer to the node to insert to (parent node)
2nd param: int key to insert (after creting a new node for this value we want to insert) */
void insert(node** node, int key)
{
    if (*node == NULL)
      *node = createNode(key);
    else if ((*node)->value > key)
      insert(&(*node)->left, key);
    else
      insert(&(*node)->right, key);
}


/* Function to search a specific value
 TO DO key not found */
node* search(node* root, int key){
    
    // Base Cases: root is null or key is present at root
    if(root == NULL || root->value == key)
        return root;
    
    // Key is smaller that root's key
    if(root->value > key)
        
        return search(root->left, key);
    
    // Key is greater than root's key
    return search(root->right, key);
}


/* Var 2 of function search(): needs FIX */
void search2(node** root, int key){
    
    // Base Cases: root is null or key is present at root
    if((*root) == NULL) printf("Key not found\n");
    
    else if((*root)->value == key) printf("The key was found  at %d\n", (*root)->value);
        
    // Key is smaller that root's key
    else if((*root)->value > key) {
        search2(&(*root)->left, key);
        //printf("Found key is %d \n", (*root)->value;
    }
    
    // Key is greater than root's key
    else {
        search2(&(*root)->right, key);
        //printf("Found key is %d \n", (*root)->value);
    }
}


/* Function to print all nodes of a tree */
void showNode(node* node){
    
    if (node->left!=NULL)
        showNode(node->left);
    printf("%d \t", node->value);
    
    if (node->right!=NULL)                                      // if FALSE: jumps up to line 73 and prints the val of parent-node
        showNode(node->right);                                  // jumps up to line 71
}

/*
void recursiveFunction(){
    doActionA();                // falling down the rabbit hole
    if()
        recursiveFunction();
    doActionB();                // coming up the rabbit hole
    
} */

int add(int x, int y){
    //printf("Adding in progress");
    if(y == 0)
        return x;
    else
        return (1 + add(x, y - 1));
}



/* Function to print the entered numbers in reverse order */
void reverse(int counter){
    
    if (counter >= 5) {
        return;                 // removing an element from stack of progres
    }
    int x;
    //printf("Enter three number: \t");
    scanf("%d", &x );
    counter++;
    
    reverse(counter);           // adding elements to the stack of progres
    
    printf("%d \t", x);            //Action B
    
    return;                     // removing an element from the stack
}


int main() {
    
    /* Let us create following BST
            10
           /
          5
         /  \
        1    7
            / \
           6   8    */
    
    node* root = NULL;
    insert(&root, 10);
    //showNode(root);
    
    insert(&root, 5);
    //showNode(root);
    
    insert(&root, 1);
    //showNode(root);
    
    insert(&root, 8);
    //showNode(root);
    
    insert(&root, 7);
    //showNode(root);
    
    insert(&root, 6);
    showNode(root);                                          // 1     5     6     7     8     10
    
    
    // Function Call to search()
   /* node* resultS = search(root, 6);
    printf("\nNode found is %d\n", resultS->value);          // Node found is 6 */
   
    
    // Function Call to search2() - need FIX
    search2(&root, 6);
    
    
    
    // Function Call to reverse()
     /* int counter=0;                                          // serial number of reverse() call
     reverse(counter);                                       // 5 4 3 2 1
    
    
    
    
    // Function Call to add()
    int result = add(4, 5);
    printf("%d \n", result); */
    /* int x, y, result;
    printf("\nEnter two intergers: \n");
    scanf("%d %d", &x, &y);
    result = add(x, y);
    printf("Sum of two numbers: %d \n", result); */
    
    return 0;
}



/* Example of recursion on factorial fun:
int factorial(int num){
    if (num!=1) {
        //printf("%d", num);
        return num * factorial(num-1);
    }
    else
        return 1;                           //we are at the bottom of the rabbit hole and it's 1st time B-action is performed
}
factorial(5);

// f1 num=5 r=5*...                        //A-action
// f2 num=4 r=4*...                        //A-action
// f3 num=3 r=3*...                        //A-action
// f4 num=2 r=2*...                        //A-action
// f5 num=1 r=1                            //B-action is performed and we start crawling up the hole

// f6 num=2 r=2*1                          //B-action
// f7 num=3 r=3*2                           ...
// f8 num=4 r=4*6                           ...
// f9 num=5 r=5*24                         // return 120

*/
