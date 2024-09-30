#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <conio.h>
#include <snake.h>
#include <Windows.h>
#include <time.h>

double double_st(void) {
    SYSTEMTIME st;
    GetSystemTime(&st); 
    return 
        (double)st.wHour * 3600.0 +
        (double)st.wMinute * 60.0 +
        (double)st.wSecond +
        (double)st.wMilliseconds / 1000.0;
}

typedef struct snake_init {
    unsigned row_size, col_size;
    int color_scheme[4];
    bool is_ai;
    double speed, accel;
    unsigned random_seed;
} snake_init_t;

/**
 * Набор предопределённых цветовых схем
 * **/
int a_scheme[4] = {
    BACKGROUND_GREEN,
    BACKGROUND_GREEN | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    BACKGROUND_GREEN | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    FOREGROUND_BLUE | FOREGROUND_INTENSITY 
};

int b_scheme[4] = {
    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
};

int c_scheme[4] = {
    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, 
    FOREGROUND_BLUE | FOREGROUND_INTENSITY,
}; 

int d_scheme[4] = {
    BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
    BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY
};

int main() {
    snake_init_t init;
    init.row_size = 20u;
    init.col_size = 30u;
    memcpy(init.color_scheme, d_scheme, 4 * sizeof(int));
    init.is_ai = true;
    init.speed = 2.0;
    init.accel = 1.2;
    init.random_seed = time(NULL);

    void *snake_context = snake_make_context(init.row_size, init.col_size, init.color_scheme, init.is_ai, double_st(), init.speed, init.accel, init.random_seed);
    if (NULL == snake_context) {
        printf("Can't start the game\n");
        return 1;
    }

#ifdef _DEBUG
    unsigned frame_cnt = 0;
#endif
    /**
     * Эти данные необходимы для относительно стабильной 
     * скорости обновления изображения независимо от движения
     * змейки.
     * **/
    double const refresh_rate = 25.;
    double const refresh_period_s = 1./refresh_rate;
    double refresh_start_s, refresh_end_s;
    refresh_start_s = refresh_end_s = double_st();

    /**
     * Основной цикл, который вызвает функции отрисовки и
     * обновления состоияния.
     * **/
    while (!snake_quit(snake_context)) {
        if (kbhit()) {
            int key = getch();
#ifdef _DEBUG
            printf("key code = %d\n", key);
#endif
            snake_key_process(key, snake_context);
            if (0 != snake_context_error(snake_context))
                break;
        }
        
        snake_advance(snake_context, double_st());
        if (0 != snake_context_error(snake_context))
                break;

        refresh_end_s = double_st();
        if (refresh_end_s - refresh_start_s > refresh_period_s) {
            system("cls");
#ifdef _DEBUG
            printf("Frame %u\n", ++frame_cnt);
#endif
            snake_draw(snake_context);
            refresh_start_s = double_st();
        } 
    }

    if (0 != snake_context_error(snake_context))
        printf("Internal snake application error: %d\n", snake_context_error(snake_context));
    else
        snake_print_exit(snake_context); // Задание 1.
    
    snake_delete_context(snake_context);
    return 0;
}
