#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
};

int main() {
    double const refresh_rate = 25.;
    double const refresh_period_s = 1./refresh_rate;
    double refresh_start_s, refresh_end_s; 
    int color_scheme[4]  = { // этот массив задаёт "цветовую схему" - набор из 4 цветов
        BACKGROUND_GREEN, // тёмно-зелёный с чёрным текстом 
        BACKGROUND_GREEN | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY, // тёмно-зелёный с белым текстом
        BACKGROUND_GREEN | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
        FOREGROUND_BLUE | FOREGROUND_INTENSITY //ярко-синий на чёрном фоне
    };
    bool is_ai = true;

    void *snake_context = snake_make_context(20u, 20u, color_scheme, is_ai, double_st(), 2.0, 1.2, time(NULL));
    if (NULL == snake_context) {
        printf("Can't start the game\n");
        return 1;
    }
#ifdef _DEBUG
    unsigned frame_cnt = 0;
#endif
    refresh_start_s = refresh_end_s = double_st();
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
