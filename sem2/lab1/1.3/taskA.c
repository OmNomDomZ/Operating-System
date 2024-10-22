#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int number; 
    char* message;
} ThreadData;

void* thread_function(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    printf("Число: %d\n", data->number);
    printf("Сообщение: %s\n", data->message);

    pthread_exit(NULL);  
}

int main(){
    pthread_t thread_id;
    int err;

    ThreadData data;
    data.number = 1;
    data.message = "Hello";

    err = pthread_create(&thread_id, NULL, thread_function, &data);
    if (err != 0){
        fprintf(stderr, "Ошибка при создании потока.\n");
        exit(EXIT_FAILURE);
    }

    err = pthread_join(thread_id, NULL);
    if (err != 0) {
        fprintf(stderr, "Ошибка при ожидании завершения потока.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}