#pragma once
#include "Player.hpp"

class AIPlayer : public Player {
public:
    AIPlayer(int id, const std::string& name, int depth = 5);
    int getMove(const Board& board) override;
private:
    int maxDepth;
    int minimax(Board& board, int depth, int alpha, int beta, bool maximizingPlayer);
    int evaluate(const Board& board);
};
