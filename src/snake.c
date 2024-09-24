#include <snake.h>
#include <snake_context.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/**
 * Тип указателя на функцию для отклика действием на нажатие клавиши
 * **/
typedef void (*snake_action_t) (snake_context_t *snake_context);

/**
 * В этом массиве храним соответствие между кодом символа и
 * действием, которое должно за этим последовать
 * **/
static snake_action_t key_action_map[256];

/**
 * Функция проверки допустимого направления
 * **/
bool check_direction(snake_context_t *snake_context, snake_direction_t dir) {
    /**
     * Если змейка стоит или мы хотим её остановить, то это 
     * можно сделать для любого направления.
     * Если мы пытаемся установить то же самое направление,
     * в котором уже движется змейка, то это можно сделать.
     */
    if (STOP == dir || STOP == snake_context->head->dir || dir == snake_context->head->dir) return true;

    /**
     * Тип направления определён следующим образом в snake_context.h
     * typedef enum snake_direction {STOP = 0, UP = 1, RIGHT = 2, DOWN = 3, LEFT = 4} snake_direction_t;
     * Если чётность устанавливаемого направления не совпадает с чётностью текущего, то оно разрешено
     */
    if (dir % 2 != snake_context->head->dir % 2) return true;

    /* в остальных случая неверное направление */
    return false;
}

/**
 * Функции действия, которые меняют направление движение
 * змейки
 * **/
static void on_move_up(snake_context_t *snake_context) {
    if (!check_direction(snake_context, UP)) return;
    snake_context->head->dir = UP;
}

static void on_move_right(snake_context_t *snake_context) {
    if (!check_direction(snake_context, RIGHT)) return;
    snake_context->head->dir = RIGHT;
}

static void on_move_down(snake_context_t *snake_context) {
    if (!check_direction(snake_context, DOWN)) return;
    snake_context->head->dir = DOWN;
}

static void on_move_left(snake_context_t *snake_context) {
    if (!check_direction(snake_context, LEFT)) return;
    snake_context->head->dir = LEFT;
}

/**
 * Функция действия для остановки работы программы
 * **/
static void on_esc(snake_context_t *snake_context) {
    snake_context->is_quit = true;
}

/**
 * Декларация функции увеличения размера змейки,
 * может быть неудачной, так как выделяется новая память
 * **/
static int snake_grow(snake_context_t *snake_context);

/**
 * Функция действия для увеличения размера змейки
 * **/
static void on_snake_grow(snake_context_t *snake_context) {
    snake_context->errcode = snake_grow(snake_context);
}

/**
 * Функция для инициализации таблицы код-действие
 * **/
static void snake_key_action_map_init() {
    /**
     * Предварительно инициализируем все коды в NULL
     * Означает, что для этих кодов нет отображения в действие
     * **/
    for (unsigned idx = 0; idx != 256; ++idx)
        key_action_map[idx] = NULL;

    /**
     * Заполняем таблицу для тех кодов, для которых действие определено
     * **/
    // esc
    key_action_map[27] = on_esc;
    // W                 // w                  // Ц                  // ц
    key_action_map[87] = key_action_map[119] = key_action_map[150] = key_action_map[230] = on_move_up;
    // D                 // d                  // В                  // в
    key_action_map[68] = key_action_map[100] = key_action_map[130] = key_action_map[162] = on_move_right;
    // S                 // s                  // Ы                  // ы
    key_action_map[83] = key_action_map[115] = key_action_map[155] = key_action_map[235] = on_move_down;
    // A                 // a                 // Ф                   // ф 
    key_action_map[65] = key_action_map[97]  = key_action_map[148] = key_action_map[228] = on_move_left;
#ifdef _DEBUG
    // при сборке в режиме отладки можно увеличивать размер змейки нажатием клавиши без еды
    // G                 // g                 // П                   // п 
    key_action_map[71] = key_action_map[103] = key_action_map[143] = key_action_map[175] = on_snake_grow;
#endif
}

static snake_vector_t directions_vec[5] = { {0, 0}, {-1, 0}, {0, 1}, {1, 0}, {0, -1} };

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

static void move_head(snake_context_t *context_ptr) {
    snake_head_t *head = context_ptr->head;
    if (head->row_pos == 0 && directions_vec[head->dir].row < 0)
        head->row_pos = context_ptr->row_size - 1;
    else if (head->row_pos == context_ptr->row_size - 1 && directions_vec[head->dir].row > 0)
        head->row_pos = 0;
    else 
        head->row_pos += directions_vec[head->dir].row;

    if (head->col_pos == 0 && directions_vec[head->dir].col < 0)
        head->col_pos = context_ptr->col_size - 1;
    else if (head->col_pos == context_ptr->col_size - 1 && directions_vec[head->dir].col > 0)
        head->col_pos = 0;
    else
        head->col_pos += directions_vec[head->dir].col;
}

static void move_snake(snake_context_t *context_ptr) {
    snake_head_t *head = context_ptr->head;
    unsigned prev_row = head->row_pos, prev_col = head->col_pos;
    move_head(context_ptr); 
     
    snake_tail_t *tail = head->tail;
    while (NULL != tail) {
        swap_pos(&(tail->row_pos), &prev_row);
        swap_pos(&(tail->col_pos), &prev_col);
        tail = tail->tail;
    }
}

static int snake_grow(snake_context_t *context_ptr) {
    snake_tail_t *new_neck = (snake_tail_t *)malloc(sizeof(snake_tail_t));
    if (NULL == new_neck)
        return -1;

    snake_head_t *head = context_ptr->head;
    unsigned prev_row = head->row_pos, prev_col = head->col_pos;
    move_head(context_ptr);

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
        context_ptr->head->dir = STOP;
        context_ptr->head->tail = NULL;

        clear_field(context_ptr->field, context_ptr->field + row_size * col_size);
        put_snake(context_ptr->field, context_ptr->col_size, context_ptr->head);

        snake_key_action_map_init();
    }
    return context_memory;
}

void snake_delete_context(void *context) {
    delete_tail(((snake_context_t*)context)->head);
    free(context);
}

/**
 * Основная функция обработки событий
 * **/
void snake_key_process(int key, void *snake_context) {
    snake_context_t *context_ptr = (snake_context_t *)snake_context;
    if (key < 0 || key > 255 || NULL == key_action_map[key]) return;
    key_action_map[key](context_ptr);
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
    move_snake(context_ptr);
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
