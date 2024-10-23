#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void* thread_function(void* arg) {
    pthread_detach(pthread_self());  
    pthread_t tid = pthread_self();  
    printf("Отсоединенный поток с ID: %lu\n", (unsigned long)tid);
    pthread_exit(NULL);  
}

int main() {
    pthread_t thread_id;
    int ret;

    while (1) {
        // Создаем новый поток
        ret = pthread_create(&thread_id, NULL, thread_function, NULL);
        if (ret != 0) {
            fprintf(stderr, "Ошибка при создании потока.\n");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
