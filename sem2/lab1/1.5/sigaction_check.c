#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
void handler1(int sig) {
    printf("Thread 1 received signal: %d\n", sig);
}

void handler2(int sig) {
    printf("Thread 2 received signal: %d\n", sig);
}

void *thread_func1(void *arg) {
    struct sigaction sa;
    sa.sa_handler = handler1;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL); // Устанавливаем handler1 для SIGUSR1
    sleep(3); 
    return NULL;
}

void *thread_func2(void *arg) {
    struct sigaction sa;
    sa.sa_handler = handler2;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL); // Устанавливаем handler2 для SIGUSR1
    sleep(3);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_func1, NULL);
    pthread_create(&t2, NULL, thread_func2, NULL);

    // Отправляем сигнал конкретному потоку
    sleep(1);
    pthread_kill(t1, SIGUSR1);  // Ожидается вызов handler1
    sleep(1);
    pthread_kill(t2, SIGUSR1);  // Ожидается вызов handler2

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}

