#ifndef SNAKE_CONTEXT_H
#define SNAKE_CONTEXT_H

#include <stdbool.h>

typedef enum snake_cell {EMPTY = 0, HEAD, TAIL, FOOD} snake_cell_t;
typedef enum snake_direction {STOP = 0, UP = 1, RIGHT = 2, DOWN = 3, LEFT = 4} snake_direction_t;

typedef struct snake_vector {
    int row, col;
} snake_vector_t;

struct snake_tail;

typedef struct snake_head {
    unsigned row_pos, col_pos;
    snake_direction_t dir;
    struct snake_tail *tail;
    bool is_ai;
} snake_head_t;

typedef struct snake_tail {
    unsigned row_pos, col_pos;
    struct snake_tail *tail;
} snake_tail_t;

typedef struct snake_food {
    unsigned row, col;
} snake_food_t;

typedef struct snake_context {
    int color_scheme[4]; // 4 цвета для четырёх типов ячеек на поле: 0-EMPTY, 1-HEAD, 2-TAIL, 3-FOOD
    unsigned row_size, col_size;
    int errcode;
    bool is_paused, is_quit;
    snake_cell_t *field;
    snake_head_t *head;
    snake_food_t *food;
    double last_st, speed, level_up;
    unsigned level;
} snake_context_t;

#endif