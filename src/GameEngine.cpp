#include "GameEngine.hpp"
#include "HumanPlayer.hpp"
#include "AIPlayer.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#else
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#endif

// Helper: compute visible length of a string ignoring ANSI escape sequences
static int visibleLength(const std::string &s) {
    int len = 0;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\x1b') {
            // skip until 'm'
            while (i < s.size() && s[i] != 'm') ++i;
            continue;
        }
        ++len;
    }
    return len;
}

static const std::vector<std::string> bigTitle = {
    R"(  _____                               _     _  _      _____  _      _____ )",
    R"( / ____|                             | |   | || |    / ____|| |    |_   _|)",
    R"(| |     ___  _ __  _ __   ___   ___ _| |_  | || |_  | |     | |      | |  )",
    R"(| |    / _ \| '_ \| '_ \ / _ \ / __|_   _| |__   _| | |     | |      | |  )",
    R"(| |___| (_) | | | | | | |  __/| (__  | |_     | |   | |____ | |____ _| |_ )",
    R"(  \_____\___/|_| |_|_| |_|\___| \___|  \__|    |_|    \_____||______||_____|)"
};

GameEngine::GameEngine() {}

void GameEngine::run() {
    while (true) {
        showMainMenu();
        int choice = promptMenuChoice();
        if (choice == 3) break; // exit
        bool vsAI = (choice == 2);
        playMatch(vsAI);
    }
}

int GameEngine::getTerminalWidth() const {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    return 80;
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) return w.ws_col;
    return 80;
#endif
}

void GameEngine::showMainMenu() {
    // Clear screen and center the menu title and options
#ifdef _WIN32
    std::system("cls");
#else
    std::cout << "\x1b[2J\x1b[H";
#endif
    int width = getTerminalWidth();
    // print big ASCII title centered and always visible
    std::cout << std::endl;
    for (auto &line : bigTitle) {
        int p = std::max(0, (width - visibleLength(line)) / 2);
        std::cout << std::string(p, ' ') << line << "\n";
    }
    std::cout << "\n";
    std::vector<std::string> options = {
        "1) Human vs Human",
        "2) Human vs AI",
        "3) Exit"
    };
    for (auto &opt : options) {
        int p = std::max(0, (width - (int)opt.size())/2);
        std::cout << std::string(p, ' ') << opt << "\n";
    }
    std::cout << "\nSelect option by pressing number key...\n";
}

int GameEngine::promptMenuChoice() {
#ifdef _WIN32
    while (true) {
        int c = _getch();
        if (c >= '1' && c <= '3') return c - '0';
    }
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    while (true) {
        int c = getchar();
        if (c >= '1' && c <= '3') { tcsetattr(STDIN_FILENO, TCSANOW, &oldt); return c - '0'; }
    }
#endif
}

void GameEngine::playMatch(bool humanVsAI) {
    int score1 = 0, score2 = 0;
    bool play = true;
    while (play) {
        std::unique_ptr<Player> p1 = std::make_unique<HumanPlayer>(1, "Player-1");
        std::unique_ptr<Player> p2;
        if (humanVsAI) p2 = std::make_unique<AIPlayer>(2, "Smart Bot", 5);
        else p2 = std::make_unique<HumanPlayer>(2, "Player-2");

        Board board;
        int current = 1;
        int winner = 0;
        bool firstTurn = true;
        while (true) {
            // On first turn of player 1, show centered instruction (two lines)
            if (current == 1 && firstTurn) {
                int termW = getTerminalWidth();
                std::string line1 = p1->getName();
                std::string line2 = "select column (1-7) by pressing the key.";
                int pad1 = std::max(0, (termW - (int)line1.size())/2);
                int pad2 = std::max(0, (termW - (int)line2.size())/2);
                std::cout << std::string(pad1, ' ') << line1 << std::endl;
                std::cout << std::string(pad2, ' ') << line2 << std::endl;
                firstTurn = false;
            }
            // print board with scoreboard to the right
            printBoardWithScore(board, score1, score2, p1->getName(), p2->getName());
            Player* curP = (current == 1) ? p1.get() : p2.get();
            int move = curP->getMove(board);
            if (move == -2) {
                int action = showPauseMenu();
                if (action == 1) { // resume
                    // just re-render and continue
                    continue;
                } else if (action == 2) { // main menu
                    return;
                } else { // exit
                    exit(0);
                }
            }
            if (move < 0) { // quit to menu
                return;
            }
            board.dropDisc(move, curP->getId());
            winner = board.checkWin();
            if (winner != 0) break;
            if (board.isFull()) break;
            current = (current==1)?2:1;
        }

        printBoardWithScore(board, score1, score2, p1->getName(), p2->getName());
        if (winner != 0) {
            if (winner == 1) score1++;
            else score2++;
            std::string winnerName = (winner==1) ? p1->getName() : p2->getName();
            std::cout << winnerName << " wins!\n";
        } else {
            std::cout << "Draw!\n";
        }

        // rematch or main menu
        std::cout << "Press 'r' to rematch, 'm' for main menu, or 'e' to exit: ";
#ifdef _WIN32
        while (true) {
            int k = _getch();
            if (k=='r' || k=='R') { play = true; break; }
            if (k=='m' || k=='M') { return; }
            if (k=='e' || k=='E') { exit(0); }
        }
#else
        int k = getchar();
        if (k=='r' || k=='R') play = true;
        else if (k=='m' || k=='M') return;
        else if (k=='e' || k=='E') exit(0);
#endif
    }
}

