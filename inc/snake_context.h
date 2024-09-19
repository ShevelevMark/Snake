#ifndef SNAKE_CONTEXT_H
#define SNAKE_CONTEXT_H

#include <stdbool.h>

typedef enum snake_cell {EMPTY = 0, HEAD, TAIL} snake_cell_t;

struct snake_tail;

typedef struct snake_head {
    unsigned row_pos, col_pos;
    int row_move, col_move;
    struct snake_tail *tail;
} snake_head_t;

typedef struct snake_tail {
    unsigned row_pos, col_pos;
    struct snake_tail *tail;
} snake_tail_t;

typedef struct snake_context {
    unsigned row_size, col_size;
    int errcode;
    bool is_paused, is_quit;
    snake_cell_t *field;
    snake_head_t *head;
} snake_context_t;

#endif