#ifndef SNAKE_CONTEXT_H
#define SNAKE_CONTEXT_H

#include <stdbool.h>

typedef enum snake_cell {EMPTY = 0, HEAD, TAIL} snake_cell_t;

typedef struct snake_context {
    unsigned row_size, col_size;
    snake_cell_t *field;
    int errcode;
    bool is_paused, is_quit;
} snake_context_t;
#endif