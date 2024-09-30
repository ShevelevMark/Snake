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

/**
 * Функция генерации настроек.
 * **/
snake_init_t menu(void);

int main() {
    snake_init_t init = menu();
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

/**
 * Функция печатает пример переданной цветовой схемы.
 * Используется шаблон . . * * * @ . . $ . 
 * **/
void print_color_scheme_example(int color_scheme[4]) {
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO prev_info;
    GetConsoleScreenBufferInfo(hStdOut,&prev_info);

    SetConsoleTextAttribute(hStdOut, color_scheme[0]);
    printf(". . ");
    SetConsoleTextAttribute(hStdOut, color_scheme[2]);
    printf("* * * ");
    SetConsoleTextAttribute(hStdOut, color_scheme[1]);
    printf("@");
    SetConsoleTextAttribute(hStdOut, color_scheme[0]);
    printf(" . . ");
    SetConsoleTextAttribute(hStdOut, color_scheme[3]);
    printf("$");
    SetConsoleTextAttribute(hStdOut, color_scheme[0]);
    printf(" . ");

    SetConsoleTextAttribute(hStdOut, prev_info.wAttributes); 
}

/**
 * Функция представляет несколько цветовых схем на выбор.
 * Выбраная схема запоминается в структуре настроек.
 * При ошибках схема не запоминается, программа возвращается
 * в основное меню.
 * **/
void change_colors(snake_init_t *settings) {
    system("cls");
    printf("Choose the color scheme:\n");
    printf("1) "); print_color_scheme_example(a_scheme); printf("\n");
    printf("2) "); print_color_scheme_example(b_scheme); printf("\n");
    printf("3) "); print_color_scheme_example(c_scheme); printf("\n");
    printf("4) "); print_color_scheme_example(d_scheme); printf("\n");

    int i = 0;
    if (1 == scanf("%d", &i)) {
        switch(i) {
            case 1: memcpy(settings->color_scheme, a_scheme, 4 * sizeof(int)); break;
            case 2: memcpy(settings->color_scheme, b_scheme, 4 * sizeof(int)); break;
            case 3: memcpy(settings->color_scheme, c_scheme, 4 * sizeof(int)); break;
            case 4: memcpy(settings->color_scheme, d_scheme, 4 * sizeof(int)); break;
        }
    }
    else
        scanf("%*s"); // на случай если введена буква
}

/**
 * Функция считывания размеров.
 * Функция не проводит проверок размеров, только 
 * успешность считывания данных.
 * Если считывание не успешно - возвращается в основное меню.
 * **/
void change_sizes(snake_init_t *settings) {
    system("cls");
    printf("Enter new sizes for the field: ");
    unsigned row_size, col_size;
    if (2 == scanf("%u %u", &row_size, &col_size)) {
        settings->row_size = row_size;
        settings->col_size = col_size;
    } else 
        scanf("%*s"); // на случай если введена буква
}

/**
 * Функция для печати текущих настроек.
 * **/
void print_current_settings(snake_init_t settings) {
    printf("Current settings: h %u; w %u; colors ", settings.row_size, settings.col_size);
    print_color_scheme_example(settings.color_scheme);
    printf("; AI %s\n", settings.is_ai ? "Yes" : "No");
}

/**
 * Функция запуска основного меню. 
 * Вначале идёт печать текущих настроке print_current_settings,
 * Затем функция ждёт ввода даных о смене настроек или запуске программы.
 * Функция будет перезапускать цилк запроса, пока пользователь
 * не выберет пунтк 1 - запусить игру (run the game).
 * **/
void main_menu(snake_init_t *settings) {
    do {
        system("cls");
        printf("You may choose to start the game or change the settings.\n");
        print_current_settings(*settings);
        printf("1) run the game\n");
        printf("2) set sizes\n");
        printf("3) change color\n");
        printf("4) %s\n", settings->is_ai ? "run as user" : "run as AI");

        int i = 0;
        if (1 == scanf("%d", &i)) { 
            switch(i) {
                case 1: return;
                case 2: change_sizes(settings); break;
                case 3: change_colors(settings); break;
                case 4: settings->is_ai = !(settings->is_ai); break; 
            } 
        }
        else
            scanf("%*s"); // на случай если случайно введена буква
    } while (1);
}

/**
 * Функция устанавливает настройки по умолчанию и запускает 
 * основное меню.
 * После функция возвращает результирующие настройки.
 * **/
snake_init_t menu(void) {
    snake_init_t settings = {20u, 30u, {0, 0, 0, 0}, true, 2.0, 1.2, time(NULL)}; // default
    memcpy(settings.color_scheme, a_scheme, 4 * sizeof(int));                     // default
    printf("Hi! This is the console-snake-game application!\n");
    main_menu(&settings); 
    return settings;
}
