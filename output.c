#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "output.h"
#include "terminal.h"
#include "commands.h"

/* Terminal size*/
extern tsize_t tsize;

/* File information */
extern char *filenames[256];
extern int fd;
extern int current;
extern int total;

/* Limited commands set */
int restricted;

/* Output buffer */
char *buffer;
uint32_t buffidx;

/* Main logic */
int rows;
direction_t direction;

/* Cursor */
off_t cursor;

void reverse(char *str);
void print_error(char *str);
void print_prompt(char *str);

result_t scroll(int rows, direction_t direction) {
    int64_t r;
    int row = 0;
    int cols = 0;

    /* Save current cursor */
    if (direction == DOWN) cursor = lseek(fd, 0, SEEK_CUR);

    /* Move cursor home */
    if (direction == UP) lseek(fd, cursor + 1, SEEK_SET), rows++;

    buffidx = 0;
    while (row < rows) {
        /* Move cursor backwards */
        if (direction == UP) {
            if (lseek(fd, -2, SEEK_CUR) == -1) {
                /* Upper EOF */
                lseek(fd, 0, SEEK_SET);
                r = 1;
                break;
            }
        }

        /* Process character */
        if ((r = read(fd, &buffer[buffidx], 1)) == 1) {
            switch (buffer[buffidx++]) {
                case '\t':
                    cols += cols % 8;
                    break;
                case '\n':
                    cols = tsize.cols;
                    break;
                default:
                    cols++;
                    break;
            }

            /* Next row */
            if (cols >= tsize.cols) {
                cols %= tsize.cols;
                row++;
            }

            if (row != 0 && (row % (tsize.rows - 1)) == 0 && row < rows) {
                if (direction == DOWN) flush();
                else buffidx = 0;
                cursor = lseek(fd, 0, SEEK_CUR);
            }
        }
        else break;
    }

    if (r >= 0) {
        if (direction == DOWN) flush();
        else scroll(tsize.rows - 1, DOWN);
        return r ? CONTINUE : E0F;
    }
    else return ERROR;
}

void flush() {
    write(STDOUT_FILENO, buffer, buffidx);
    buffidx = 0;
}

void prompt(result_t result) {
    char str[256];

    /* Prepare message */
    switch (result) {
        case DIDPIPE:
            sprintf(str, "Can view standard input only once");
            print_error(str);
            break;
        case ERROR:
            sprintf(str, "%s: %s", filenames[current], strerror(errno));
            print_error(str);
            break;
        case WTYPE:
            sprintf(str, "%s is not a regular file or FIFO", filenames[current]);
            print_error(str);
            break;
        case E0F:
            sprintf(str, "%s: END (next file: %s)", filenames[current], filenames[current+1]);
            print_prompt(str);
            break;
        case CONTINUE:
            if (total == 1)
                sprintf(str, "%s", filenames[current]);
            else
                sprintf(str, "%s (file %d of %d)", filenames[current], current, total);
            print_prompt(str);
            break;
    }
}

void reverse(char *str) {
    /* Enable formatted output */
    print("\033[7m");

    /* Write message */
    print(str);

    /* Disable formatted output */
    print(" \033[0m");
}

void print_error(char *str) {
    int i;
    for (i = 0; i < tsize.rows - 1; i++) write(STDOUT_FILENO, "~\n", 2);
    reverse(str);
    restricted++;
    read_command(&rows, &direction);
    restricted--;
    tcleareol();
}

void print_prompt(char *str) {
    reverse(str);
    read_command(&rows, &direction);
    tcleareol();
}

void bufferalloc(int rows, int cols) {
    buffer = realloc(buffer, rows * cols);
}

void print(char *str) {
    write(STDOUT_FILENO, str, strlen(str));
}

