#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <netdb.h>
#include <signal.h>
#include <arpa/inet.h>
#include "logger.h"

#define FAIL (-1)
#define PORT 8080
#define MAX_USERS_COUNT 15
#define BUFFER_SIZE 2048

typedef struct {
    int client_socket;
    char *request;
} context;

volatile int server_is_on = 1;
int server_socket;

// Обработчик SIGINT
void sigint_handler(int signo) {
    if (signo == SIGINT) {
        logg("Shutting down the server", BLUE);
        server_is_on = 0;
        close(server_socket); // Закрываем сокет
    }
}

int read_request_fully(int client_socket, char *request, size_t buf_size) {
    size_t total_read = 0;

    // Читаем в цикле конец заголовков HTTP
    // В HTTP заголовок заканчивается \r\n\r\n
    while (1) {
        ssize_t bytes_read = read(client_socket, request + total_read, buf_size - total_read - 1);

        if (bytes_read < 0) {
            logg("Error while reading request", RED);
            close(client_socket);
            return EXIT_FAILURE;
        }
        if (bytes_read == 0) {
            // Клиент закрыл соединение
            logg("Connection closed from client", RED);
            close(client_socket);
            return EXIT_FAILURE;
        }

        total_read += bytes_read;
        request[total_read] = '\0';

        // конец заголовков
        char *end_of_headers = strstr(request, "\r\n\r\n");
        if (end_of_headers != NULL) {
            logg("нашли конец заголовков, достаточно", YELLOW);
            // нашли конец заголовков, достаточно
            break;
        }

        // если буфер — выходим (можно расширить динамически)
        if (total_read >= buf_size - 1) {
            logg("Request too large for buffer", YELLOW);
            break;
        }
    }

    logg_char("Received request:\n", request, GREEN);
    return EXIT_SUCCESS;
}


ssize_t send_all(int socket_fd, const char *buffer, size_t length) {
    size_t total_sent = 0;
    while (total_sent < length) {
        ssize_t sent_now = send(socket_fd, buffer + total_sent,
                                length - total_sent, 0);
        if (sent_now < 0) {
            return -1; // ошибка
        }
        total_sent += sent_now;
    }

    //  все запрошенные байты ушли в сокет
    return (ssize_t)total_sent; 
}

// читает из сокета (src_fd) данные и пересылает их в другой сокет (dst_fd)
void forward_data(int src_fd, int dst_fd) {
    char buffer[BUFFER_SIZE];
    while (1) {
        ssize_t bytes_read = recv(src_fd, buffer, BUFFER_SIZE, 0);
        if (bytes_read < 0) {
            logg("Error receiving data", RED);
            break;
        }
        if (bytes_read == 0) {
            // источник закрыл соединение
            break;
        }

        // отправляем, что прочитали, в dst_fd
        size_t total_sent = 0;
        while ((ssize_t)total_sent < bytes_read) {
            ssize_t sent_now = send(dst_fd, buffer + total_sent, bytes_read - total_sent, 0);
            if (sent_now < 0) {
                logg("Error sending data", RED);
                break;
            }
            total_sent += sent_now;
        }

        // внутри цикла произошла ошибка отправки
        if (total_sent < (size_t)bytes_read) {
            break;
        }
    }
}


// создание сокета 
int create_server_socket() {
    struct sockaddr_in server_addr;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        logg("Error while creating server socket", RED);
        return FAIL;
    }

    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        logg("Error setting SO_REUSEADDR", RED);
        close(server_socket);
        return FAIL;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    logg("Server socket created", GREEN);

    // привязываем сокет к IP и порту 
    int err = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == FAIL) {
        logg("Failed to bind server socket", RED);
        close(server_socket);
        return FAIL;
    }

    logg("Server socket bound", GREEN);

    // слушаем
    err = listen(server_socket, MAX_USERS_COUNT);
    if (err == FAIL) {
        logg("Server socket failed to listen", RED);
        close(server_socket);
        return FAIL;
    }

    return server_socket;
}

// // читаем запрос
// int read_request(int client_socket, char *request) {
//     ssize_t bytes_read = read(client_socket, request, BUFFER_SIZE);
//     if (bytes_read < 0) {
//         logg("Error while reading request", RED);
//         close(client_socket);
//         return EXIT_FAILURE;
//     }
//     if (bytes_read == 0) {
//         logg("Connection closed from client", RED);
//         close(client_socket);
//         return EXIT_FAILURE;
//     }
//     request[bytes_read] = '\0';
//     logg_char("Received request:\n", request, GREEN);
//     return EXIT_SUCCESS;
// }

