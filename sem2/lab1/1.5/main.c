#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

// Обработчик для SIGINT
void handler(int sig) {
    printf("Second thread received SIGINT: %d\n", sig);
}

// Первый поток: Блокирует все сигналы
void *thread1(void *arg) {
    sigset_t mask;
    // Заполняем маску всеми сигналами
    sigfillset(&mask);  
    // Блокируем все сигналы
    pthread_sigmask(SIG_BLOCK, &mask, NULL);  
    printf("First thread blocking all signals\n");

    sleep(10);
    printf("First thread has finished its work\n");
    return NULL;
}

// Второй поток: Устанавливает обработчик для SIGINT
void *thread2(void *arg) {
    struct sigaction sa;
    // Устанавливаем обработчик
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    // Назначаем обработчик для SIGINT
    sigaction(SIGINT, &sa, NULL);
    printf("Second thread waiting for SIGINT...\n");

    sleep(10);
    return NULL;
}

// Третий поток: Ожидает сигнал SIGQUIT с помощью sigwait
void *thread3(void *arg) {
    sigset_t mask;
    int sig;

    // Очищаем маску
    sigemptyset(&mask);  
    // Добавляем SIGQUIT в маску
    sigaddset(&mask, SIGQUIT);  
    // Блокируем SIGQUIT, чтобы использовать sigwait
    pthread_sigmask(SIG_BLOCK, &mask, NULL);  

    printf("Third thread is waiting for SIGQUIT...\n");
    // Ожидаем SIGQUIT
    sigwait(&mask, &sig);  

    printf("Third thread received SIGQUIT signal: %d\n", sig);
    return NULL;
}

int main() {
    pthread_t tid1, tid2, tid3;
    sigset_t mask;

    printf("Main thread PID: %d\n", getpid());

    // блокируем SIGQUIT в main потоке
    sigemptyset(&mask);
    sigaddset(&mask, SIGQUIT);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);

    // Создаём потоки
    pthread_create(&tid1, NULL, thread1, NULL);
    pthread_create(&tid2, NULL, thread2, NULL);
    pthread_create(&tid3, NULL, thread3, NULL);

    
    sleep(2);

    // Отправляем SIGINT второму потоку
    // pthread_kill(tid2, SIGINT);  
    sleep(1);
    // Отправляем SIGQUIT третьему потоку
    // pthread_kill(tid3, SIGQUIT);  

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);

    return 0;
}
