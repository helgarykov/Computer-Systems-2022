//
//  main.c
//  ProjectC_Pointers
//
//  Created by Helga on 03/08/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

///<summary> Write a struct with student names and surnames
///sort the array alphabetically after surnamees and print in the console.
///Place data in the dinamic memory.<summary>

int main() {
   
    char** studentName;
    char* surname;
    char* name;
    char* temporary;
    int numberOfNames;
    
    
    // enter the number of elements in two arrays: surname and name; should be equal
    printf("How many names are you going to enter?: ");
    scanf("%d", &numberOfNames);
    
    
    //allocate memory to array of pointers to arrays surname and name
    studentName = malloc(2 * sizeof(char*));
    surname = malloc(numberOfNames * sizeof(char));
    name = malloc(numberOfNames * sizeof(char));
    temporary = malloc(numberOfNames * sizeof(char));
    
    // Add values (adresses of arrays) into the block array
    studentName[0] = name;
    studentName[1] = surname;
    
    
    // enter values into array "names"
    printf("Enter first names one by one: ");
    for (int i = 0; i <= numberOfNames; i++) {
        
        scanf("%c", &name[i]);
        
    };

    
    // enter values into the array of surnames and sort the names alphabetically
    printf("Enter the last name one by one: ");
    for (int i = 0; i <= numberOfNames; i++) {
        
        scanf("%c", &surname[i]);
        
        for (int j = 0 + 1; j < numberOfNames ; j++) {
            
            if (strcmp(&surname[i], &surname[j]) > 0) {
                
                strcpy(temporary, &surname[i]);
                strcpy(&surname[i], &surname[j]);
                strcpy(&surname[j], temporary);
            }
        }
    }
    
    // print the names in the order: last name, first name
          
    printf("Order of student names sorted alphabetically by the last name:");
      
    for(int i = 0; i < numberOfNames; i++)
    {
        printf("\n");
 
        for (int j = 0; j < studentName[i][j]; j++)
        {
            printf("%c \t", studentName[i][j]);
        }
        
        // освобождение памяти для одной строки
        //free(studentName[i]);
    }
    
    return 0;
}

///Static array. 

/*char **studentName[2];
char *surname[6][6] = {"Kirkegård", "Schmidt", "Tomborg", "Holberg", "Splidsboel", "Hansen" };
char *name[6][6] = {"Søren", "Victor", "Anne", "Ludvig", "Flemming", "Bjørn"}; */

