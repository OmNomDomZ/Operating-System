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
#define BUFF_SIZE 256

char file_name[NAME_MAX] = {};

char *get_file_name(const char *file_path);

void create_dir(const char *dir_path);

void print_dir(const char *dir_path);

void remove_dir(const char *dir_path);

void create_file(const char *file_name);

void print_file(const char *file_path);

void remove_file(const char *file_path);

void create_symlink(const char *file_path);

void print_symlink(const char *symlink_path);

void print_content_of_symlink_target(const char *symlink_path);

void remove_symlink(const char *symlink_path);

void create_hardlink(const char *file_path);

void remove_hardlink(const char *hardlink_path);

void print_permissions_and_number_of_hardlinks(const char *file_path);

void change_permissions(const char *file_path, const char *permissions);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        puts("Too few args");
        return EXIT_FAILURE;
    }

    if (strcmp(argv[0], "./cdir.out") == SUCCESS) {
        create_dir(argv[1]);
    } else if (strcmp(argv[0], "./pdir.out") == SUCCESS) {
        print_dir(argv[1]);
    } else if (strcmp(argv[0], "./rdir.out") == SUCCESS) {
        remove_dir(argv[1]);
    } else if (strcmp(argv[0], "./cfile.out") == SUCCESS) {
        create_file(argv[1]);
    } else if (strcmp(argv[0], "./pfile.out") == SUCCESS) {
        print_file(argv[1]);
    } else if (strcmp(argv[0], "./rfile.out") == SUCCESS) {
        remove_file(argv[1]);
    } else if (strcmp(argv[0], "./csym.out") == SUCCESS) {
        create_symlink(argv[1]);
    } else if (strcmp(argv[0], "./psym.out") == SUCCESS) {
        print_symlink(argv[1]);
    } else if (strcmp(argv[0], "./psymtarget.out") == SUCCESS) {
        print_content_of_symlink_target(argv[1]);
    } else if (strcmp(argv[0], "./rsym.out") == SUCCESS) {
        remove_symlink(argv[1]);
    } else if (strcmp(argv[0], "./chard.out") == SUCCESS) {
        create_hardlink(argv[1]);
    } else if (strcmp(argv[0], "./rhard.out") == SUCCESS) {
        remove_hardlink(argv[1]);
    } else if (strcmp(argv[0], "./pperm.out") == SUCCESS) {
        print_permissions_and_number_of_hardlinks(argv[1]);
    } else if (strcmp(argv[0], "./chperm.out") == SUCCESS) {
        if (argc < 3) {
            puts("To change permissions enter permissions value\n");
        } else {
            change_permissions(argv[1], argv[2]);
        }
    } else {
        puts("Unknown command");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

char *get_file_name(const char *file_path) {
    char working_dir[PATH_MAX] = {};
    const char *last_slash = NULL;

    last_slash = strrchr(file_path, '/');
    if (last_slash != NULL) {
        strncpy(working_dir, file_path, last_slash - file_path + 1);
        strcpy(file_name, last_slash + 1);
    } else {
        strcpy(working_dir, "./");
        strcpy(file_name, file_path);
    }

    return file_name;
}

void create_dir(const char *dir_path) {
    if (mkdir(dir_path, 0777) == ERROR) {
        puts("Error while creating directory");
        perror("mkdir");
        exit(EXIT_FAILURE);
    }
}

void print_dir(const char *dir_path) {
    struct dirent *entry = NULL;
    DIR *dir = NULL;

    dir = opendir(dir_path);
    if (dir == NULL) {
        perror("diropen");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        printf("%lu - %s type:%d %d\n", entry->d_ino, entry->d_name, entry->d_type, entry->d_reclen);
    }
}

void remove_dir(const char *dir_path) {
    if (remove(dir_path) == ERROR) {
        puts("Error while removing directory");
        perror("rmdir");
        exit(1);
    }
}

void create_file(const char *file_name) {
    FILE *file = NULL;

    file = fopen(file_name, "wb+");
    if (file == NULL) {
        puts("Error while creating file");
        perror("create_file");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

void print_file(const char *file_path) {
    FILE *file = NULL;
    char *buffer = NULL;

    file = fopen(file_path, "r");
    if (file == NULL) {
        puts("Error while opening file");
        perror("open_file");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    buffer = (char *) calloc(sizeof(char), BUFF_SIZE);
    while (fread(buffer, sizeof(char), BUFF_SIZE, file) > 0) {
        puts(buffer);
    }

    fflush(file);
    fclose(file);

    free(buffer);
}

void remove_file(const char *file_path) {
    if (remove(file_path) == ERROR) {
        puts("Error while removing file");
        perror("rm_file");
        exit(EXIT_FAILURE);
    }
}


void create_symlink(const char *file_path) {
    if (symlink(file_path, strcat(get_file_name(file_path), "_symlink")) == ERROR) {
        puts("Error while creating symlink");
        perror("cr_symlink");
        exit(EXIT_FAILURE);
    }
}

void print_symlink(const char *symlink_path) {
    char *buffer = NULL;

    buffer = (char *) calloc(sizeof(char), BUFF_SIZE);
    if (readlink(symlink_path, buffer, BUFF_SIZE) == ERROR) {
        puts("Error while reading symlink");
        perror("read_symlink");
        exit(EXIT_FAILURE);
    }

    puts(buffer);
    free(buffer);
}

void print_content_of_symlink_target(const char *symlink_path) {
    char *read_buffer = NULL;
    char *print_buffer = NULL;
    FILE *file = NULL;

    read_buffer = (char *) calloc(sizeof(char), BUFF_SIZE);
    if (readlink(symlink_path, read_buffer, BUFF_SIZE) == ERROR) {
        puts("Error while reading symlink");
        perror("read_symlink");
        exit(EXIT_FAILURE);
    }

    file = fopen(read_buffer, "r");
    if (file == NULL) {
        puts("Error while opening file");
        perror("open_file");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    print_buffer = (char *) calloc(sizeof(char), BUFF_SIZE);
    while (fread(print_buffer, sizeof(char), BUFF_SIZE, file) != 0) {
        puts(print_buffer);
    }

    fclose(file);

    free(read_buffer);
    free(print_buffer);
}

void remove_symlink(const char *symlink_path) {
    if (remove(symlink_path) == ERROR) {
        puts("Error while removing symlink");
        perror("rm_symlink");
        exit(EXIT_FAILURE);
    }
}

void create_hardlink(const char *file_path) {
    if (link(file_path, strcat(get_file_name(file_path), "_hardlink")) == ERROR) {
        puts("Error while creating hardlink");
        perror("cr_hardlink");
        exit(EXIT_FAILURE);
    }
}

void remove_hardlink(const char *hardlink_path) {
    if (remove(hardlink_path) == ERROR) {
        puts("Error while removing hardlink");
        perror("rm_hardlink");
        exit(EXIT_FAILURE);
    }
}

void print_permissions_and_number_of_hardlinks(const char *file_path) {
    struct stat fileStat;

    if (stat(file_path, &fileStat) == ERROR) {
        perror("Error while making stat file");
        perror("make_stat");
        exit(EXIT_FAILURE);
    }
    printf("Count of hardlinks: %lu\n", fileStat.st_nlink);

    printf("Permissions: ");
    printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n");
}

void change_permissions(const char *file_path, const char *permissions) {
    int num;
    char *ptr;

    num = (int) strtol(permissions, &ptr, 8);

    if (chmod(file_path, num) == ERROR) {
        perror("Error changing file permissions");
        exit(EXIT_FAILURE);
    }
}
