//
//  main.c
//  CProject_Cyclic_Lists
//
//
//  Created by Helga on 15/08/2022.
//

/*
 Create a cyclical journal of messages of a fixed size N.
 Make a function that saves the message and the time of its receival.
 If the journal is full, the new message should replace the oldest one.
 Make a function that prints the last K messages (K<=N).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Data saved in a node : text message and time of its receive */
typedef struct message{
    
    char message[20];                                   //a static array of 20 chars
    struct tm* timeinfo;                                //a pointer to the structure timeinfor in lib:time.h
    int num;                                            //serial number of the node in the linked list
}message;


/* A linked list node */
typedef struct nodee{
    
    struct nodee* next;
    struct nodee* prev;
    message message;
                                           
}node;


/* A linked list */
typedef struct {
    node* head;
    node* tail;
    node* newestMessage;                                //pointer to the newest message in the journal
    int maxN;                                           //the maximal number of messages in the journal
}list;


void startJournal(void);                                //function declaration (prototype)


/* Function that deals only with linked lists and adds nodes to them */
void addNode(list* list, node* prevNode, message value){
    
    node* newNode=malloc(sizeof(node));
    newNode->message=value;
    
    // Add to an empty list
    if (list->head==NULL) {
        
        //List Head and TAIL will become a new node.
        list->head=newNode;
        list->tail=newNode;
        
        //From New Node to Old Nodes
        newNode->next=list->head;                        //Since, it is circular linked list head(and here tail) will point to head (here itself).
        newNode->prev=list->tail;
        
        //From Old Nodes to New Node
        list->tail->next=newNode;
        list->head->prev=newNode;
        
       
    }
    
    // Add a new HEAD
    else if (prevNode==NULL) {
        
        //From New Node to Old Nodes
        newNode->next=list->head;                       //New Node will point to head
        newNode->prev=list->tail;
        
        //New Node will become new HEAD
        list->head=newNode;
        
        //Update TAIL & HEAD: list tail will point to head,
        //list head prev will point to tail
        list->tail->next=list->head;
        list->head->prev=list->tail;
    }
    
    // Add a new TAIL
    else if (list->tail==prevNode) {
        
        //From NewNode to Old Nodes
        newNode->prev=list->tail;
        newNode->next=list->tail->next;
        
        //From Old Nodes to New Node
        list->tail->next=newNode;                   //Tail will point to new node
        list->head->prev=newNode;
        
        //New node will become new tail.
        list->tail=newNode;
    }
    
    //Add a node between old nodes
    else {
        
        //From New Node to Old Nodes
        newNode->next=prevNode->next;
        
        //From Old Nodes to New Node
        prevNode->next=newNode;
        prevNode->next->prev=newNode;
    }
}

/* Function to print all elements of a list */
void showList(list* list)
{
    
    printf("\nShow List\n");
    // If list is empty
    if (list->tail == NULL)
        printf("\nList is empty\n");
  
    // Else print the list
    else {
        node* iterator = list->tail->next;
  
        // While first node is not
        // reached again, print,
        // since the list is circular
        do {
            printf("\nMessage Text/Time : %s  %02d:%02d:%02d\t ", iterator->message.message, iterator->message.timeinfo->tm_hour, iterator->message.timeinfo->tm_min, iterator->message.timeinfo->tm_sec );                                                                                                                                                                 //Step 1: prints the head (here list->tail->next=list->head)
            iterator = iterator->next;                                                                      //Step 2: goes to list->tail->next->next
        } while (iterator != list->tail->next);                                                             //Step 3: only now evaluates if iterator!=list->head
        printf("\n");
    }
}

/* Function to print the last *INT AMOUT* messages saved in the journal */
void showJournal(list* list, int amount){
    
    int i=0;
    printf("\nShow Journal\n");
    
    if (list->newestMessage!=NULL) {
        node* iterator=list->newestMessage;
        
        while (i<amount) {
            printf("\nMessage Text/Time : %s  %02d:%02d:%02d\t ", iterator->message.message, iterator->message.timeinfo->tm_hour, iterator->message.timeinfo->tm_min, iterator->message.timeinfo->tm_sec );
            i++;
            iterator=iterator->prev;                                                                        //direction and movement
        }
    }
}


