#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <err.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

#include "job_queue.h"

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;                  // The job_queue's mutex
pthread_cond_t queue_cond_not_full = PTHREAD_COND_INITIALIZER;            // Condition: wait until the queue is not full.
pthread_cond_t queue_cond_not_empty = PTHREAD_COND_INITIALIZER;           // Condition: wait until the queue is not empty.
int destroy_flag = 0;                                                     // Indicates whether or not the job_queue should be destroyed.

// returns 1 if there is no used nodes in the job_queue. Otherwise, returns 0.
int queue_is_empty (struct job_queue* job_queue){
  if (!job_queue->used_nodes) return 1; 
  return 0;
}

// returns 1 if the number of used nodes in the job_queue matches the capacity. Otherwise, returns 0.
int queue_is_full (struct job_queue* job_queue){
  if (job_queue->capacity == job_queue->used_nodes) return 1;
  return 0;
}

int job_queue_init(struct job_queue *job_queue, int capacity) {
  job_queue->used_nodes = 0;
  job_queue->head = NULL;
  job_queue->tail = NULL;
  job_queue->capacity = capacity;
  return 0;
}

int job_queue_destroy(struct job_queue *job_queue) {
  destroy_flag = 1;                                   
  pthread_cond_broadcast(&queue_cond_not_full);
  pthread_cond_broadcast(&queue_cond_not_empty);
  while(!queue_is_empty(job_queue));
  return 0;
}

int job_queue_push(struct job_queue* job_queue, void* data) {

  if (destroy_flag) return 1;
  
  pthread_mutex_lock(&queue_mutex);

  while (queue_is_full(job_queue)) {                                      // Blocks threads as long as the queue is full

    pthread_cond_wait(&queue_cond_not_full, &queue_mutex);

    if (destroy_flag) {                                                   // Kills all push-jobs, when the queue is set to destroy

      pthread_mutex_unlock(&queue_mutex);
      return 1;

    }
  }

  node* new_node = malloc(sizeof(node));
  assert(new_node != NULL);
  new_node->data = data;
  new_node->next = NULL;                                           
  
  if (queue_is_empty(job_queue)) {                                        // If there are no current nodes, set both head and tail to the new node

    job_queue->head = new_node;
    job_queue->tail = new_node;

  } else {                                                                // Else, set the new node as the new head, and set the old head's next pointer to the new node
    
    job_queue->tail->next = new_node;
    job_queue->tail = new_node;

  }
  job_queue->used_nodes++;
  pthread_mutex_unlock(&queue_mutex);
  pthread_cond_signal(&queue_cond_not_empty);                             // Potentially signals to a thread blocked by an empty queue, that the queue is no longer empty
  return 0;
}

int job_queue_pop(struct job_queue *job_queue, void **data) {

  pthread_mutex_lock(&queue_mutex);

  while (queue_is_empty(job_queue)) {                                     // Blocks threads while the queue is empty

    if (destroy_flag) {                                                   // Stops thread by returning -1, when queue is empty and queue is set to destroy

      pthread_mutex_unlock(&queue_mutex);
      return -1;

    }

    pthread_cond_wait(&queue_cond_not_empty, &queue_mutex);
  }
  
  node* node_to_pop = job_queue->head;                                    // Create the pointer of the node to delete and set it to the head
  *data = node_to_pop->data;                                              // The pointer to a given variable (**data points to THE given variable), in which the deleted node is saved

  if (job_queue->head == job_queue->tail) {    
      
    job_queue->head = NULL;                                               // Update the pointer of the head
    job_queue->tail = NULL;                                               // Update the pointer of the tail

  } else {

    job_queue->head = job_queue->head->next;                              // Update the pointer of the head
    
  }
  job_queue->used_nodes--;
  pthread_mutex_unlock(&queue_mutex);
  free(node_to_pop);                                                      // Delete the node (the old head)
  pthread_cond_signal(&queue_cond_not_full);                              // Sends a "GO" signal to the waiting pushing-threads so that they can add nodes since the qeue is no longer full
  return 0;
}

