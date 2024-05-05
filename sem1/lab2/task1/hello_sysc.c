#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

ssize_t my_write(unsigned int fd, const char* buf, size_t count ){
	return syscall(SYS_write,fd,buf,count);
}

int main(){

	my_write(1,"Hello world!\n",13);
	return 0;
}
