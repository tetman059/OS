#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <string.h>

int radix_count(int n) {
        if (n / 10 == 0) {
                return 1;
        } else {
                int count = 0;
                while (n > 0) {
                        n = n / 10;
                        count += 1;
                }
                return count;
        }
}

void reverse(char s[]) {
        int i, j;
        char c;
        for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
                c = s[i];
                s[i] = s[j];
                s[j] = c;
        }
}

void itoa(int n, char s[]) {
        int i, sign;
        if ((sign = n) < 0) {
                n = -n;
        }
        i = 0;
        do {
                s[i++] = n % 10 + '0';
        } while ((n /= 10) > 0);
        if (sign < 0) {
                s[i++] = '-';
        }
        s[i] = '\0';
        reverse(s);
}

void Write(char* path, int n) {
        char str[radix_count(n)];
        itoa(n ,str);
        int fd = open(path, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
        if (fd == -1) {
                perror("Error opening the file for writing");
                exit(EXIT_FAILURE);
        }

        size_t str_size = strlen(str);
        if (lseek(fd, str_size, SEEK_SET) == -1) {
                close(fd);
                perror("Error while calling lseek()");
                exit(EXIT_FAILURE);
        }

    if (write(fd, "", 1) == -1) {
                close(fd);
                perror("Error writing last byte of the file");
                exit(EXIT_FAILURE);
        }

        char* map = mmap(NULL, str_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (map == MAP_FAILED) {
                close(fd);
                perror("Error while mapping the file (WRITE)");
                exit(EXIT_FAILURE);
        }

        for (size_t i = 0; i < str_size; ++i) {
                map[i] = str[i];
        }

        if (msync(map, str_size, MS_SYNC) == -1) {
                close(fd);
                perror("Error while unmapping the file (WRITE)");
                exit(EXIT_FAILURE);
        }
        close(fd);
}

int Read(char* path) {
        int fd = open(path, O_RDONLY, 0600);
        if (fd == -1) {
                perror("Error opening the file for reading");
                exit(EXIT_FAILURE);
        }

        struct stat info = {0};
        if (fstat(fd, &info) == -1) {
                perror("Error getting the file size");
                exit(EXIT_FAILURE);
        }

        if (info.st_size == 0) {
                perror("The file is empty");
                exit(EXIT_FAILURE);
        }

        char* map = mmap(NULL, info.st_size, PROT_READ, MAP_SHARED, fd, 0);
        if (map == MAP_FAILED) {
                close(fd);
                perror("Error while mapping the file (READ)");
                exit(EXIT_FAILURE);
        }

        int result = atoi(map);

        if (munmap(map, info.st_size) == -1) {
                close(fd);
                perror("Error while unmapping the file (READ)");
                exit(EXIT_FAILURE);
        }
        close(fd);
        return result;
}

int sum(int n) {
        if (n > 0) {
                int result = 0, tmp = n - 1;
                Write("in", tmp);
                pid_t p = fork();
                if (p < 0) {
                        printf("Can't create fork\n");
                        exit(-1);
                } else if (p > 0) {
                        wait(NULL);
                        result = Read("out");
                        return result + n;
                } else {
                        int num = Read("in");
                        num = sum(num);
                        Write("out", num);
                        exit(0);
                }
        } else {
                return n;
        }
}

int main() {
        int n = 0;
        printf("Enter the number to calculate: \n");
        scanf("%d", &n);
        if (n > 0) {
                n = sum(n);
        } else {
                printf("Number must be greater than zero\n");
                return 0;
        }
        printf("Result: %d\n", n);
        return 0;
}