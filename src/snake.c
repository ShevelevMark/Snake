#include <snake.h>
#include <snake_context.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <Windows.h>

static snake_vector_t directions_vec[5] = { {0, 0}, {-1, 0}, {0, 1}, {1, 0}, {0, -1} };

/**
 * У змейки появилось несколько новых параметров, которые нужны для
 * выполнения домашнего задания: 
 * snake_context_t::level    : unsigned - уровень змейки (равен количеству съеденной еды)
 * snake_context_t::speed    : double   - текущая скорость змейки (через каждый 1./speed секунд змейка смещается на один квадратик)
 * snake_context_t::level_up : double   - множитель скорости (при увеличении уровня speed *= level_up)
 * 
 * Начальная скорость и коэффициент ускорения задаются при построении змейки snake_make_context
 * **/

/**
 * Задание 1.
 * Функция печати уровня
 * Эта функция вызывается автоматически каждый раз при 
 * отрисовке змейки.
 * **/
void snake_print_level(void *snake_context) {
    snake_context_t *context_ptr = (snake_context_t*)snake_context;
    printf("Current level %u\n", context_ptr->level);
}

/**
 * Задание 1.
 * Функция печати финального уровня.
 * Она вызывается в функции main при успешном завершении 
 * основного цикла.
 * **/
void snake_print_exit(void *snake_context) {
    snake_context_t *context_ptr = (snake_context_t*)snake_context;
    printf("Final result %u\n", context_ptr->level);
}

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
     * Если у змейки есть только голова или мы хотим её остановить,
     * разрешим в ей остановиться или изменить направление на любое.
     */
    if (STOP == dir || NULL == snake_context->head->tail) return true;

    /**
     * Вычислим направление от хвоста к голове, не прибегая к привидению 
     * беззнакового типа к знаковому. Полученное направление запрещено.
     * **/
    int row_dir = 0, col_dir = 0;
    if (snake_context->head->row_pos > snake_context->head->tail->row_pos) row_dir = -1;
    if (snake_context->head->row_pos < snake_context->head->tail->row_pos) row_dir = 1;
    if (snake_context->head->col_pos > snake_context->head->tail->col_pos) col_dir = -1;
    if (snake_context->head->col_pos < snake_context->head->tail->col_pos) col_dir = 1;

    return !(row_dir == directions_vec[dir].row && col_dir == directions_vec[dir].col);
}

/**
 * Функции действия, которые меняют направление движение
 * змейки
 * **/
static void on_move_up(snake_context_t *snake_context) {
    if (!check_direction(snake_context, UP) || snake_context->head->is_ai) return;
    snake_context->head->dir = UP;
}

static void on_move_right(snake_context_t *snake_context) {
    if (!check_direction(snake_context, RIGHT) || snake_context->head->is_ai) return;
    snake_context->head->dir = RIGHT;
}

static void on_move_down(snake_context_t *snake_context) {
    if (!check_direction(snake_context, DOWN) || snake_context->head->is_ai) return;
    snake_context->head->dir = DOWN;
}

