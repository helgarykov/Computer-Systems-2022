//
//  main.c
//  ProjectC_Pointers
//
//  Created by Helga on 03/08/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

///<summary> Write a struct node и list.
///Write Add function that adds elements to a linked list.<summary>

typedef struct nodee
{
    struct nodee* prev; // член структуры node: указатель с именем previous адресного типа
    struct nodee* next; // указатель содержит адрес of next node
    int value;          // челен структуры интового типа с назнавием value
} node;                // имя структуры для использования в других структурах или функциях

typedef struct{
    node* head;   //член структуры list; указатель head содержит адрес структуры node
    node* tail;   // node* — то же, что int/double... Это @структура адресного типа@ тип
}list;


//Добавляет node в структуру list (берет адреса структуры node и list, и int значение)
void add(node* leftNode, int value, list* list){
    
    printf("%d \n",value);
    
    //creating new node
    node* newNode = malloc(sizeof(node)); // выделяем динамич память новому экземпляру структуры node; фактически malloc возвращает адрес 1й ячейки массива; t.e. newNode это фактически массив, содержащий одну ячейку
    
    // Это то же что мы создаем новый объект типа структура node, newNode, и на этот объект указывает указатель newNode (кладем его адрес в указатель под названием newNode)
    
    newNode->value=value; // присваеваем значение value к члену value, созданного нового экземпляра структуры node
    
    
   
    // 1) setting up conections FROM new node TO existing ones;
    //applies to adding an element to an empty list
    newNode->prev=leftNode; //сначала присваивается адрес объекта leftNode указателю previous, члену объекта newNode
    if(leftNode!=NULL){
        newNode->next=leftNode->next; //указателю next(oбъекта newNode) мы присваеваем адрес объекта leftNode->next, т.е. адрес этого следующего объекта справа от leftNode
    }else{
        newNode->next=list->head; //leftNode не существует; тогда адрес объекта newNode(a иммено его члену next) приравнивается к адресу heada  в структуре list
    }
    
    
    
    // 2) setting up conections FROM existing ones TO new node; list is NOT empty
    
    //2.a. from RightNode's prev is set to the adress of NewNode
    if(leftNode!=NULL){
        if(leftNode->next!=NULL){
            leftNode->next->prev=newNode;
        }
    }else{ // line 113 is executed
        if(list->head!=NULL){
            list->head->prev=newNode;
        }
    }
    
    //2.b. from LeftNode to NewNode
    if(leftNode!=NULL){ // line 112 executed
        leftNode->next=newNode;
    }
    
    
    // 3) setting up conections FROM head/tail pointers TO the list (Line 111)
    if(list->head == NULL && list->tail == NULL){
        list->head=newNode;
        list->tail=newNode;
    }
    if(list->head==newNode->next){ //если head является следующим объектом к newNode, т.е. newNode идет перед headом
        list->head=newNode; // to newNode станотся новым head-ом
    }
    if(list->tail==newNode->prev){ //tail является предыдущим объектом к newNode
        list->tail=newNode;; // to newNode станотся новым tail-ом
    }
    
}

void showList(list* list){
    
    node* iterator=list->head;
    while(iterator!=NULL){
        
        printf("%d \t ",iterator->value);
        
        iterator=iterator->next;
    }
    printf("\n");
    
    iterator = list->tail;
    while (iterator!=NULL) {
        
        printf("%d \t ", iterator->value);
        
        iterator = iterator->prev;
    }
}


int main() {
    list ourList;
    ourList.head = NULL; //list is initiated empty
    ourList.tail = NULL; //list is initiated empty
    add(ourList.head, 1, &ourList);//1 to empty
    add(ourList.head, 2, &ourList);//1 2 new tail
    add(NULL, 3, &ourList);//3 1 2 new head
    add(ourList.tail, 4, &ourList);//3 1 2 4 new tail
    add(ourList.head->next, 5, &ourList);//3 1 5 2 4 new node
    showList(&ourList);
}

