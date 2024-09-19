#include <snake.h>
#include <snake_context.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

static void clear_field(snake_cell_t *begin, snake_cell_t *end) {
    for (snake_cell_t *it = begin; it != end; ++it)
        *it = EMPTY;
}

static void put_snake(snake_cell_t *field, unsigned col_size, snake_head_t *head) {
    if (NULL == head) return;
    field[head->row_pos * col_size + head->col_pos] = HEAD;
    snake_tail_t *tail = head->tail;
    while (NULL != tail) {
        field[tail->row_pos * col_size + tail->col_pos] = TAIL;
        tail = tail->tail;
    }
}

static void swap_pos(unsigned *a, unsigned *b) {
    unsigned tmp = *a;
    *a = *b;
    *b = tmp;
}

static void move_snake(snake_head_t *head) {
    unsigned prev_row = head->row_pos, prev_col = head->col_pos;
    head->row_pos += head->row_move; head->col_pos += head->col_move;
    snake_tail_t *tail = head->tail;

    while (NULL != tail) {
        swap_pos(&(tail->row_pos), &prev_row);
        swap_pos(&(tail->col_pos), &prev_col);
        tail = tail->tail;
    }
}

static int snake_grow(snake_head_t *head) {
    snake_tail_t *new_neck = (snake_tail_t *)malloc(sizeof(snake_tail_t));
    if (NULL == new_neck)
        return -1;

    unsigned prev_row = head->row_pos, prev_col = head->col_pos;
    head->row_pos += head->row_move; head->col_pos += head->col_move;
    new_neck->tail = head->tail;
    head->tail = new_neck;
    new_neck->row_pos = prev_row;
    new_neck->col_pos = prev_col;

    return 0;
}

static void delete_tail(snake_head_t *head) {
    snake_tail_t *tail = head->tail;
    while (NULL != tail) {
        snake_tail_t *tmp = tail->tail;
        free(tail);
        tail = tmp;
    }
}

void* snake_make_context(unsigned row_size, unsigned col_size) {
    void* context_memory = malloc(sizeof(snake_context_t) + sizeof(snake_head_t) + sizeof(snake_cell_t) * row_size * col_size);
    if (NULL != context_memory) {
        snake_context_t *context_ptr = (snake_context_t *)context_memory;
        context_ptr->row_size   = row_size;
        context_ptr->col_size   = col_size;
        context_ptr->errcode    = 0;
        context_ptr->is_paused  = false;
        context_ptr->is_quit    = false;
        context_ptr->head       = (snake_head_t *)((unsigned char*)context_memory + sizeof(snake_context_t));
        context_ptr->field      = (snake_cell_t *)((unsigned char*)context_memory + sizeof(snake_context_t) + sizeof(snake_head_t));

        context_ptr->head->col_pos = context_ptr->col_size / 2;
        context_ptr->head->row_pos = context_ptr->row_size / 2;
        context_ptr->head->col_move = 1;
        context_ptr->head->row_move = 0;
        context_ptr->head->tail = NULL;

        clear_field(context_ptr->field, context_ptr->field + row_size * col_size);
        put_snake(context_ptr->field, context_ptr->col_size, context_ptr->head);
    }
    return context_memory;
}

void snake_delete_context(void *context) {
    delete_tail(((snake_context_t*)context)->head);
    free(context);
}

void snake_key_process(int key, void *snake_context) {
    snake_context_t *context_ptr = (snake_context_t *)snake_context;
    switch(key) {
        case 27: { // esc
            context_ptr->is_quit = true;
            break;
        }
#ifdef _DEBUG
        case 71:  // G
        case 103: // g
        case 143: // П 
        case 175: // п 
        {
            snake_grow(context_ptr->head);
            break;
        }    
#endif
        // UP
        case 87:  // W
        case 119: // w
        case 150: // ц 
        case 230: // Ц 
        {
            context_ptr->head->row_move = -1;
            context_ptr->head->col_move = 0;
            break;
        }

        // RIGHT
        case 68:  // D
        case 100: // d
        case 130: // В
        case 162: // в
        {
            context_ptr->head->row_move = 0;
            context_ptr->head->col_move = 1;
            break;
        }

        // DOWN
        case 83:  // S
        case 115: // s
        case 155: // Ы
        case 235: // ы
        {
            context_ptr->head->row_move = 1;
            context_ptr->head->col_move = 0;
            break;
        }

        // LEFT
        case 65:  // A
        case 97:  // a
        case 148: // Ф
        case 228: // ф
        {
            context_ptr->head->row_move = 0;
            context_ptr->head->col_move = -1;
            break;
        }
    }
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
    snake_context_t *context_ptr = (snake_context_t *)snake_context;
    move_snake(context_ptr->head);
    clear_field(context_ptr->field, context_ptr->field + context_ptr->row_size * context_ptr->col_size);
    put_snake(context_ptr->field, context_ptr->col_size, context_ptr->head); 
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
