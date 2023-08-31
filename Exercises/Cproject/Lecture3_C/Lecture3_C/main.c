//
//  main.c
//  Lecture3_C
//
//  Created by Helga on 30/07/2022.
//

#include <stdio.h>
#include <stdlib.h>

// prototype (header) of the function takes as parameter
void getSum(int** block);

int main(void)
{
    int** block; // variable for pointer for memory block for the array of pointers
    int* array1; // variable for pointer for memory block for array 1
    int* array2; // pointer for memory block for array 2
    int* array3; // pointer for memory block for array 3
    int narr1;      // number of elements in an array 1
    int narr2;      // number of elements in an array 2
    int narr3;      // number of elements in an array 3
    
    // enter the size of elements in the three arrays
    printf("Size of array 1 = ");
    scanf("%d", &narr1);
    
    
    printf("Size of array 2 = ");
    scanf("%d", &narr2);
    
    printf("Size of array 3 = ");
    scanf("%d", &narr3);
    
    // allocate memory to the array of array1, array2, array3 and pointers
    block = malloc(3 * sizeof(int*)); // allocates memory for array of pointers to arrays, but it is empty now

    array1 = malloc(narr1 * sizeof(int));
    array2 = malloc(narr2 * sizeof(int));
    array3 = malloc(narr3 * sizeof(int));
    
    // Add values (adresses of arrays) into the block array
    block[0] = array1; // gets the adress of the 1.cell of array1 and saves in the 0.cell of block array
    block[1] = array2;
    block[2] = array3;
    
    
    // enter value numbers into the array1
    array1[0]=narr1; //  1st element in the array 1 corresponds to narr1 (*array1 = narr1;)
    array2[0]=narr2;
    array3[0]=narr3;
    // find length of arrays
    //int size_array1 = sizeof(array1) / sizeof(*array1);
    
    for (int i = 1; i < narr1; i++) {
        printf("array1[%d] = ", i);
        scanf("%d", &array1[i]);
    };
    
    // enter value numbers into the array2
    for (int i = 1; i < narr2; i++) {
        printf("array2[%d] = ", i);
        scanf("%d", &array2[i]);
    };
    
    // enter value numbers into the array3
    for (int i = 1; i < narr3; i++) {
        printf("array3[%d] = ", i);
        scanf("%d", &array3[i]);
    }
    
    // Return the sum of the elements in the three arrays
    getSum(block);
    
    // Return the sum of the elements in the three arrays
    
    
    /*int sum = 0;
    for (int i = 0; i < narr1; i++) {
        sum = sum + array1[i];
    };
    for (int i = 0; i < narr2; i++) {
        sum = sum + array2[i];
    };
    for (int i = 0; i < narr3; i++) {
        sum = sum + array3[i];
    };
    
    printf("\n The sum of the elements in the three arrays = %d", sum);
    printf("\n"); */
    
    return 0;
}

void getSum(int** block){
    int sum = 0;
    
    // Loops through the block array of 3 elements; i = nr. of array
    for (int i = 0; i < 3; i++) {
        
        // Loops through any of the arrays contained in the block array; to get length of the array1, 2, 3 we need to get at the element of cell 0 of array1, 2, 3; we need to rereference block[i] (rozimenuvaty)
        for (int j = 0; j < *block[i]; j++) {
            sum = sum + block[i][j];
        }
    }
    printf("The sum = %d \n", sum);
    
}












/*{
    int *block; // указатель для блока памяти
    int n;      // число элементов массива
    // ввод числа элементов
    printf("Size of array=");
    scanf("%d", &n);
     
    // выделяем память для массива
    // функция malloc возвращает указатель типа void*
    // который автоматически преобразуется в тип int*
    block = malloc(n * sizeof(int));
     
    // вводим числа в массив
    for(int i=0;i<n; i++)
    {
        printf("block[%d]=", i);
        scanf("%d", &block[i]);
    }
    printf("\n");
     
    // вывод введенных чисел на консоль
    for(int i=0;i<n; i++)
    {
        printf("%d \t", block[i]);
    }
    printf("\n");
     
    // освобождаем память
    free(block);
    return 0;
}*/