static void on_move_left(snake_context_t *snake_context) {
    if (!check_direction(snake_context, LEFT) || snake_context->head->is_ai) return;
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
 * Задание 3.
 * Для паузы используется буква P.
 * Если кнопка нажата, то состояние паузы меняется на противопроложное.
 * **/
static void on_pause(snake_context_t *snake_context) {
    snake_context->is_paused = !snake_context->is_paused;
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
    // Задание 3.
    // P                 // p                 // З                   // з 
    key_action_map[80] = key_action_map[112]  = key_action_map[135] = key_action_map[167] = on_pause;
}

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

static void put_food(snake_context_t *context_ptr) {
    context_ptr->field[context_ptr->food->row * context_ptr->col_size + context_ptr->food->col] = FOOD;
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


static void random_init(unsigned seed) {
    srand(seed);
}

static void random_food(snake_context_t *context_ptr) {
    context_ptr->food->col = rand() % context_ptr->col_size;
    context_ptr->food->row = rand() % context_ptr->row_size;
}

static bool check_food(snake_context_t *context_ptr) {
    return 
        context_ptr->head->col_pos == context_ptr->food->col 
        && context_ptr->head->row_pos == context_ptr->food->row;
}

void* snake_make_context(unsigned row_size, unsigned col_size, int color_scheme[4], bool is_ai, double st, double initial_speed, double level_up, unsigned random_seed) {
    void* context_memory = malloc(sizeof(snake_context_t) + sizeof(snake_head_t) + sizeof(snake_food_t) + sizeof(snake_cell_t) * row_size * col_size);
    if (NULL != context_memory) {
        snake_context_t *context_ptr = (snake_context_t *)context_memory;
        context_ptr->row_size   = row_size;
        context_ptr->col_size   = col_size;
        memcpy(context_ptr->color_scheme, color_scheme, 4 * sizeof(int)); // копируем цвета во внутреннюю память структуры
        context_ptr->errcode    = 0;
        context_ptr->is_paused  = false;
        context_ptr->is_quit    = false;
        context_ptr->head       = (snake_head_t *)((unsigned char*)context_memory + sizeof(snake_context_t));
        context_ptr->food       = (snake_food_t *)((unsigned char*)context_memory + sizeof(snake_context_t) + sizeof(snake_head_t));
        context_ptr->field      = (snake_cell_t *)((unsigned char*)context_memory + sizeof(snake_context_t) + sizeof(snake_head_t) + sizeof(snake_food_t));
        context_ptr->last_st    = st;
        context_ptr->speed      = initial_speed;
        context_ptr->level_up   = level_up;
        context_ptr->level      = 1;
        
        context_ptr->head->col_pos = context_ptr->col_size / 2;
        context_ptr->head->row_pos = context_ptr->row_size / 2;
        context_ptr->head->dir = STOP;
        context_ptr->head->tail = NULL;
        context_ptr->head->is_ai = is_ai;
        
        snake_key_action_map_init();
        random_init(random_seed);
        random_food(context_ptr);
        
        clear_field(context_ptr->field, context_ptr->field + row_size * col_size);
        put_snake(context_ptr->field, context_ptr->col_size, context_ptr->head);
        put_food(context_ptr);
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
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO prev_info;
    GetConsoleScreenBufferInfo(hStdOut,&prev_info);

    snake_context_t *context_ptr = (snake_context_t *)snake_context;
    for (unsigned row_idx = 0; row_idx != context_ptr->row_size; ++row_idx) {
        for (unsigned col_idx = 0; col_idx != context_ptr->col_size; ++col_idx) {
            switch(context_ptr->field[row_idx * context_ptr->col_size + col_idx]) {
                case EMPTY: SetConsoleTextAttribute(hStdOut, context_ptr->color_scheme[EMPTY]); printf("."); break;
                case HEAD:  SetConsoleTextAttribute(hStdOut, context_ptr->color_scheme[HEAD]);  printf("@"); break;
                case TAIL:  SetConsoleTextAttribute(hStdOut, context_ptr->color_scheme[TAIL]);  printf("*"); break;
                case FOOD:  SetConsoleTextAttribute(hStdOut, context_ptr->color_scheme[FOOD]);  printf("$"); break;
            }
            SetConsoleTextAttribute(hStdOut, context_ptr->color_scheme[EMPTY]); // пробел между ячейками заполняем цветом фона
            printf(" ");
        }
        printf("\n");
    }
    SetConsoleTextAttribute(hStdOut, prev_info.wAttributes); // возвращаем изначальные настройки цвета для текстового вывода
    snake_print_level(snake_context);
}

bool is_crush(snake_head_t *head) {
    for (snake_tail_t *tail = head->tail; NULL != tail; tail = tail->tail)
        if (head->row_pos == tail->row_pos && head->col_pos == tail->col_pos) return true;
    return false;
}

void snake_advance(void *snake_context, double st) {
    snake_context_t *context_ptr = (snake_context_t *)snake_context;

    if (st - context_ptr->last_st < 1. / context_ptr->speed)
        return;

    if (!context_ptr->is_paused) {
        if (is_crush(context_ptr->head)) {
            context_ptr->is_quit = true;
            return;
        }
        /**
         * Если змейка управляется втоматически, то она пытается выбрать направление,
         * чтобы поровняться с едой сначала по строке, а потом по столбцу.
         * Змейка НЕ выбирает кратчайший путь на торе.
         * **/
        if (context_ptr->head->is_ai) {
            snake_direction_t dir = STOP;
            if (context_ptr->head->row_pos != context_ptr->food->row) {
                // если направление, в котором хотим идти, запрещено, попытаемся сместиться в сторону для разворота
                if (context_ptr->head->row_pos > context_ptr->food->row) dir = check_direction(context_ptr, UP) ? UP : LEFT;
                else if (context_ptr->head->row_pos < context_ptr->food->row) dir = check_direction(context_ptr, DOWN) ? DOWN : RIGHT;
            } else {
                if (context_ptr->head->col_pos > context_ptr->food->col) dir = check_direction(context_ptr, LEFT) ? LEFT : UP;
                else if (context_ptr->head->col_pos < context_ptr->food->col) dir = check_direction(context_ptr, RIGHT) ? RIGHT : DOWN;
            }
            context_ptr->head->dir = dir;
        }
        
        move_snake(context_ptr);
        
        if (check_food(context_ptr)) {
            ++context_ptr->level;
            context_ptr->speed *= context_ptr->level_up;
            snake_grow(context_ptr);
            random_food(context_ptr); 
        }
        
        clear_field(context_ptr->field, context_ptr->field + context_ptr->row_size * context_ptr->col_size);
        put_snake(context_ptr->field, context_ptr->col_size, context_ptr->head);
        put_food(context_ptr);
    }
    context_ptr->last_st = st;
}

int snake_context_error(void *snake_context) {
    snake_context_t *context_ptr = (snake_context_t *)snake_context;
    return context_ptr->errcode;
}

bool snake_quit(void *snake_context) {
    snake_context_t *context_ptr = (snake_context_t *)snake_context;
    return context_ptr->is_quit;
}
