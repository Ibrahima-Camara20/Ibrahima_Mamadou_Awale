#ifndef BOT_HPP
#define BOT_HPP

#include "Game.hpp"
#include <string>

class Bot {
public:
    virtual ~Bot() = default;
    virtual std::string name() const = 0;
    virtual Move suggest_move(const Game& g, double time_limit) = 0;
};

class RandomBot : public Bot {
public:
    Move suggest_move(const Game& g, double time_limit) override;
    std::string name() const override { return "RandomBot"; }
};

class GreedyBot : public Bot {
public:
    Move suggest_move(const Game& g, double time_limit) override;
    std::string name() const override { return "GreedyBot"; }
};

#endif

