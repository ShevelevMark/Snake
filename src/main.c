#include <stdio.h>
#include <conio.h>
#include <unistd.h>


int main() {
    unsigned frame_cnt = 0;
    while (1) {
        printf("Frame %u\n", ++frame_cnt);
        if (kbhit()) {
            int key = getch();
            printf("key code = %d\n", key);
            if (27 == key)
                break;
        }
        sleep(1);
        system("cls");
    }
    return 0;
}