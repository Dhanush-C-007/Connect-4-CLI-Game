#pragma once
#include "Player.hpp"
#include <iostream>

class HumanPlayer : public Player {
public:
    HumanPlayer(int id, const std::string& name): Player(id, name) {}
    int getMove(const Board& board) override;
};
