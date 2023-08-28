#include <stdlib.h>
#include "list.h"

typedef struct list_node {
  void *data;
  struct list_node* next;
}node;

typedef struct list {
   node* head;
}list;

struct list* list_create() {
  list* list = malloc(sizeof(list));
  list->head = NULL;
  return list;
}

void list_free(list* list) {
  node* delete_node = list->head;
  while (delete_node != NULL) {
    node* next = delete_node->next;
    free(delete_node);
    delete_node = next;
  }
  free(list);
}

int list_insert_first(list* list, void* data) {
  node* new_head = malloc(sizeof(node));
  new_head->data = data;
  new_head->next = list->head;

  list->head = new_head;
  return 0;
}

int list_insert_last(list* list, void* data) {

  node* new_tail = malloc(sizeof(node));
  new_tail->data = data;
  new_tail->next = NULL;

  if(list->head == NULL) 
    list->head = new_tail;
    
  node* iterator = list->head;
  while(iterator->next != NULL){
    iterator = iterator->next;
  }
  iterator->next = new_tail;
  
  return 0;
} 


int list_remove_first(list* list, void** dest) {
  node *old_head = list->head;
  if (old_head == NULL) 
    return 1;
  
  *dest = old_head->data;
  list->head = old_head->next;
  free(old_head);
  return 0;
}

void list_map(list* list, map_fn f, void *aux) {
  node *node = list->head;
  while (node != NULL) {
    node->data = f(node->data, aux);
    node = node->next;
  }
}
