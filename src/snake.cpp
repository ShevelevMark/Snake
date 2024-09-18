#include <snake.h>
#include <snake_context.h>
#include <stdlib.h>
#include <stdio.h>

void* snake_make_context(unsigned row_size, unsigned col_size) {
    void* context_memory = malloc(sizeof(snake_context_t) + sizeof(snake_cell_t) * row_size * col_size);
    if (NULL != context_memory) {
        snake_context_t *context_ptr = (snake_context_t *)context_memory;
        context_ptr->row_size   = row_size;
        context_ptr->col_size   = col_size;
        context_ptr->field      = (snake_cell_t *)(context_ptr + 1);
        context_ptr->errcode    = 0;
        context_ptr->is_paused  = false;
        context_ptr->is_quit     = false;

        for (unsigned row_idx = 0; row_idx != context_ptr->row_size; ++row_idx) {
            for (unsigned col_idx = 0; col_idx != context_ptr->col_size; ++col_idx) {
                context_ptr->field[row_idx * context_ptr->col_size + col_idx] = EMPTY;
            }
        }
        context_ptr->field[context_ptr->row_size / 2 * context_ptr->col_size + context_ptr->col_size / 2] = HEAD;
    }
    return context_memory;
}

void snake_delete_context(void *context) {
    free(context);
}

void snake_key_process(int key, void *snake_context) {
    snake_context_t *context_ptr = (snake_context_t *)snake_context;
    if (27 == key)
        context_ptr->is_quit = true;
}

void snake_draw(void *snake_context) {
    snake_context_t *context_ptr = (snake_context_t *)snake_context;
    for (unsigned row_idx = 0; row_idx != context_ptr->row_size; ++row_idx) {
        for (unsigned col_idx = 0; col_idx != context_ptr->col_size; ++col_idx) {
            switch(context_ptr->field[row_idx * context_ptr->col_size + col_idx]) {
                case EMPTY: printf("."); break;
                case HEAD: printf("@"); break;
                case TAIL: printf("*"); break;
            }
            printf(" ");
        }
        printf("\n");
    }
}

void snake_advance(void *snake_context) {

}

int snake_context_error(void *snake_context) {
    snake_context_t *context_ptr = (snake_context_t *)snake_context;
    return context_ptr->errcode;
}

bool snake_quit(void *snake_context) {
    snake_context_t *context_ptr = (snake_context_t *)snake_context;
    return context_ptr->is_quit;
}

bool snake_paused(void *snake_context) {
    snake_context_t *context_ptr = (snake_context_t *)snake_context;
    return context_ptr->is_paused;
}