// подключаемся к серверу
int connect_to_remote(char *host) {
    struct addrinfo hints, *res0;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(host, "http", &hints, &res0);
    if (status != 0) {
        logg("getaddrinfo error", RED);
        return FAIL;
    }

    int dest_socket = socket(res0->ai_family, res0->ai_socktype, res0->ai_protocol);
    if (dest_socket == FAIL) {
        logg("Error while creating remote server socket", RED);
        freeaddrinfo(res0);
        return FAIL;
    }

    int err = connect(dest_socket, res0->ai_addr, res0->ai_addrlen);
    if (err == FAIL) {
        logg("Error while connecting to remote server", RED);
        close(dest_socket);
        freeaddrinfo(res0);
        return FAIL;
    }

    freeaddrinfo(res0);
    return dest_socket;
}

// Разбираем запрос чтобы узнать Host
void parse_request(char *request, char *parsed_request, char *host) {
    char *host_start = strstr(request, "Host: ");

    if (!host_start) {
        logg("Host header not found", RED);
        return;
    }
    // Пропускаем Host
    host_start += 6; 

    while (*host_start == ' ') {
        host_start++;
    }

    int i = 0;
    while (host_start[i] != '\r' && host_start[i] != '\n' && host_start[i] != '\0') {
        host[i] = host_start[i];
        i++;
    }
    host[i] = '\0';

    // Копируем весь запрос
    strcpy(parsed_request, request); 
}

// обработчик (получает запрос, проксирует его и пересылает обратно ответ)
void *client_handler(void *arg) {
    context *ctx = (context *)arg;
    int client_socket = ctx->client_socket;
    char *request = ctx->request;

    // Игнорируем HTTPS 
    if (strncmp(request, "CONNECT", 7) == 0) {
        logg("Ignoring CONNECT request", YELLOW);
        close(client_socket);
        free(ctx->request);
        free(ctx);
        pthread_exit(NULL);
    }

    // HTTP
    char parsed_request[BUFFER_SIZE];
    char host[256];
    memset(host, 0, sizeof(host));
    parse_request(request, parsed_request, host);

    logg_char("Remote server host name: ", host, GREEN);

    // подключаемся к серверу
    int dest_socket = connect_to_remote(host);
    if (dest_socket == FAIL) {
        close(client_socket);
        free(ctx->request);
        free(ctx);
        pthread_exit(NULL);
    }

    logg("Connected to remote server", PURPLE);

    // отправляем запрос
    size_t req_len = strlen(parsed_request);
    if (send_all(dest_socket, parsed_request, req_len) < 0) {
        logg("Error sending request to remote server", RED);
        close(client_socket);
        close(dest_socket);
        free(ctx->request);
        free(ctx);
        pthread_exit(NULL);
    }

    // пересылаем ответ обратно клиенту
    forward_data(dest_socket, client_socket);

    close(client_socket);
    close(dest_socket);
    free(ctx->request);
    free(ctx);
    logg("Connection with client closed", BLUE);
    pthread_exit(NULL);
}

int main() {
    logg("SERVER START", BACK_PURP);
    signal(SIGINT, sigint_handler);

    server_socket = create_server_socket();
    if (server_socket == FAIL) {
        logg("Failed to create server socket", RED);
        exit(EXIT_FAILURE);
    }

    logg_int("Server listening on port ", PORT, PURPLE);

    while (server_is_on) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(client_addr);

        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_size);
        if (!server_is_on) break;

        if (client_socket == FAIL) {
            logg("Failed to accept connection", RED);
            continue;
        }

        logg("Accepted new client", GREEN);

        char *request = calloc(BUFFER_SIZE, sizeof(char));
        if (read_request_fully(client_socket, request, BUFFER_SIZE) == EXIT_FAILURE) {
            free(request);
            continue;
        }

        context *ctx = malloc(sizeof(context));
        ctx->client_socket = client_socket;
        ctx->request = request;

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, client_handler, ctx) != 0) {
            logg("Failed to create thread", RED);
            close(client_socket);
            free(request);
            free(ctx);
        } else {
            pthread_detach(thread_id); // отделяем поток
        }
    }

    close(server_socket);
    logg("Server shut down", PURPLE);
    return 0;
}
