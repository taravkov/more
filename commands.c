#include <unistd.h>
#include <stdlib.h>
#include "commands.h"
#include "terminal.h"

/* Limited commands set */
int restricted;

extern tsize_t tsize;
extern result_t result;

void parse_arg(char *cmd, int *arg);

int read_command(int *rows, direction_t *direction) {
    char cmd;
    int arg = 0;

    /* Read first char of command */
    read(STDOUT_FILENO, &cmd, 1);

    /* Parse decimal argument */
    if (cmd >= '1' && cmd <= '9') parse_arg(&cmd, &arg);

    switch (cmd) {
        case ' ':
            *rows = arg ? arg : tsize.rows - 1;
            *direction = DOWN;
            break;
        case '\n':
            *rows = arg ? arg : 1;
            *direction = DOWN;
            break;
        case 'b':
            if (!restricted) {
                *rows = arg ? arg : tsize.rows - 1;
                *direction = UP;
                break;
            }
            else
        default:
            write(STDOUT_FILENO, "\a", 1);
            *rows = 0;
            *direction = DOWN;
            break;
    }
}

void parse_arg(char *cmd, int *arg) {
    char decimal[256];
    int pos = 0;
    decimal[pos++] = *cmd;

    tcleareol();
    write(STDOUT_FILENO, ":", 1);
    write(STDOUT_FILENO, cmd, 1);

    while (1) {
        read(STDOUT_FILENO, cmd, 1);
        if (*cmd >= '0' && *cmd <= '9') {
            write(STDOUT_FILENO, cmd, 1);
            decimal[pos++] = *cmd;
        }
        else if (*cmd == '\b' || *cmd == 127) {
            if (pos-- > 1) write(STDOUT_FILENO, "\b \b", 3);
            else {
                prompt(result);
                break;
            }
        }
        else break;
    }
    decimal[pos] = '\0';
    *arg = atoi(decimal);
}
