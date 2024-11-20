// структура потока
typedef struct {
    int cancel_requested; // флаг отмены
    int cancel_enabled;   // разрешена ли отмена
    int cancel_type;      // тип отмены
    ...
} pthread_t;

// deferred
int pthread_cancel(pthread_t thread) {
    // становим флаг отмены
    thread->cancel_requested = 1; 
    if (thread->cancel_type == PTHREAD_CANCEL_ASYNCHRONOUS) {
        pthread_kill(thread, SIGCANCEL);
    }
    return 0;
}

void pthread_testcancel() {
    pthread_t self = pthread_self();
    if (self->cancel_requested && self->cancel_enabled) {
        // обработчики 
        pthread_exit(PTHREAD_CANCELED);
    }
}



// asynchronous
int pthread_cancel(pthread_t thread) {
    thread->cancel_requested = 1;
    if (thread->cancel_type == PTHREAD_CANCEL_ASYNCHRONOUS) {
        pthread_kill(thread, SIGCANCEL); 
    }
    return 0;
}

// целевой поток обрабатывает SIGCANCEL
void __cancel_handler(int sig) {
    pthread_t self = pthread_self();
    if (self->cancel_requested) {
        // обработчики 
        pthread_exit(PTHREAD_CANCELED);
    }
}

