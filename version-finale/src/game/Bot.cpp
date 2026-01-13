#include "Bot.hpp"
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>

Move RandomBot::suggest_move(const Game& g, double time_limit) {
    (void)time_limit; // Unused in RandomBot
    std::vector<Move> moves = g.generate_moves(g.to_play);
    if (moves.empty()) return {0, MoveType::R}; // Should not happen if check_end is called
    
    int idx = std::rand() % moves.size();
    return moves[idx];
}

Move GreedyBot::suggest_move(const Game& g, double time_limit) {
    (void)time_limit; // Unused in GreedyBot
    std::vector<Move> moves = g.generate_moves(g.to_play);
    if (moves.empty()) return {0, MoveType::R};

    Move best_move = moves[0];
    int max_gain = -1;

    for (const auto& mv : moves) {
        Game temp = g;
        int prev_score = temp.cap[(int)g.to_play];
        temp.play_move(mv);
        int new_score = temp.cap[(int)g.to_play];
        int gain = new_score - prev_score;

        if (gain > max_gain) {
            max_gain = gain;
            best_move = mv;
        }
    }

    return best_move;
}
