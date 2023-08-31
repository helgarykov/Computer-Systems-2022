//
//  main.c
//  CProject_Simple_Demonstration
//
//  Created by Helga on 01/09/2022.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>



/* Function to calculate the chain of the following expression:
 
 a_{1}/(b_{1}+a_{2}/(b_{2}+a_{3}/(b_{3}+a_{4}+...+a_{n-1}/b_{n-1}+a_{n}/b_{n}...))))
 
 where a_{n} and b_{n} are given integers */

float count(int n, int a, int b){
    
    
    if (n == 1)
       
        return a/b;
    
    
    return a / (b + count(n - 1, a, b));
}


/* Function to check the correctness of the parantheses */
// Returns the position of the closing parantethesis')'

int findClosingParantPosOrReturnMinusOne(char array[], int positionOfOpening){
    
    
    for (int i = positionOfOpening + 1; i < 5; i++){
        
        if (array[i] == '(') {
            
            i = findClosingParantPosOrReturnMinusOne(array, i);                   // returns the position of the ')', where findClosingParantPosOrReturnMinusOne stopped
            
            if (i==-1) {                                                          // alerts the previous call of the function about the missing paranthesis ')'
                return -1;
            }
        }
        
        else if (array[i] == ')') {
            
            return i;                                                             // Huzza! Finally found pair! Go to line 79 and assign the value of the found i to i there. (returns the position of ')' )
            
        }
        
    }
    //printf("The ')' not found.");
    return -1;                                                                     // -1 similar to NULL, since position -1 does not exist in an array
}

/* Master function that:
 1) reads a string
 2) calls findClosingParantPosOrReturnMinusOne() where needed
and 3) searches for errors:
3.a) error var 1 : searches ')' without its partner ("Bruh! That array sucks!")
3.b) error var 2 : searches '(' without its partner ("Bruh! That array sucks!")
3.c) no error : all '(' have thier own ')' ("Tada!") */

void preprocessFindClosingParantPosOrReturnMinusOne(char array[]){
    
    //printf("Enter an arithmetix expression with parantheses \n");
    //char array[20];
    //fgets(array, 20, stdin);
    int i=0;
    
    int noErrorFound = 1;                   // default: no error found (1 == TRUE); bool does not exist in C
    
    while (i < 5) {
        
        if (array[i] == '(') {
            
            i = findClosingParantPosOrReturnMinusOne(array, i);
            
            if (i == -1) {
                printf("The number of ')' is less than the nr of '('\n");
                noErrorFound = 0;
                break;
            }
        
        }
        
        
        else if (array[i] == ')') {
            printf("The paranthesis ')' lacks its own '('\n");
            noErrorFound = 0;
            break;
        }
        
        i++;
    }
    
    if (noErrorFound) {
        printf("No error found\n");                         // default
    }
}





int* createMetaIntArray(int size){
    
    int* newArray = malloc(size * sizeof(int));
    
    newArray[0] = size;
    
    return newArray;
}

/* Creates an array which is a copy of the old one but with one extra cell*/
int* returnsOldArraysCopyWithExtraKValue(int* array, int k){               // enlarge the array by one cell and add value k into it
    
    
    int* newArray = createMetaIntArray(array[0]+1);     //create new array with extra space for k-value
    
    for (int i = 1; i < array[0]; i++) {                // array[0]= size of array
        newArray[i] = array[i];
    }
    
    newArray[newArray[0] - 1] = k;
    
    return newArray;
}


/* Changes the pointer from the address of the old array to the address the resized array and saves the value k in the last cell */
void addNewCellToThisArray(int** array, int k){
    
    int* tempArray = returnsOldArraysCopyWithExtraKValue(*array, k);
    
    free(*array);                                       // passed the address of the old array as the parameter
    
    *array = tempArray;                                 // pointer that pointed to the address of the old array now points to the address of the new array
    
}

void showMetaArray(int* array){
    
    for (int i = 1; i < array[0]; i++) {
        
        printf("%d ", array[i]);
    }
    //printf("\n");
  
}

/* Function to print all combinations of numbers from 1 to `n` having sum `n`*/

void printSumRec(int* array, int n){
    
    showMetaArray(array);
    printf("%d \n", n);
    
    if(n != 1){
        
        for (int i = 2; i <= n/2; ++i) {
        
        showMetaArray(array);
        printf("%d %d \n", i, n - i);                // 1, 4 // 2, 3
        
        }
        
        addNewCellToThisArray(&array, 1);
        printSumRec(array, n - 1);
    }
    
}



int main(int argc, const char * argv[]) {
    
    int* array = createMetaIntArray(1);
    printSumRec(array, 5);
   
    
    /* int* pointerToArray = createMetaIntArray(5);
    
    for (int i=1; i < 5; i++) {
        pointerToArray[i] = i;
    }
    
    addNewCellToThisArray(&pointerToArray, 6);
    
    for (int i=0; i < 6; i++) {
        printf("%d ", pointerToArray[i]);
    } */
    
    
    
    
   /* Function call to count():
    int a, b, n;
    printf("Enter max of iterations and two ints:\n");
    scanf("%d %d %d", &n, &a, &b);
    printf("%f\n", count(n, a, b)); */
    
    
    // (2 + 3 * (2 - 1))                                        // No error found
    // ((2 + 3) * (3 - 1) * (5 + 9))                            // No error found (Returns Error : if array size is less than 30)
    // 4 * (2 - 1) / (3 + 7)                                    // No error found (Returns Error : if array size is less than 30)
    // (2 + 3 - 4 * 5)                                          // No error found
    // ( () () () () )                                          // No error found
    // ( )   ()   ()                                            // No error found
    
    // ( () () ( ( )                                            // Error found  :   The number of ')' is less than the nr of '('
    // ( () )   )  )                                            // Error found  :   The paranthesis ')' lacks its own '('
    //
    

    
    
    
    /*char array[5];
    printf("Enter an arithmetic expression with parantheses: \n");
    fgets(array, 5, stdin);                                    //1st. param array of chars-destination point; 2nd. param length of the string; 3rd. param the source of data (file, console..)
    
    //scanf("%s ", array);
    printf("%s \n", array);
    
    preprocessFindClosingParantPosOrReturnMinusOne(array); */
    
    
    
    
    
    
    return 0;
}

// NOTE оn scanf and fgets:
// Read strings with errors if the text is entered directly into the console.
// Use copy-paste!!

// Краткое пояснення:
// Рекурсия - проходим цыклом от А до Б, а потом возвращаемся назад тем же путем.
// Возможные ошибки мы находим методом тыку, а потом прописываем кодом.
// Когда мы работаем с рекурсией, то мы хотим чтобы все было красиво.
// Поэтому у нас есть функция с рекурсией и есть другая функция, которая ее вызывает(preprocess).


