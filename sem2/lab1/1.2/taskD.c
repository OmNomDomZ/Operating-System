#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void* thread_function(void* arg) {
    pthread_t tid = pthread_self();  // Получаем идентификатор потока
    printf("Поток с ID: %lu\n", (unsigned long)tid);
    pthread_exit(NULL);  // Завершаем поток
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

        // Ожидаем завершения созданного потока
        // ret = pthread_join(thread_id, NULL);
        // if (ret != 0) {
        //     fprintf(stderr, "Ошибка при ожидании завершения потока.\n");
        //     exit(EXIT_FAILURE);
        // }
    }

    return 0;
}
