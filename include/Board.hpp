#pragma once
#include <array>
#include <vector>
#include <string>

static const int ROWS = 6;
static const int COLS = 7;

class Board {
public:
    Board();
    bool dropDisc(int col, int player);
    bool isValidMove(int col) const;
    bool isFull() const;
    int checkWin() const; // 0 = none, 1 or 2 = player
    void print() const;
    std::vector<int> validMoves() const;
    int at(int r, int c) const { return grid[r][c]; }
    void undoDrop(int col);
    std::vector<std::string> renderLines() const;
private:
    std::array<std::array<int, COLS>, ROWS> grid;
};
