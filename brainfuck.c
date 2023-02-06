#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

char *parse(const char *const buffer, const long buffersize);
char interpret(const char *const buffer, const long buffersize);

int main(const int argc, const char *const argv[])
{
    if (argc != 2) {
        perror("No file specified.\n");
        exit(EXIT_FAILURE);
    }

    char *buffer = NULL;
    long buffersize = 0;

    {
        int status;
        const char *const path = argv[1];

        FILE *file = fopen(path, "rb");
        if (!file) {
            perror("fopen() failed");
            exit(EXIT_FAILURE);
        }

        status = fseek(file, 0, SEEK_END);
        if (status != 0) {
            perror("fseek() failed");
            fclose(file);
            exit(EXIT_FAILURE);
        }

        buffersize = ftell(file);
        if (buffersize == -1L) {
            perror("ftell() failed");
            fclose(file);
            exit(EXIT_FAILURE);
        }

        rewind(file);

        // avoid overflow of size+1 in malloc
        if (buffersize == LONG_MAX) {
            perror("file too big");
            fclose(file);
            exit(EXIT_FAILURE);
        }

        buffer = (char *)malloc(buffersize + 1);
        if (!buffer) {
            perror("malloc() failed");
            fclose(file);
            exit(EXIT_FAILURE);
        }

        buffer[buffersize] = '\0';
        size_t bytesread = fread(buffer, buffersize, 1, file);
        if (bytesread != buffersize) {
            perror("fread() failed");
            free(buffer);
            fclose(file);
            exit(EXIT_FAILURE);
        }

        fclose(file);
    }

    char result = interpret(buffer, buffersize);

    free(buffer);

    return 0;
}

char *parse(const char *const buffer, const long buffersize)
{
    return NULL;
}

char interpret(const char *const buffer, const long buffersize)
{
    long memorysize = 30000;
    char *memory = (char *)calloc(memorysize, 1);
    if (!memory) {
        perror("malloc() failed");
        return (char)-1;
    }

    char *ptr = memory;
    ptrdiff_t n = memory - ptr;

    for(long i = 0; i < buffersize; ++i) {
        switch (buffer[i]) {
            case '>':
                ++ptr;
                break;
            case '<':
                --ptr;
                break;
            case '+':
                ++*ptr;
                break;
            case '-':
                --*ptr;
                break;
            case '.':
                putchar(*ptr);
                break;
            case ',':
                *ptr = getchar();
                break;
            case '[':
                // if(*ptr == 0) { jump(matching(']')) } else { ++ptr };
                break;
            case ']':
                // if(*ptr != 0) { jump(matching('[')) } else { ++ptr };
                break;
        }
    }

    return (char)0;
}
