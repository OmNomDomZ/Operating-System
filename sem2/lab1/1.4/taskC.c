#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Функция очистки для освобождения памяти
void cleanup(void* arg) {
    printf("Освобождение памяти: %s\n", (char*)arg);
    free(arg);  // Освобождаем память
}

// Поточная функция
void* print_hello(void* arg) {
    // Выделяем память под строку
    char* message = (char*)malloc(20 * sizeof(char));
    if (message == NULL) {
        perror("Не удалось выделить память");
        return NULL;
    }
    strcpy(message, "hello world");

    // Регистрируем функцию очистки
    pthread_cleanup_push(cleanup, message);

    // Бесконечный цикл для печати строки
    while (1) {
        printf("%s\n", message);
        sleep(1);  // Небольшая задержка
    }

    // Обязательно используйте pop с ненулевым аргументом, чтобы очистка выполнилась
    pthread_cleanup_pop(1);

    return NULL;
}

int main() {
    pthread_t thread;

    // Создаем поток
    if (pthread_create(&thread, NULL, print_hello, NULL) != 0) {
        perror("Не удалось создать поток");
        return 1;
    }

    // Даем потоку поработать несколько секунд
    sleep(3);

    // Останавливаем поток с помощью pthread_cancel()
    printf("Основной поток: остановка\n");
    pthread_cancel(thread);

    // Ожидаем завершения потока
    if (pthread_join(thread, NULL) != 0) {
        perror("Не удалось завершить поток");
        return 1;
    }

    printf("Поток остановлен.\n");
    return 0;
}
