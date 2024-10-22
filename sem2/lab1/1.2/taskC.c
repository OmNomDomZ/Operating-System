#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* thread_function(void* arg) {
    // Выделяем память в куче для строки
    char* message = malloc(13 * sizeof(char));
    if (message == NULL) {
        fprintf(stderr, "Ошибка выделения памяти.\n");
        pthread_exit(NULL);
    }
    strcpy(message, "hello world");
    printf("Дочерний поток возвращает строку: %s\n", message);
    pthread_exit((void*)message);
}

int main() {
    pthread_t thread_id;
    int ret;
    void* thread_return;

    // Создаем новый поток
    ret = pthread_create(&thread_id, NULL, thread_function, NULL);
    if (ret != 0) {
        fprintf(stderr, "Ошибка при создании потока.\n");
        exit(EXIT_FAILURE);
    }

    // Ожидаем завершения потока
    ret = pthread_join(thread_id, &thread_return);
    if (ret != 0) {
        fprintf(stderr, "Ошибка при ожидании завершения потока.\n");
        exit(EXIT_FAILURE);
    }

    char* returned_message = (char*)thread_return;
    printf("Основной поток получил строку: %s\n", returned_message);

    // Освобождаем выделенную память
    free(returned_message);

    return 0;
}
