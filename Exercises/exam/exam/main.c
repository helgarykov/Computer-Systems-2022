//
//  main.c
//  exam
//
//  Created by Helga on 25/01/2023.
//

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int x = 1;

void* thread(void* arg) {
    x = 2;
    printf("%d\n", x);
    return NULL;
}

int main() {
    printf("%d\n", x);
    pthread_t tid;
    pthread_create(&tid, NULL, thread, NULL);
    fork();
    printf("%d\n", x);
    return 0;
}
