#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <unistd.h>


void* snake_make_context(unsigned row_size, unsigned col_size);
void snake_delete_context(void *snake_context);

void snake_key_process(int key, void *snake_context);
void snake_draw(void *snake_context);
void snake_advance(void *snake_context);
int snake_context_error(void *snake_context);
bool snake_quit(void *snake_context);
bool snake_paused(void *snake_context);

int main() {
    void *snake_context = snake_make_context(20u, 20u);
    if (NULL == snake_context) {
        printf("Can't start the game\n");
        return 1;
    }

    unsigned frame_cnt = 0;
    while (!snake_quit(snake_context)) {
#ifdef _DEBUG
        printf("Frame %u\n", ++frame_cnt);
#endif
        if (kbhit()) {
            int key = getch();
#ifdef _DEBUG
            printf("key code = %d\n", key);
#endif
            snake_key_process(key, snake_context);
        }
        
        if (!snake_paused(snake_context))
            snake_advance(snake_context);

        sleep(1);
        system("cls");
        
        snake_draw(snake_context);
    }

    snake_delete_context(snake_context);
    return 0;
}

typedef enum snake_cell {EMPTY = 0, HEAD, TAIL} snake_cell_t;

typedef struct snake_context {
    unsigned row_size, col_size;
    snake_cell_t *field;
    int errcode;
    bool is_paused, is_quit;
} snake_context_t;

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


