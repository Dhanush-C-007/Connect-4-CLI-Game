#include "Board.hpp"
#include <iostream>
#include <cstdlib>
#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

Board::Board() {
    for (auto &row : grid) row.fill(0);
}

bool Board::isValidMove(int col) const {
    if (col < 0 || col >= COLS) return false;
    return grid[0][col] == 0;
}

bool Board::dropDisc(int col, int player) {
    if (!isValidMove(col)) return false;
    for (int r = ROWS - 1; r >= 0; --r) {
        if (grid[r][col] == 0) {
            grid[r][col] = player;
            return true;
        }
    }
    return false;
}

void Board::undoDrop(int col) {
    for (int r = 0; r < ROWS; ++r) {
        if (grid[r][col] != 0) {
            grid[r][col] = 0;
            return;
        }
    }
}

std::vector<int> Board::validMoves() const {
    std::vector<int> moves;
    for (int c = 0; c < COLS; ++c) if (isValidMove(c)) moves.push_back(c);
    return moves;
}

bool Board::isFull() const {
    for (int c = 0; c < COLS; ++c) if (grid[0][c] == 0) return false;
    return true;
}

int Board::checkWin() const {
    // Horizontal
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c <= COLS - 4; ++c) {
            int v = grid[r][c];
            if (v != 0 && v == grid[r][c+1] && v == grid[r][c+2] && v == grid[r][c+3]) return v;
        }
    }
    // Vertical
    for (int c = 0; c < COLS; ++c) {
        for (int r = 0; r <= ROWS - 4; ++r) {
            int v = grid[r][c];
            if (v != 0 && v == grid[r+1][c] && v == grid[r+2][c] && v == grid[r+3][c]) return v;
        }
    }
    // Diagonal down-right
    for (int r = 0; r <= ROWS - 4; ++r) {
        for (int c = 0; c <= COLS - 4; ++c) {
            int v = grid[r][c];
            if (v != 0 && v == grid[r+1][c+1] && v == grid[r+2][c+2] && v == grid[r+3][c+3]) return v;
        }
    }
    // Diagonal up-right
    for (int r = 3; r < ROWS; ++r) {
        for (int c = 0; c <= COLS - 4; ++c) {
            int v = grid[r][c];
            if (v != 0 && v == grid[r-1][c+1] && v == grid[r-2][c+2] && v == grid[r-3][c+3]) return v;
        }
    }
    return 0;
}

void Board::print() const {
    // Enable ANSI escape codes and UTF-8 output on Windows consoles
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
    std::system("cls");
#else
    std::cout << "\x1b[2J\x1b[H";
#endif

    // Draw top border
    for (int c = 0; c < COLS; ++c) std::cout << "+---";
    std::cout << "+\n";

    // Rows with vertical separators
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            std::cout << "| ";
            if (grid[r][c] == 1) {
                std::cout << "\x1b[31mO\x1b[0m";
            } else if (grid[r][c] == 2) {
                std::cout << "\x1b[32mO\x1b[0m";
            } else {
                std::cout << ' ';
            }
            std::cout << ' ';
        }
        std::cout << "|\n";

        // Row separator
        for (int c = 0; c < COLS; ++c) std::cout << "+---";
        std::cout << "+\n";
    }

    // Column numbers (1-based) printed in same cell positions
    for (int c = 0; c < COLS; ++c) {
        std::cout << "| " << (c+1) << ' ';
    }
    std::cout << "|\n";
}

std::vector<std::string> Board::renderLines() const {
    std::vector<std::string> lines;
    // top border
    std::string border;
    for (int c = 0; c < COLS; ++c) border += "+---";
    border += "+";
    lines.push_back(border);

    for (int r = 0; r < ROWS; ++r) {
        std::string content;
        for (int c = 0; c < COLS; ++c) {
            content += "| ";
            if (grid[r][c] == 1) content += "O ";
            else if (grid[r][c] == 2) content += "O ";
            else content += "  ";
        }
        content += "|";
        lines.push_back(content);
        lines.push_back(border);
    }

    // column numbers centered under each cell (1-based)
    std::string nums;
    for (int c = 0; c < COLS; ++c) {
        nums += "| ";
        nums += std::to_string(c+1);
        if ((c+1) < 10) nums += ' ';
    }
    nums += "|";
    lines.push_back(nums);
    return lines;
}
