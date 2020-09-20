#include "stack.h"

#include <stdio.h>

bool stack_empty(Stack* stack) {
        if (stack->size == 0) {
                return true;
        }
        return false;
}

void push(Stack* stack, const int value) {
        stack->size++;
        stack->stack[stack->size - 1] = value;
}

void pop(Stack* stack) {
        if (stack->size == 0) {
                printf("Stack is empty\n");
                return;
        }
        stack->stack[stack->size - 1] = 0;
        stack->size--;
}

int top(Stack* stack) {
        if (stack->size == 0) {
                printf("Stack is empty\n");
                return 0;
        }
        return stack->stack[stack->size - 1];
}

int size(Stack* stack) {
        return stack->size;
}

void print(Stack* stack) {
        for (int i = stack->size - 1; i >= 0; --i) {
                printf("%d ", stack->stack[i]);
        }
        printf("\n");
}