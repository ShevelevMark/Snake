#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <unistd.h>
#include <snake.h>
#include <Windows.h>

double double_st(void) {
    SYSTEMTIME st;
    GetSystemTime(&st); 
    return 
        (double)st.wHour * 3600.0 +
        (double)st.wMinute * 60.0 +
        (double)st.wSecond +
        (double)st.wMilliseconds / 1000.0;
}

int main() {
    double period_s = 0.3, start_s, end_s; 

    void *snake_context = snake_make_context(20u, 20u);
    if (NULL == snake_context) {
        printf("Can't start the game\n");
        return 1;
    }
#ifdef _DEBUG
    unsigned frame_cnt = 0;
#endif
    start_s = end_s = double_st();
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
        
        end_s = double_st();
        if (end_s - start_s > period_s) {
            if (!snake_paused(snake_context))
                snake_advance(snake_context);
            if (0 != snake_context_error(snake_context))
                break;
            
            system("cls");
#ifdef _DEBUG
            printf("Frame %u\n", ++frame_cnt);
#endif
            snake_draw(snake_context);
            start_s = double_st();
        } 
    }

    if (0 != snake_context_error(snake_context))
        printf("Internal snake application error: %d\n", snake_context_error(snake_context));
    
    snake_delete_context(snake_context);
    return 0;
}
