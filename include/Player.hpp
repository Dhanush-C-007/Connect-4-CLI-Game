#pragma once
#include "Board.hpp"
#include <string>

class Player {
public:
    Player(int id, const std::string& name): id(id), name(name) {}
    virtual ~Player() = default;
    virtual int getMove(const Board& board) = 0;
    int getId() const { return id; }
    std::string getName() const { return name; }
private:
    int id;
    std::string name;
};
