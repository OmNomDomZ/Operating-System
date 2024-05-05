#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <libgen.h>
#include <unistd.h>
#include <fcntl.h>

#define SUCCESS 0
#define ERROR (-1)
#define BUFFER_MAX 256

void print_error(const char *file_path);
void reverse_string(char *dest, const char *src);
int create_reverse_dir(const char *dest_working_dir, const char *src_working_dir, const char *dir_name);
int create_reverse_file(const char *dest_working_dir, const char *src_working_dir, const char *file_name);
void reverse_file_data(FILE *src_stream, FILE *dest_stream);

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s DIRECTORY...\n", argv[0]);
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; ++i) {
        char real_path[PATH_MAX];
        if (realpath(argv[i], real_path) == NULL) {
            print_error(argv[i]);
            continue;
        }

        char *parent_path = dirname(strdup(real_path));
        char *dir_name = basename(real_path);

        if (create_reverse_dir(parent_path, parent_path, dir_name) == ERROR)
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void print_error(const char *file_path) {
    fprintf(stderr, "%s: %s\n", file_path, strerror(errno));
}

void reverse_string(char *dest, const char *src) {
    int i, len = strlen(src);
    for (i = 0; i < len; i++) {
        dest[i] = src[len - 1 - i];
    }
    dest[len] = '\0';
}

int create_reverse_file(const char *dest_working_dir, const char *src_working_dir, const char *file_name) {
    char src_file_path[PATH_MAX], dest_file_path[PATH_MAX], reversed_file_name[NAME_MAX];
    int src_fd, dest_fd;
    FILE *src_stream, *dest_stream;
    struct stat stat_buf;

    sprintf(src_file_path, "%s/%s", src_working_dir, file_name);
    reverse_string(reversed_file_name, file_name);
    sprintf(dest_file_path, "%s/%s", dest_working_dir, reversed_file_name);

    // Получаем права доступа исходного файла
    if (stat(src_file_path, &stat_buf) == -1) {
        print_error(src_file_path);
        return ERROR;
    }

    // Открытие исходного файла для чтения
    src_stream = fopen(src_file_path, "rb");
    if (src_stream == NULL) {
        print_error(src_file_path);
        return ERROR;
    }

    // Открытие целевого файла для записи, с правами как у исходного файла
    dest_fd = open(dest_file_path, O_WRONLY | O_CREAT | O_TRUNC, stat_buf.st_mode);
    if (dest_fd == -1) {
        fclose(src_stream);
        print_error(dest_file_path);
        return ERROR;
    }
    dest_stream = fdopen(dest_fd, "wb");
    if (dest_stream == NULL) {
        fclose(src_stream);
        close(dest_fd);
        print_error(dest_file_path);
        return ERROR;
    }

    reverse_file_data(src_stream, dest_stream);

    fclose(src_stream);
    fclose(dest_stream);

    return SUCCESS;
}


int create_reverse_dir(const char *dest_working_dir, const char *src_working_dir, const char *dir_name) {
    char src_dir_path[PATH_MAX];
    char dest_dir_path[PATH_MAX];
    char reversed_dir_name[NAME_MAX];
    DIR *dir;
    struct dirent *entry;

    // путь до исходного каталога
    sprintf(src_dir_path, "%s/%s", src_working_dir, dir_name);
    reverse_string(reversed_dir_name, dir_name);
    // путь до перевернутого каталога
    sprintf(dest_dir_path, "%s/%s", dest_working_dir, reversed_dir_name);

    dir = opendir(src_dir_path);
    if (dir == NULL) {
        print_error(src_dir_path);
        return ERROR;
    }

    if (mkdir(dest_dir_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == ERROR) {
        if (errno != EEXIST) {
            print_error(dest_dir_path);
            closedir(dir);
            return ERROR;
        }
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        if (entry->d_type == DT_REG) {
            if (create_reverse_file(dest_dir_path, src_dir_path, entry->d_name) == ERROR) {
                closedir(dir);
                return ERROR;
            }
        } else if (entry->d_type == DT_DIR) {
            if (create_reverse_dir(dest_dir_path, src_dir_path, entry->d_name) == ERROR) {
                closedir(dir);
                return ERROR;
            }
        }
    }

    closedir(dir);
    return SUCCESS;
}


void reverse_file_data(FILE *src_stream, FILE *dest_stream) {
    int file_size = 0;
    int count_blocks = 0;
    size_t bytes_read = 0, bytes_written = 0;
    char *buffer = NULL;

    fseek(src_stream, 0L, SEEK_END);
    file_size = ftell(src_stream);
    rewind(src_stream);

    count_blocks = file_size / BUFFER_MAX;
    if (file_size % BUFFER_MAX != 0) {
        count_blocks += 1;
    }

    buffer = (char *) calloc(BUFFER_MAX, sizeof(char));
    if (!buffer) {
        perror("Memory allocation failed");
        return;
    }

    for (int i = count_blocks; i > 0; i--) {
        fseek(src_stream, (i - 1) * BUFFER_MAX, SEEK_SET);
        bytes_read = fread(buffer, sizeof(char), BUFFER_MAX, src_stream);

        if (ferror(src_stream)) {
            fprintf(stderr, "Error reading file.\n");
            break;
        }

        // Обратная перестановка байт в буфере
        for (int j = 0; j < bytes_read / 2; j++) {
            char tmp = buffer[j];
            buffer[j] = buffer[bytes_read - j - 1];
            buffer[bytes_read - j - 1] = tmp;
        }

        bytes_written = fwrite(buffer, sizeof(char), bytes_read, dest_stream);
        if (bytes_written < bytes_read) {
            fprintf(stderr, "Error writing file.\n");
            break;
        }

        fflush(dest_stream);
    }
    free(buffer);
}
