#ifndef SNAKE_H
#define SNAKE_H

#include <stdbool.h>

void* snake_make_context(unsigned row_size, unsigned col_size, int color_scheme[4], bool is_ai, double start_time, double initial_speed, double level_up_speed_coeff, unsigned random_seed);
void snake_delete_context(void *snake_context);
void snake_key_process(int key, void *snake_context);
void snake_draw(void *snake_context);
void snake_advance(void *snake_context, double time);
int snake_context_error(void *snake_context);
bool snake_quit(void *snake_context);
void snake_print_exit(void *snake_context);

#endif