#ifndef SNAKE_H
#define SNAKE_H
    void* snake_make_context(unsigned row_size, unsigned col_size);
    void snake_delete_context(void *snake_context);
    void snake_key_process(int key, void *snake_context);
    void snake_draw(void *snake_context);
    void snake_advance(void *snake_context);
    int snake_context_error(void *snake_context);
    bool snake_quit(void *snake_context);
    bool snake_paused(void *snake_context);
#endif