void GameEngine::printBoardWithScore(const Board& board, int score1, int score2, const std::string& name1, const std::string& name2) const {
    // Clear screen so board doesn't stack
#ifdef _WIN32
    std::system("cls");
#else
    std::cout << "\x1b[2J\x1b[H";
#endif

    // Enable ANSI/UTF-8 on Windows consoles
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif

    // build scoreboard lines (single column) with colors
    std::vector<std::string> scoreLines;
    scoreLines.push_back(" SCOREBOARD ");
    scoreLines.push_back("--------------");
    scoreLines.push_back(std::string("\x1b[31m") + name1 + std::string(": ") + std::to_string(score1) + std::string("\x1b[0m"));
    scoreLines.push_back(std::string("\x1b[32m") + name2 + std::string(": ") + std::to_string(score2) + std::string("\x1b[0m"));

    int gap = 4;
    // construct board lines with colored tokens
    std::vector<std::string> boardLines;
    std::string border;
    for (int c = 0; c < COLS; ++c) border += "+---";
    border += "+";
    boardLines.push_back(border);
    for (int r = 0; r < ROWS; ++r) {
        std::string content;
        for (int c = 0; c < COLS; ++c) {
            content += "| ";
            int v = board.at(r, c);
            if (v == 1) content += std::string("\x1b[31mO\x1b[0m ");
            else if (v == 2) content += std::string("\x1b[32mO\x1b[0m ");
            else content += "  ";
        }
        content += "|";
        boardLines.push_back(content);
        boardLines.push_back(border);
    }
    // column numbers
    std::string nums;
    for (int c = 0; c < COLS; ++c) {
        nums += "| ";
        nums += std::to_string(c+1);
        if ((c+1) < 10) nums += ' ';
    }
    nums += "|";
    boardLines.push_back(nums);

    // calculate visible widths (ignore ANSI escapes) and padding
    int boardWidth = 0;
    for (auto &l : boardLines) boardWidth = std::max(boardWidth, visibleLength(l));
    int scoreWidth = 0;
    for (auto &s : scoreLines) scoreWidth = std::max(scoreWidth, visibleLength(s));
    int totalWidth = boardWidth + gap + scoreWidth;
    int termW = getTerminalWidth();
    int padding = std::max(0, (termW - totalWidth) / 2);

    // print big title at top so it's always visible
    for (auto &line : bigTitle) {
        int p = std::max(0, (termW - visibleLength(line)) / 2);
        std::cout << std::string(p, ' ') << line << std::endl;
    }
    std::cout << std::endl;

    for (size_t i = 0; i < boardLines.size(); ++i) {
        std::string line = boardLines[i];
        int vlen = visibleLength(line);
        int extra = boardWidth - vlen;
        std::cout << std::string(padding, ' ') << line << std::string(extra, ' ') << std::string(gap, ' ');
        if (i < scoreLines.size()) {
            std::string s = scoreLines[i];
            std::cout << s;
            int sv = visibleLength(s);
            // pad score column if needed
            if (sv < scoreWidth) std::cout << std::string(scoreWidth - sv, ' ');
        }
        std::cout << std::endl;
    }
}

int GameEngine::showPauseMenu() const {
#ifdef _WIN32
    int termW = getTerminalWidth();
    std::string opt = "Resume (r)   Main Menu (m)   Exit (e)";
    int pad = std::max(0, (termW - (int)opt.size())/2);
    std::cout << std::string(pad, ' ') << opt << std::endl;
    while (true) {
        int c = _getch();
        if (c=='r' || c=='R') return 1;
        if (c=='m' || c=='M') return 2;
        if (c=='e' || c=='E') return 3;
    }
#else
    int termW = getTerminalWidth();
    std::string opt = "Resume (r)   Main Menu (m)   Exit (e)";
    int pad = std::max(0, (termW - (int)opt.size())/2);
    std::cout << std::string(pad, ' ') << opt << std::endl;
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    while (true) {
        int c = getchar();
        if (c=='r' || c=='R') { tcsetattr(STDIN_FILENO, TCSANOW, &oldt); return 1; }
        if (c=='m' || c=='M') { tcsetattr(STDIN_FILENO, TCSANOW, &oldt); return 2; }
        if (c=='e' || c=='E') { tcsetattr(STDIN_FILENO, TCSANOW, &oldt); return 3; }
    }
#endif
}
