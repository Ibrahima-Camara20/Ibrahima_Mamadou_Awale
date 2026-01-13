#ifndef ALPHABETABOT_HPP
#define ALPHABETABOT_HPP

#include "Bot.hpp"
#include <chrono>
#include "TranspositionTable.hpp"
#include "Evaluator.hpp"

class AlphaBetaBot : public Bot {
public:
    Move suggest_move(const Game& g, double time_limit);
    std::string name() const override { return "Version2Bot"; }

private:
    int minimax(Game& g, int depth, int alpha, int beta, bool maximizingPlayer, std::chrono::steady_clock::time_point start_time, double time_limit);
    int evaluate(const Game& g);
    
    // Helper to check time
    bool time_out(std::chrono::steady_clock::time_point start_time, double time_limit);
    
    // Stats
    int nodes_explored = 0;
    
    // Optimization
    TranspositionTable tt;
    
    // Evaluator
    Evaluator evaluator;
};

#endif

