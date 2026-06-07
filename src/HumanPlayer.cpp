#include "HumanPlayer.hpp"
#include <iostream>
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#endif

static int getch_noblock() {
#ifdef _WIN32
    int c = _getch();
    return c;
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    int c = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return c;
#endif
}

int HumanPlayer::getMove(const Board& board) {
    while (true) {
        int ch = getch_noblock();
        if (ch == EOF) continue;
        if (ch >= '1' && ch <= '7') {
            int col = (ch - '1');
            if (board.isValidMove(col)) return col;
            else {
                // do nothing; caller will re-render and prompt
            }
        }
        // allow q to quit back to menu
        if (ch == 'q' || ch == 'Q') return -1;
        // ESC to pause
        if (ch == 27) return -2;
    }
}
