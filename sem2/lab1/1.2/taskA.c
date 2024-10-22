#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void* thread_function(void* arg) {
    printf("Дочерний поток начал работу.\n");
    printf("Дочерний поток завершает работу.\n");
    pthread_exit(NULL);
}

int main() {
    pthread_t thread_id;
    int result;

    // Создаем поток
    result = pthread_create(&thread_id, NULL, thread_function, NULL);
    if (result != 0) {
        printf("Ошибка при создании потока.\n");
        exit(EXIT_FAILURE);
    }

    // Ожидаем завершения потока
    result = pthread_join(thread_id, NULL);
    if (result != 0) {
        printf("Ошибка при ожидании завершения потока.\n");
        exit(EXIT_FAILURE);
    }

    printf("Основной поток завершил ожидание дочернего потока.\n");
    return 0;
}
