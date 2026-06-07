#pragma once
#include "Board.hpp"
#include "Player.hpp"

class GameEngine {
public:
    GameEngine();
    void run();
private:
    int getTerminalWidth() const;
    void showMainMenu();
    int promptMenuChoice();
    void playMatch(bool humanVsAI);
    void printBoardWithScore(const Board& board, int score1, int score2, const std::string& name1, const std::string& name2) const;
    int showPauseMenu() const; // returns 1=resume,2=main menu,3=exit
};
