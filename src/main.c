#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <unistd.h>
#include <snake.h>

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
