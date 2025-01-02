#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// #define _pthread_cleanup_push(routine, arg) do { \
//     void (*__cleanup_routine)(void *) = (routine); \
//     void *__cleanup_arg = (arg);

// #define _pthread_cleanup_pop(execute) \
//     if (execute) __cleanup_routine(__cleanup_arg); \
// } while (0)


void cleanup(void* arg) {
    printf("Освобождение памяти: %s\n", (char*)arg);
    free(arg);
}

void* print_hello(void* arg) {
    char* message = (char*)malloc(20 * sizeof(char));
    if (message == NULL) {
        perror("Не удалось выделить память");
        return NULL;
    }
    strcpy(message, "hello world");

    pthread_cleanup_push(cleanup, message);

    while (1) {
        printf("%s\n", message);
        sleep(1); 
    }

    pthread_cleanup_pop(1);

    return NULL;
}

int main() {
    pthread_t thread;

    if (pthread_create(&thread, NULL, print_hello, NULL) != 0) {
        perror("Не удалось создать поток");
        return 1;
    }

    sleep(3);

    printf("Основной поток: остановка\n");
    pthread_cancel(thread);

    if (pthread_join(thread, NULL) != 0) {
        perror("Не удалось завершить поток");
        return 1;
    }

    printf("Поток остановлен.\n");
    return 0;
}
