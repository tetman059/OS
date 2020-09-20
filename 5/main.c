#include "stack.h"

#include <stdio.h>
#include <dlfcn.h>

void print_menu() {
        printf("Menu: \n");
        printf("1. Push an element to stack\n");
        printf("2. Pop an element from stack\n");
        printf("3. Check if stack empty\n");
        printf("4. Get the top element from stack\n");
        printf("5. Get the size of stack\n");
        printf("6. Print stack\n");
        printf("7. Exit\n");
        printf("8. Print menu\n");
}

int main() {
        Stack stack;
        stack.size = 0;
        for (int i = 0; i < 20; ++i) {
                stack.stack[i] = 0;
        }

        int n = 0;
        bool flag = true;

        print_menu();
        while (flag) {
                printf("Enter the number to select action\n");
                scanf("%d", &n);

                switch (n) {
                        case 1: {
                                int number = 0;
                                printf("Enter the number to push\n");
                                scanf("%d", &number);
                                push(&stack, number);
                                break;
                        }
                        case 2:
                                pop(&stack);
                                break;
                        case 3:
                                if (stack_empty(&stack)) {
                                        printf("Stack is empty\n");
                                } else {
                                        printf("Stack isn't empty\n");
                                }
                                break;
                        case 4:
                                printf("The top element from stack is: %d\n", top(&stack));
                                break;
                        case 5:
                                printf("The size of stack is: %d\n", size(&stack));
                                break;
                        case 6:
                                print(&stack);
                                break;
                        case 7:
                                flag = false;
                                break;
                        case 8:
                                print_menu();
                                break;
                        }
        }

        return 0;
}