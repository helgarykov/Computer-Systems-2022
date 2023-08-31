//
//  main.c
//  CProject_LinkedLists_Alphabet_Sort
//
//  Created by Helga on 14/08/2022.
//
/* Write a struct with student names and surnames
sort the array alphabetically after surnamees and print in the console.
Place data in the dynamic memory. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* A student struct */
typedef struct stdent{
    
    struct stdent* firstName;
    struct stdent* secondName;
    
}student;

/* A linked list node */
typedef struct nodee{
    
    struct nodee* next;
    struct nodee* prev;
    student* student;
    int numberOfNames;
}node;

/* A linked list */
/*typedef struct{
    
    node* head;
    node* tail;
}list;*/

/* Function that prompts the user to enter a number of students and their names and surnames */
int enterNames(void){
    
    node* node=malloc(sizeof(node));
    
    printf("How many names are you going to enter?");
    scanf("%d", &node->numberOfNames);
    return 1;
}

int main(void){
    
    //create an object
    node* node
    
    //create a list of student names and a list of surnames
    list listOf1stNames;
    list listOf2ndNames;
    listOf1stNames.head=NULL;
    listOf1stNames.tail=NULL;
    listOf2ndNames.head=NULL;
    listOf2ndNames.tail=NULL;
    
    enterNames();
    return 1;
    
    
}
