#ifndef MORE_TERMINAL_H
#define MORE_TERMINAL_H

typedef struct {
    int rows;
    int cols;
} tsize_t;

typedef enum { ENABLE, DISABLE } state_t;

void tsetflag(state_t state, int flag);
void tgetsize();
void tcleareol();
void tclearscr();

#endif //MORE_TERMINAL_H
