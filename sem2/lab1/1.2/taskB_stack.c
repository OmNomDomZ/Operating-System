#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void* thread_function(void* arg) {
    // Переменная создается на стеке
    int result = 42;  
    printf("Дочерний поток возвращает число %d.\n", result);
    pthread_exit((void*)&result); 
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

    int returned_value = *((int*)thread_return);
    printf("Основной поток получил число %d от дочернего потока.\n", returned_value);

    return 0;
}
