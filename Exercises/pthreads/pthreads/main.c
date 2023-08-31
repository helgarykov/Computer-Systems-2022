
#include<stdio.h>
#include<pthread.h>


/* Run:  pthreads % ./thread2 */
int x = 0;
void* thread_result;

void* worker(void* thread_result) {
    printf("Hello from the thread before counting\n");
    for (int i = 0; i < 1000; i++) {
        x++;
    }
    printf("Thread returns the result :%d\n ", x);
    return NULL;
}

int main() {
    pthread_t t;
    pthread_create(&t, NULL, worker, NULL);
    printf("Hello 1\n");
    pthread_join(t, &thread_result);
    printf("Helloo before 2.thread is created \n");
    pthread_create(&t, NULL, worker, NULL);
    printf("Hellooo \n");
    pthread_join(t, &thread_result);
    //printf("%d\n", x);
}
