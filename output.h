#ifndef MORE_HEADER_FILE_H
#define MORE_HEADER_FILE_H

typedef enum { UP, DOWN } direction_t;
typedef enum { DIDPIPE, ERROR, CONTINUE, E0F, WTYPE } result_t;

result_t scroll(int, direction_t);
void bufferalloc(int rows, int cols);
void prompt(result_t result);
void flush();
void print(char *str);

#endif //MORE_HEADER_FILE_H
