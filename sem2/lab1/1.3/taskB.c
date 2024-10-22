#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

// Определение структуры
typedef struct {
    int number;
    char *message;
} ThreadData;

// Поточная функция
void *mythread(void *arg) {
    // Преобразуем указатель arg к типу указателя на нашу структуру
    ThreadData *data = (ThreadData *)arg;

    // Вывод содержимого структуры
    printf("Thread: number = %d, message = %s\n", data->number, data->message);

    // Освобождаем выделенную для структуры память
    free(data);

    return NULL;
}

int main() {
    pthread_t thread;
    ThreadData *data;

    // Выделяем память для структуры на куче
    data = (ThreadData *)malloc(sizeof(ThreadData));
    if (data == NULL) {
        printf("Failed to allocate memory for thread data.\n");
        return -1;
    }

    // Инициализация структуры
    data->number = 1;
    data->message = "Hello from the detached thread!";

    // Атрибуты потока
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);  // Устанавливаем состояние "detached"

    // Создаем detached-поток, передаем указатель на структуру
    int err = pthread_create(&thread, &attr, mythread, (void *)data);
    if (err != 0) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        free(data); // Освобождаем память, если поток не был создан
        return -1;
    }

    // Основной поток не вызывает pthread_join, так как поток отсоединен
    // Поток завершится сам и освободит выделенную память

    // Уничтожаем атрибуты
    pthread_attr_destroy(&attr);

    sleep(1);


    return 0;
}
