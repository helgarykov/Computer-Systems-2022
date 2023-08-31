//
//  main.c
//  CProject_More_Recursion
//
//  Created by Helga on 03/09/2022.
//

#include <stdio.h>
#include <stdlib.h>

/* Fun to sort the array */
void bubbleSort(int *num, int sizeOfArray)
{
  
  for (int i = 0; i < sizeOfArray - 1; i++)
  {
    for (int j = (sizeOfArray - 1); j > i; j--)
    {
      if (num[j - 1] > num[j])
      {
        int temp = num[j - 1];
        num[j - 1] = num[j];
        num[j] = temp;
      }
    }
  }
}

int* removeDuplicates(int arrayOfInts[], int sizeOfArray) {
    
    int i=0;
    
    int noDuplicates = 1;
    
    int emptyArrayPos = 0;
    
    int* newArray;
    
    // int newArray[sizeOfArray];
    
    newArray = malloc(sizeOfArray * sizeof(int));
    
    while (i < sizeOfArray) {
        
        for (int j= i+1; j < sizeOfArray - 1; j++) {
            
            if(arrayOfInts[i] == arrayOfInts[j]) {
                
                noDuplicates = 0;
            }
            
        }
        if (noDuplicates) {
            newArray[emptyArrayPos] = arrayOfInts[i];
            emptyArrayPos++;
        }
        noDuplicates = 1;
    }
    return newArray;
}


/* Function to find all possible distinct combinations of numbers
 param <numOfCell> nr of cell of outputArray */
void findAllDistinctCombinations(int arrayOfInts[], int* outputArray, int sizeOfArray, int k, int numOfCell){
    
    if (numOfCell < k) {
    
        for (int i = 0; i < sizeOfArray; i++) {
        
            outputArray[numOfCell] = arrayOfInts[i];
            
            findAllDistinctCombinations(arrayOfInts, outputArray, sizeOfArray, k, numOfCell + 1);
           
        }
        
    }
    
    else {
        
        for (int i = 0; i < k; i++) {
            
            printf("%d \t", outputArray[i]);
            
        }
        printf("\n");
    }
}

/* Master fun to print the combinations and calls the recursive fun
 where necessary. */
void printAllDistinctCombinations(int arrayOfInts[], int sizeOfArray, int k) {
    
    int* outputArray = malloc(k * sizeof(int));
    
    findAllDistinctCombinations(arrayOfInts, outputArray, sizeOfArray, k, 0);
    
}


int main(int argc, const char * argv[]) {
   
    
    
    int arrayOfInts [5] = {1, 2, 3, 4, 5};
    
    int sizeOfArray = 5;
    
    // number of elements in a subset
    //int k;
    
    
    
    
    //printf("Enter the max number of ints:\n");
    //scanf("%d \n", &sizeOfArray);
    
   
    printAllDistinctCombinations(arrayOfInts, sizeOfArray, 3);
    
  
    
    return 0;
}
