//
//  main.c
//  ProjectC_._vs_&
//
//  Created by Helga on 04/08/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    int a, b;
} newStruct;

typedef struct
{
    newStruct* strr; // create a pointer that contains the adress of structure "newStruct"
    char* name; //create a pointer to the (1-cell array?) called name
    char* surname; //create a pointer to the (1-cell array?) called surname
    int LengthOfName, LengthOfSurname; //create int variables: nr of elements in array
} student;


int main() {

    student Tom; //create an instance of structure "student"
    Tom.strr = malloc(sizeof(newStruct)); // allocate memory to one cell of array Tom.strr
    
    //What does one cell contains? a and b only?

    Tom.strr -> a = 2; //  разименовывает указатель strr;  находит там переменную а и; assigns value of int 2 to int variable a;
    
    //What is the order of reading? From right to left or vice versa?

    student* pointer = &Tom; //создаем указатель с адресом Tom (instance of student)
    
    pointer -> strr -> a; //мы разименовываем указатель pointer, что указывает на Toma; там мы разименовываем указатель на структуру strr и; выбираем там члена структуры а и достаем значение, спрятанное в а.
    
    //Поскольку синтаксис доступа к членам структур/классов с помощью указателя несколько неудобен, то C++ предоставляет второй оператор выбора членов (->) для осуществления доступа к членам через указатель. Доступ к члену структуры осуществляется через оператор выбора члена (.) (или «оператор доступа к члену» (https://ravesli.com/urok-90-operator-dostupa-k-chlenam-cherez-ukazatel/) Идентичны:
    
    // (*pointer).weight = 60; и ptr -> weight = 60;
    
    //Обратите внимание, разыменование указателя должно находиться в круглых скобках, поскольку оператор выбора члена (.) имеет более высокий приоритет, чем оператор разыменования.
    
    //To есть (.) и -> делают одно и то же, но первый просто выбирает член структуры, а второй сначала разыменовывает указатель слева от оператора -> а потом выбирает член структуры; (.) унитарный опер с действием вправо, а -> бинарный оператор с действием сначала влево, а потом вправо.
    
    //Правило: При использовании указателя для доступа к значению члена структуры или класса используйте оператор «->» вместо оператора «.»
   
    
    return 0;
}
