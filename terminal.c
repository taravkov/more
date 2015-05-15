#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include "terminal.h"
#include "output.h"

/* Terminal parameters */
struct termios tdef;
struct termios tmod;

/* Terminal size */
tsize_t tsize;

void tsetflag(state_t state, int flag) {
    switch (state) {
        case ENABLE:
            tmod.c_lflag |= flag;
            break;
        case DISABLE:
            tmod.c_lflag &= ~flag;
            break;
    }
    tcsetattr(STDOUT_FILENO, TCSANOW, &tmod);
}

void tgetsize() {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != -1) {
        tsize.rows = ws.ws_row;
        tsize.cols = ws.ws_col;
    }
    else {
        perror("ioctl");
        exit(1);
    }
}

void tcleareol() {
    print("\r\033[K");
}

void tclearscr() {
    print("\033[2J");
}

