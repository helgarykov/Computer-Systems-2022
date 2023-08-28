#include <stdlib.h>
#include "list.h"

typedef struct list_node {
  void* data;
  struct list_node* next;
  struct list_node* prev;
}node;

typedef struct list{
  node* head;
  node* tail;
}list;

struct list* list_create() {
  list* list = malloc(sizeof(struct list));
  list->head = NULL;
  list->tail = NULL;
  return list;
}

void list_free(struct list** ppList) {
  struct list* pList = *ppList;
  *ppList = NULL;


  struct list_node *node = pList->head;
  while (node != NULL) {
    struct list_node *next = node->next;
    free(node);
    node = next;
  }
  free(pList);
}

int list_insert_first(struct list* list, void* data) {
  if(list == NULL) 
    list = list_create();

  node* new_head = malloc(sizeof(node));
  new_head->data = data;
  new_head->next = list->head;

  list->head = new_head;
  if(list->tail == NULL)
    list->tail = list->head;
  return 0;
}

int list_insert_last(struct list* list, void* data) {
   if(list == NULL) 
    list = list_create();
  node* new_tail = malloc(sizeof(node));
  new_tail->data = data;
  
  list->tail->next = new_tail;
  
  new_tail->next = NULL;
  new_tail->prev = list->tail;

  list->tail = new_tail;
  if(list->head==NULL)
    list->head = list->tail;

  return 0;
} 

int list_remove_first(struct list* list, void** dest) {
  if(list == NULL || list->head == NULL)
    return 1;
  
  node* old_head = list->head;
  *dest = old_head->data;
  list->head = old_head->next;
  free(old_head);
  return 0;
}

void list_map(struct list* list, map_fn f, void *aux) {
  node* node = list->head;
  while (node != NULL) {
    node->data = f(node->data, aux);
    node = node->next;
  }
}





