#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <err.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t queue_not_empty = PTHREAD_COND_INITIALIZER;

int push(int* array, int val) {
    int pos = array[1];
    int size = array[0];
    pthread_mutex_lock(&mutex);
    if(pos==size){
        pthread_cond_wait(&queue_not_full, &mutex);

    }
    array[pos]=val;
        return 1;
    
    pthread_mutex_unlock(&mutex);

   
}
int pop(int* array, int val) {
    int pos = array[1];
    int size = array[0];
    pthread_mutex_lock(&mutex);
    
    array[pos]=val;
        return 1;
    
    pthread_mutex_unlock(&mutex);

    if(pos<size){
        pthread_cond_signal(&queue_not_full);

    }
}