#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void* thread_function(void* arg) {
    pthread_t tid = pthread_self();  // Получаем идентификатор потока
    printf("Отсоединенный поток (через атрибут) с ID: %lu\n", (unsigned long)tid);
    pthread_exit(NULL);  // Завершаем поток
}

int main() {
    pthread_t thread_id;
    pthread_attr_t attr;
    int ret;

    // Инициализируем атрибуты потока
    pthread_attr_init(&attr);
    // Устанавливаем атрибуты потока для создания detached потока
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    while (1) {
        // Создаем новый поток с установленными атрибутами
        ret = pthread_create(&thread_id, &attr, thread_function, NULL);
        if (ret != 0) {
            fprintf(stderr, "Ошибка при создании потока.\n");
            exit(EXIT_FAILURE);
        }

        // Основной поток не ждет завершения дочернего потока
    }

    // Уничтожаем атрибуты потока
    pthread_attr_destroy(&attr);

    return 0;
}