/* Intermediate fun between addNode() and startJournal().
Saves the incoming message (array of 20 chars) in a *structure* that
adds to the text the real time and the serial nr of the incoming message. */
void saveMessage(list* list, char message[20] ){
    
    time_t rawtime;                                                                                    //create an instance of time
    time( &rawtime );
    struct tm* timeinfo = localtime( &rawtime );
    
    //Initialise a new message structure
    struct message newMessage = {};                                     //create an instance of the structure message: contains a triple -- a string, real time and serial nr of the received message
    strcpy(newMessage.message, message);                                //copy every char from message array to newMessage array
    newMessage.timeinfo=timeinfo;
    newMessage.num=1;
    
    if (list->newestMessage!=NULL) {
        
        newMessage.num=list->newestMessage->message.num + 1;                                           //the number of the old message + 1 = the number of the new message
        
        if (list->newestMessage->message.num < list->maxN) {                                           //the serial nr of the last message is less than the limit
            addNode(list, list->tail, newMessage);
            list->newestMessage=list->tail;                                                            //update the pointer to the newest message
            
            printf("\nMessage Text/Time : %s  %02d:%02d:%02d\t %d\t", newMessage.message, newMessage.timeinfo->tm_hour, newMessage.timeinfo->tm_min, newMessage.timeinfo->tm_sec, newMessage.num );
        } else{
            
            list->newestMessage->next->message=newMessage;                                              //aka list->tail->next (head)->message (head's content) HERE: rewrite the contents of node 0 (head) by simply updating the pointers
            list->newestMessage= list->newestMessage->next;                                             //update the pointer newestMessage that now points at list->head because list->newestMessage->next=list->tail->next=list->head in a cyclic linked list
            
            printf("\nMessage Text/Time : %s  %02d:%02d:%02d\t %d\t", newMessage.message, newMessage.timeinfo->tm_hour, newMessage.timeinfo->tm_min, newMessage.timeinfo->tm_sec, newMessage.num );
            
        }
        
    } else {
        
        addNode(list, NULL, newMessage);                                                                //the list is empty (tail/newestMessage points at NULL)
        
        printf("\nMessage Text/Time : %s  %02d:%02d:%02d\t %d\t", newMessage.message, newMessage.timeinfo->tm_hour, newMessage.timeinfo->tm_min, newMessage.timeinfo->tm_sec, newMessage.num );
        
        list->newestMessage=list->head;                                                                 //update the pointer newestMessage: newestMessage is the HEAD
    }

}

/* Function to take the input from the user */
void startJournal(void){
    
    //Initialise a journal
    list journal;
    journal.head = NULL;
    journal.tail = NULL;
    journal.newestMessage = NULL;
  
    
    //Input data
    printf("Welcome. Enter the number of messages to be saved in the journal:\t");                      //prompts the user to define the limit of messages to be saved in the journal
    scanf("%d", &journal.maxN);
    
    //Enter messages. Can be done via scanf.
    char message[20] = "Before Hello";                                                                  //initialise an non-dynamic array of 20 chars (h:12 chars + '\0')
    saveMessage(&journal, message);  // {B, e, f,'\0'};

    strcpy(message, "Hello");
    saveMessage(&journal, message);
    
    strcpy(message, "After Hello");
    saveMessage(&journal, message);
    
    strcpy(message, "After After Hello");
    saveMessage(&journal, message);
    
    showList(&journal);
    
    strcpy(message, "Hello & Goodbye");
    saveMessage(&journal, message);
    
    showList(&journal);
    
    strcpy(message, "Goodbye");
    saveMessage(&journal, message);
    
    showList(&journal);
    
    printf("\n");
    showJournal(&journal, 3);
}

int main() {
    
    startJournal();
}
