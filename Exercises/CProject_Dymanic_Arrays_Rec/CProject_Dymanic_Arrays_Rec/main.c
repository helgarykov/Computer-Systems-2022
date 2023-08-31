//
//  main.c
//  CProject_Dymanic_Arrays_Rec
//
//  Created by Helga on 09/09/2022.
//

//
//  main.c
//  CProject_Simple_Demonstration
//
//  Created by Helga on 01/09/2022.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int* createMetaIntArray(int size){
    
    int* newArray = malloc(size * sizeof(int));
    
    newArray[0] = size;
    
    return newArray;
}

/* Creates an array which is a copy of the old one but with one extra cell
 int* array - the pointer to the old array
 int k - the value set in the last cell of the new array */
int* returnsOldArraysCopyWithExtraKValue(int* array, int k){               // enlarge the array by one cell and add value k into it
    
    
    int* newArray = createMetaIntArray(array[0]+1);     //create new array with extra space for k-value
    
    for (int i = 1; i < array[0]; i++) {                // array[0]= size of array is put into the 0. cell
        newArray[i] = array[i];
    }
    
    newArray[newArray[0] - 1] = k;
    
    return newArray;
}


/* Changes the pointer from the address of the old array to the address of the resized array and saves the value k in the last cell
 int** array - the pointer to the pointer of the old array */
void addNewCellToThisArray(int** array, int k){
    
    int* tempArray = returnsOldArraysCopyWithExtraKValue(*array, k);        // create a temporary pointer to the old array
    
    free(*array);                                       // passed the address of the old array as the parameter
    
    *array = tempArray;                                 // pointer that pointed to the address of the old array now points to the address of the new array
    
}

void showMetaArray(int* array){
    
    for (int i = 1; i < array[0]; i++) {
        
        printf("%d ", array[i]);
    }
    //printf("\n");
  
}

/* Function to print all combinations of numbers from 1 to `n` having sum `n`
 param <veryFirstN> initial n */

void printSumRec(int* array, int n, int veryFirstN){
    
    if (veryFirstN == n)
        printf("%d\n", n);
    //showMetaArray(array);                           // 1. itr: empty array
                                                    // 2. itr: 0
    //printf("%d \n", n);                             // 1. itr. n = 5, print 5
                                                    // 2. itr: 4
    
    if(n != 1){
        
        for (int i = 1; i <= n/2; ++i) {
        
        showMetaArray(array);                       //
        printf("%d %d \n", i, n - i);               // print i= 2, print= n-i=5-2
                                                    // print sum = 2 + 3
        
        }
        
        addNewCellToThisArray(&array, 0);           // adds a cell to the array
        printSumRec(array, n - 1, veryFirstN);      // 2. itr prints 0 4
    }
    
}



int main(int argc, const char * argv[]) {
    
    int* array = createMetaIntArray(1);
    int n = 5;
    //printf("%d\n", n);
    printSumRec(array, n, n);
   
    
    /* int* pointerToArray = createMetaIntArray(5);
    
    for (int i=1; i < 5; i++) {
        pointerToArray[i] = i;
    }
    
    addNewCellToThisArray(&pointerToArray, 6);
    
    for (int i=0; i < 6; i++) {
        printf("%d ", pointerToArray[i]);
    } */
    
   
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


