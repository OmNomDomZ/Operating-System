#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* print_lines(void* arg) {
    while (1) {
        printf("Поток выполняется...\n");
        // sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t thread;

    // Создаем поток
    if (pthread_create(&thread, NULL, print_lines, NULL) != 0) {
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

    return 0;
}
