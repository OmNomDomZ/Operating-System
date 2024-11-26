#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

typedef struct {
    int number;
    char *message;
} ThreadData;

void *mythread(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    printf("Thread: number = %d, message = %s\n", data->number, data->message);

    free(data);
    return NULL;
}

int main() {
    pthread_t thread;
    ThreadData *data;

    data = (ThreadData *)malloc(sizeof(ThreadData));
    if (data == NULL) {
        printf("Failed to allocate memory for thread data.\n");
        return -1;
    }

    data->number = 1;
    data->message = "Hello from detached thread!";

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);  

    int err = pthread_create(&thread, &attr, mythread, data);
    if (err != 0) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        free(data); 
        return -1;
    }

    pthread_attr_destroy(&attr);

    sleep(1);

    return 0;
}
