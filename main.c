#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "commands.h"
#include "terminal.h"

/* Terminal parameters */
extern struct termios tdef;
extern struct termios tmod;

/* Terminal size */
extern tsize_t tsize;

/* Limited commands set */
extern int restricted;

/* File information */
char **filenames;
int current;
int total;
int fd;
int filearg;
int didpipe;
struct stat info;

/* Main logic */
int rows;
direction_t direction;
result_t result;

void parse_opt(char *option);
void more(char *filename);

int main(int argc, char **argv) {
    int i, istty;

    /* Process stdin */
    istty = isatty(STDIN_FILENO);
    if (!istty) {
        filenames[current++] = "-";
        total++;
    }

    /* Process command line */
    for (i = 1; i < argc; i++) {
        switch (argv[i][0]) {
            case '-':
                if (strlen(argv[i]) != 1) {
                    parse_opt(argv[i]);
                    break;
                }
                else
            default:
                filearg = i;
                i = argc;
                break;
        }
    }

    /* Copy filenames */
    for (i = filearg; i < argc; i++, current++, total++) {
        filenames[current] = malloc(sizeof(argv[i]));
        memcpy(&filenames[current], argv[i], sizeof(argv[i]));
    }

    /* No input files */
    if (total == 0) print("Missing input file(s)\n");

    /* Just cat the file(s) if standard */
    /* output is not a terminal */
    istty = isatty(STDOUT_FILENO);
    if (!istty) execvp("cat", filenames);

    /* Save terminal attributes */
    tcgetattr(STDOUT_FILENO, &tdef);
    tcgetattr(STDOUT_FILENO, &tmod);

    /* TODO */
    tmod.c_cc[VMIN] = 1;
    tmod.c_cc[VTIME] = 0;

    /* Set terminal flags */
    tsetflag(DISABLE, ICANON);
    tsetflag(DISABLE, ECHO);

    /* Get terminal size */
    tgetsize();

    /* Allocate output buffer */
    bufferalloc(tsize.rows - 1, tsize.cols);

    /* Process files in a sequence */
    for (; current < total; current++) {
        more(filenames[current]);
    }

    /* Restore terminal attributes */
    tcsetattr(STDOUT_FILENO, TCSANOW, &tdef);

    return EXIT_SUCCESS;
}

void parse_opt(char *option) {
    if (strlen(option) > 2)
        printf("more: illegal option -- %s\n", option);
    else {
        switch (option[1]) {

        }
    }
}

void more(char *filename) {
    restricted = 1;

    /* Process standard input */
    if (!strcmp(filename, "-")) {
        if (didpipe) {
            prompt(DIDPIPE);
            return;
        }
        else {
            fd = STDIN_FILENO;
            didpipe++;
        }
    }
    else {
        if ((fd = open(filename, O_RDONLY)) != -1) {
            /* Determine file type */
            fstat(fd, &info);
            if (!S_ISREG(info.st_mode) && !S_ISFIFO(info.st_mode)) {
                prompt(WTYPE);
                return;
            }

            /* Disable scroll up for FIFO */
            if (S_ISREG(info.st_mode)) restricted = 0;
        }
        else {
            prompt(ERROR);
            return;
        }
    }

    /* Main logic */
    rows = tsize.rows - 1;
    direction = DOWN;

    for (;;) {
        result = scroll(rows, direction);

        switch (result) {
            case E0F:
                if (total > 1 && current + 1 < total) prompt(result);
                return;
            case CONTINUE:
                prompt(result);
                break;
        }
    }
}
