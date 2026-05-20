#include <stdio.h>
#include "cli-lib/include/screen.h"
#include "cli-lib/include/keyboard.h"
#include "cli-lib/include/timer.h"

int main () {

    screenInit(1);
    keyboardInit();
    timerInit(50);

    int ch = 0;
    int x = 10;
    int y = 10;

    while(ch != 'q') {
        if (keyhit()) {
            ch = readch();

            if(ch == 'w') y--;
            if(ch == 's') y++;
            if(ch == 'a') x--;
            if(ch == 'd') x++;

        }

        if(timerTimeOver()) {
            screenClear();

            screenGotoxy(x,  y);
            printf("P");

            screenUpdate();

        }
    }

    keyboardDestroy();
    screenDestroy();
    timerDestroy();

    return 0;

}