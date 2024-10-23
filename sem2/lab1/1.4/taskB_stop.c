#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* increment_counter(void* arg) {
    int *counter = (int*)arg;
    while (1) {
        (*counter)++;
        // Явная точка отмены
        pthread_testcancel();  
    }
    return NULL;
}

int main() {
    pthread_t thread;
    int counter = 0;

    // Создаем поток
    if (pthread_create(&thread, NULL, increment_counter, &counter) != 0) {
        perror("Не удалось создать поток");
        return 1;
    }

    sleep(2);

    printf("Основной поток: отправка команды для остановки дочернего потока...\n");
    pthread_cancel(thread);

    if (pthread_join(thread, NULL) != 0) {
        perror("Не удалось завершить поток");
        return 1;
    }

    printf("Поток остановлен. Значение счетчика: %d\n", counter);
    return 0;
}
