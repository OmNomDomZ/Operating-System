all: hello

hello: libhello.a
	gcc main.c  -L. -lhello -o main

libhello.a: hello.o
	ar -rcs libhello.a hello.o

hello.o: hello.c
	gcc -c hello.c

clean:
	rm -f *.a *.o main
