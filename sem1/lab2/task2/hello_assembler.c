#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/syscall.h>


void print_hello()
{
    uint64_t syscall_num = SYS_write;
    uint64_t fd = STDOUT_FILENO; // файловый дескриптор, STDOUT_FILENO -- стандартный вывод
    const char buf[] = "Hello, world!\n";
    size_t size = sizeof(buf) - 1;

    ssize_t return_value; // возвращаемое значение системного вызова

    asm volatile (
        "movq %1, %%rax\n\t" // syscall_num
        "movq %2, %%rdi\n\t" // fd
        "movq %3, %%rsi\n\t" // buf
        "movq %4, %%rdx\n\t" // size
        "syscall\n\t" // системный вызов
        "movq %%rax, %0" // сохраняем результат сисколла из rax в переменную return_value
        : "=r"(return_value) // выходной операнд, значение помещенное в rax должно быть сохранено в return_value
        : "r"(syscall_num), "r"(fd), "r"(buf), "r"(size) // входные операнды которые нужно загрузить перед выполнением инструкций
        : "%rax", "%rdi", "%rsi", "%rdx" // список регистров, которые инлайн-ассемблер использует и должен сохранить
    );

    perror("SYS_write");
}

int main(void)
{
    print_hello();
}
