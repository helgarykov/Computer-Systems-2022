//
//  main.c
//  CProject_Partition_QuickSort
//
//  Created by Helga on 16/09/2022.
//

#include <stdio.h>

int partition(int *array, int p, int r) {
    
    int pivot = array[p];
    int i = p-1;
    int j = r;
    
    while (1) {
        
        do { j--; } while (pivot < array[j]);
        
        do { i++; } while (array[i] < pivot);
        
        if (i < j) {
            int tmp = array[i];
            array[i] = array[j];
            array[j] = tmp;
        } else {
            return printf("Index after partition: \t%d \n", j+1);
        }
    }
}


void quicksort(int *array, int start, int end) {
    
    if (end-start < 2) {
        return;
    }
    
    int q = partition(array, start, end);
    quicksort(array, start, q);
    quicksort(array, q, end);
}

void showArray (int* array) {
    
    //int length = sizeof(array)/sizeof(array[0]);
    
    for (int i = 0; i < 11; ++i) {
        printf("%d \t", array[i]);
    }
    printf("\n");
}


int main(int argc, const char * argv[]) {
    
    //int arrayToPartition[20] = {0,50, 4, 9, 31, 22, 22, 8, 3, 50, 5, 9, 3, 12, 39, 27, 16, 14, 29, 17};
    
    int arrayToPartition[11] = {30, 7, 50, 9, 31, 6, 22, 8, 3, 4, 0};
    showArray(arrayToPartition);
    partition(arrayToPartition, 0, 11);
    showArray(arrayToPartition);
    
    return 0;
}
