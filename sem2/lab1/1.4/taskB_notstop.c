#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// Поточная функция
void* increment_counter(void* arg) {
    int *counter = (int*)arg;
    // Бесконечный цикл увеличения счетчика
    while (1) {
        (*counter)++;
        // Поток не проверяет возможность завершения, так что pthread_cancel() может не сработать
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

    // Даем потоку поработать несколько секунд
    sleep(2);

    // Останавливаем поток с помощью pthread_cancel()
    printf("Основной поток: отправка команды для остановки дочернего потока...\n");
    pthread_cancel(thread);

    // Ожидаем завершения потока
    if (pthread_join(thread, NULL) != 0) {
        perror("Не удалось завершить поток");
        return 1;
    }

    printf("Поток остановлен. Значение счетчика: %d\n", counter);
    return 0;
}
