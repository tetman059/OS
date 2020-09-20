all: main main_so

main: stack.so main.c
        gcc main.c -L. stack.so -o main

main_so: stack.so main_so.c
        gcc main_so.c -ldl -o main_so

stack.so: stack.o
        gcc -shared stack.o -o stack.so

stack.o: stack.c
        gcc -fPIC -c stack.c

clean:
        rm -rf *.so
        rm -rf *.o