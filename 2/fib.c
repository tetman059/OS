#include <sys/types.h>          // библиотека с типами данных size_t
#include <sys/wait.h>           // библиотека с waitpid()
#include <unistd.h>             // библиотека с системными вызовами fork(), pipe(), read(), write(), close()
#include <errno.h>              // бибоиотека с perror()
#include <stdlib.h>
#include <string.h>

int fd_1[2];                    // массив для pipe(), дескриптор 0 - чтение, 1 - запись

int Fibonacci(int n) {
        int result, n_1, n_2 = 0;
        int status = 0;         // переменная для ожидания завершения процесса
        ssize_t wr = 0;         // переменная для проверки записи в поток

        if (n == 0) {
                return 0;
        } else if (n < 2) {
                return n;
        }

        pid_t pid, ppid;        // id процесса-родителя и ребенка
        pid = fork();
        if (pid < 0) {
                perror("Can't create fork\n");
                exit(-1);
        } else if (pid == 0) {  // процесс-ребенок
                n_1 = Fibonacci(n - 1);
                close(fd_1[0]);
                wr = write(fd_1[1], &n_1, sizeof(n_1));
                if (wr < 0) {
                        perror("Can't write in pipe\n");
                }
                close(fd_1[1]);
                exit(0);
        } else {                // процесс-родитель
                n_2 = Fibonacci(n - 2);
        }
        // ожидаем завершения процессов и сохраняем все в буффер
        if (waitpid(pid, &status, 0) < 0) {
                perror("Error in waitpid\n");
        }
        if (read(fd_1[0], &n_1, sizeof(n_1)) < 0) {
                perror("Can't read from pipe\n");
        }
        result = n_1 + n_2;
        return result;
}

void reverse(char s[]) {
        int i, j;
        char c;
        for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
                c = s[i];
                s[i] = s[j];
                s[j] = c;
        }
}

void itoa(int n, char s[]) {
        int i, sign;
        i = 0;
        do {                            // генерируем цифры в обратном порядке
                s[i] = n % 10 + '0';    // берем следующую цифру
                ++i;
        } while ((n /= 10) > 0);        // удаляем
        s[i] = '\0';
        reverse(s);
}

int main() {
        int n, num_len = 0;     // номер числа в последовательности и его длина
        char read_sym[10];      // массив, содержащий считываемое число
        write(1, "Enter the number: ", sizeof("Enter the number: ")); // 1 - это стандартный поток вывода

        if (pipe(fd_1) < 0) {
                perror("Can't create pipe\n");
                exit(-1);
        }

        num_len = read(2, &read_sym, 10); // 2 - стандартный поток ввода
        read_sym[num_len - 1] = '\0';
        n = atoi(read_sym);             // преобразуем считанную строку в int
        if (n <= 0) {
                write(1, "Number must be > 0\n", sizeof("Number must be > 0\n"));
        } else {
                n = Fibonacci(n);
                char result[10];
                for (int i = 0; i < 10; ++i) {
                        result[i] = ' ';
                }
                itoa(n, result);
                write(1, result, sizeof(result));
                write(1, "\n", sizeof("\n"));
        }
        return 0;
}