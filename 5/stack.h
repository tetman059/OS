#include <stdbool.h>

typedef struct Stack {
        int stack[20];
        int size;
} Stack;

bool stack_empty(Stack* stack);
void push(Stack* stack, const int value);
void pop(Stack* stack);
int top(Stack* stack);
int size(Stack* stack);
void print(Stack* stack);