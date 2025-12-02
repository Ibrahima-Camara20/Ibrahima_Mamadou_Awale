#include "AlphaBetaBot.hpp"
#include <limits>
#include <vector>
#include <algorithm>
#include <iostream>

// Improved evaluation function
int AlphaBetaBot::evaluate(const Game& g) {
    int score_diff = g.cap[0] - g.cap[1];
    
    // Heuristic weights
    const int W_SCORE = 100;
    const int W_MOBILITY = 1;
    const int W_VULNERABILITY = -2;
    
    int mobility = 0;
    int vulnerability = 0;
    
    // Mobility: Number of legal moves
    // Vulnerability: Holes with 1 or 2 seeds that opponent can capture?
    // This is expensive to calculate accurately.
    // Simple proxy: Holes with 1 or 2 seeds are vulnerable IF opponent can reach them.
    
    // For now, let's just stick to score difference as primary, 
    // but maybe add a bonus for having seeds on our side (potential defense/attack)?
    // Or penalty for empty holes (starvation risk)?
    
    // Let's count seeds on board per player
    int seeds_p1 = 0;
    int seeds_p2 = 0;
    for(int i=0; i<16; ++i) {
        int s = g.h[i].r + g.h[i].b + g.h[i].t;
        if (g.is_owned(Player::P1, i)) seeds_p1 += s;
        else seeds_p2 += s;
    }
    
    // If I have more seeds, I am safer from starvation and have more ammo.
    int seeds_diff = seeds_p1 - seeds_p2;
    
    return score_diff * W_SCORE + seeds_diff;
}

bool AlphaBetaBot::time_out(std::chrono::steady_clock::time_point start_time, double time_limit) {
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = now - start_time;
    return elapsed.count() >= time_limit;
}

Move AlphaBetaBot::suggest_move(const Game& g, double time_limit) {
    auto start_time = std::chrono::steady_clock::now();
    nodes_explored = 0;
    
    std::vector<Move> moves = g.generate_moves(g.to_play);
    if (moves.empty()) return {0, MoveType::R};
    
    Move best_move = moves[0];
    
    // Iterative Deepening
    int max_depth = 1;
    bool time_up = false;
    
    // We want to maximize score for current player.
    // If current player is P1, we want max(evaluate).
    // If current player is P2, we want min(evaluate).
    bool maximizing = (g.to_play == Player::P1);
    // how deep can we go? // 20 is arbitrary // how many moves can we look ahead?
     
    while (!time_up && max_depth <= 20) { // Cap depth to avoid infinite loop
        int best_val = maximizing ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
        Move current_best_move = moves[0];
        bool completed_depth = true;
        
        for (const auto& mv : moves) {
            if (time_out(start_time, time_limit)) {
                time_up = true;
                completed_depth = false;
                break;
            }
            
            Game temp = g;
            temp.play_move(mv);
            
            int val = minimax(temp, max_depth - 1, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), !maximizing, start_time, time_limit);
            
            // Check if minimax returned due to timeout (heuristic check needed?)
            // For now, if time_out inside minimax, it returns heuristic.
            
            if (maximizing) {
                if (val > best_val) {
                    best_val = val;
                    current_best_move = mv;
                }
            } else {
                if (val < best_val) {
                    best_val = val;
                    current_best_move = mv;
                }
            }
        }
        
        if (completed_depth) {
            best_move = current_best_move;
            // std::cout << "Depth " << max_depth << " completed. Best move: " << best_move.src << " Val: " << best_val << "\n";
            max_depth++;
        }
    }
    
    std::cout << "Nodes explored: " << nodes_explored << " Max Depth: " << max_depth - 1 << "\n";
    return best_move;
}

int AlphaBetaBot::minimax(Game& g, int depth, int alpha, int beta, bool maximizingPlayer, std::chrono::steady_clock::time_point start_time, double time_limit) {
    nodes_explored++;
    
    if (depth == 0 || g.check_end() != State::CONTINUE) {
        return evaluate(g);
    }
    
    if (nodes_explored % 1000 == 0) {
        if (time_out(start_time, time_limit)) return evaluate(g);
    }

    std::vector<Move> moves = g.generate_moves(g.to_play);
    
    if (maximizingPlayer) { // P1
        int maxEval = std::numeric_limits<int>::min();
        for (const auto& mv : moves) {
            Game temp = g;
            temp.play_move(mv);
            int eval = minimax(temp, depth - 1, alpha, beta, false, start_time, time_limit);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
        }
        return maxEval;
    } else { // P2

        int minEval = std::numeric_limits<int>::max(); 
        for (const auto& mv : moves) {
            Game temp = g;
            temp.play_move(mv);
            int eval = minimax(temp, depth - 1, alpha, beta, true, start_time, time_limit);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
        }
        return minEval;
    }
}
