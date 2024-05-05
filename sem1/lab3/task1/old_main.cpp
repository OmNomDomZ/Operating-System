#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define SUCCESS 0
#define ERROR (-1)
#define BUFFER_MAX 256

void print_error(const char *file_path);
void reverse_string(char *dest, const char *src);
int create_reverse_dir(const char *working_dir, const char *dir_name);
int create_reverse_file(const char *dest_working_dir, const char *src_working_dir, const char *file_name);
void reverse_file_data(FILE *src_stream, FILE *dest_stream);

int main(int argc, char **argv) {
    char working_dir[PATH_MAX] = {};
    char dir_name[NAME_MAX] = {};
    char *last_slash = NULL;

    if (argc < 2) {
        printf("Usage: %s DIRECTORY...\n", argv[0]);
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; ++i) {
        // последнее вхождение '/'
        last_slash = strrchr(argv[i], '/');
        if (last_slash != NULL) {
            strncpy(working_dir, argv[i], last_slash - argv[i] + 1);
            strcpy(dir_name, last_slash + 1);
        } else {
            strcpy(working_dir, "./");
            strcpy(dir_name, argv[i]);
        }

        if (create_reverse_dir(working_dir, dir_name) == ERROR)
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void print_error(const char *file_path) {
    fprintf(stderr, "%s : %s\n", file_path, strerror(errno));
}

void reverse_string(char *dest, const char *src) {
        char temp;
        char *begin = NULL;
        char *end = NULL;

        strcpy(dest, src);
        begin = dest;
        end = dest + strlen(dest) - 1;

        while (end > begin) {
            temp = *end;
            *end = *begin;
            *begin = temp;
            ++begin;
            --end;
        }
}

int create_reverse_dir(const char *working_dir, const char *dir_name) {
    char dir_path[PATH_MAX] = {};
    char reverse_dir_name[NAME_MAX] = {};
    char reverse_dir_path[PATH_MAX] = {};
    DIR *dir = NULL;
    struct dirent *entry = NULL;

    sprintf(dir_path, "%s/%s", working_dir, dir_name);
    reverse_string(reverse_dir_name, dir_name);
    sprintf(reverse_dir_path, "%s/%s", working_dir, reverse_dir_name);

    dir = opendir(dir_path);
    if (dir == NULL) {
        print_error(dir_path);
        return ERROR;
    }

    if (mkdir(reverse_dir_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == ERROR) {
        if (errno != EEXIST) {
            print_error(reverse_dir_path);
            closedir(dir);
            return ERROR;
        }
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        if (entry->d_type == DT_REG) {  // Regular file
            if (create_reverse_file(dir_path, reverse_dir_path, entry->d_name) == ERROR) {
                closedir(dir);
                return ERROR;
            }
        } else if (entry->d_type == DT_DIR) {  // Directory
            // Recursively call create_reverse_dir for the subdirectory
            char new_src_dir[PATH_MAX];
            sprintf(new_src_dir, "%s/%s", dir_path, entry->d_name);
            if (create_reverse_dir(dir_path, entry->d_name) == ERROR) {
                closedir(dir);
                return ERROR;
            }
        }
    }

    closedir(dir);
    return SUCCESS;
}


int create_reverse_file(const char *dest_working_dir, const char *src_working_dir, const char *file_name) {
    char file_path[PATH_MAX] = {};
    char reverse_file_path[PATH_MAX] = {};
    char reverse_file_name[NAME_MAX] = {};
    FILE *src_stream = NULL;
    FILE *dest_stream = NULL;

    sprintf(file_path, "%s/%s", dest_working_dir, file_name);

    reverse_string(reverse_file_name, file_name);
    sprintf(reverse_file_path, "%s/%s", src_working_dir, reverse_file_name);

    src_stream = fopen(file_path, "rb");
    if (src_stream == NULL) {
        print_error(file_path);
        return ERROR;
    }

    dest_stream = fopen(reverse_file_path, "wb");
    if (dest_stream == NULL) {
        fclose(src_stream);
        print_error(reverse_file_path);
        return ERROR;
    }

    reverse_file_data(src_stream,dest_stream);

    fclose(src_stream);
    fclose(dest_stream);

    return SUCCESS;
}

void reverse_file_data(FILE *src_stream, FILE *dest_stream) {
    int file_size  = 0;
    int count_blocks = 0;
    unsigned int bytes_read = 0;
    char *buffer = NULL;

    // указатель чтения файла в конец файла
    fseek(src_stream, 0L, SEEK_END);
    file_size = (int) ftell(src_stream);
    // указатель вернули в начало
    rewind(src_stream);

    // кол-во блоков данных в файла размера BUFFER_MAX
    count_blocks = file_size / BUFFER_MAX;
    if (file_size % BUFFER_MAX != 0) {
        count_blocks += 1;
    }

    buffer = (char *) calloc(sizeof(char), BUFFER_MAX);
    for (int i = count_blocks; i > 0; i--) {
        // позиция чтения в начало текущего блока данных
        fseek(src_stream, (i - 1) * BUFFER_MAX, SEEK_SET);
        bytes_read = fread(buffer, sizeof(char), BUFFER_MAX, src_stream);

        // обмен данных
        for (int j = 0; j < bytes_read / 2; j++) {
            char tmp = buffer[j];
            buffer[j] = buffer[bytes_read - j - 1];
            buffer[bytes_read - j - 1] = tmp;
        }

        fwrite(buffer,sizeof(char),bytes_read,dest_stream);
        fflush(dest_stream);
    }
    free(buffer);
}
