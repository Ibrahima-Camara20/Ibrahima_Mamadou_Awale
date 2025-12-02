#ifndef ALPHABETABOT_HPP
#define ALPHABETABOT_HPP

#include "Bot.hpp"
#include <chrono>

class AlphaBetaBot : public Bot {
public:
    Move suggest_move(const Game& g, double time_limit) override;
    std::string name() const override { return "AlphaBetaBot"; }

private:
    int minimax(Game& g, int depth, int alpha, int beta, bool maximizingPlayer, std::chrono::steady_clock::time_point start_time, double time_limit);
    int evaluate(const Game& g);
    
    // Helper to check time
    bool time_out(std::chrono::steady_clock::time_point start_time, double time_limit);
    
    // Stats
    int nodes_explored = 0;
};

#endif
