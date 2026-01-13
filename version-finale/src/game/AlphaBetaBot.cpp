#include "AlphaBetaBot.hpp"
#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>

// Internal exception for timeout control
struct TimeoutException {};

int AlphaBetaBot::evaluate(const Game &g) {
  // Convert Game to GameState for the Evaluator
  GameState state;
  
  // Convert holes from Game::Hole to EvalHole
  for (int i = 0; i < NHOLES; ++i) {
    state.holes[i].red = g.h[i].r;
    state.holes[i].blue = g.h[i].b;
    state.holes[i].transparent = g.h[i].t;
  }
  
  // Copy scores
  state.score_p1 = g.cap[0];
  state.score_p2 = g.cap[1];
  
  // Calculate total seeds on board
  state.total_seeds = g.sum_board();
  
  // Set turn
  state.turn_p1 = (g.to_play == Player::P1);
  
  // Call the Evaluator
  double eval_result = evaluator.evaluate(state);
  
  // Cast to int (evaluation weights are large enough that precision is preserved)
  return static_cast<int>(eval_result);
}

bool AlphaBetaBot::time_out(std::chrono::steady_clock::time_point start_time,
                            double time_limit) {
  auto now = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed = now - start_time;
  return elapsed.count() >= time_limit;
}

Move AlphaBetaBot::suggest_move(const Game &g, double time_limit) {
  auto start_time = std::chrono::steady_clock::now();
  nodes_explored = 0;

  std::vector<Move> moves = g.generate_moves(g.to_play);
  if (moves.empty())
    return {0, MoveType::R};

  Move best_move = moves[0];

  // Iterative Deepening
  int max_depth = 1;
  bool time_up = false;

  // We want to maximize score for current player.
  // If current player is P1, we want max(evaluate).
  // If current player is P2, we want min(evaluate).
  bool maximizing = (g.to_play == Player::P1);
  // how deep can we go? // 20 is arbitrary // how many moves can we look ahead?


  try {
    while (!time_up && max_depth <= 20) { // Cap depth to avoid infinite loop
      int best_val = maximizing ? std::numeric_limits<int>::min()
                                : std::numeric_limits<int>::max();
      Move current_best_move = moves[0];
      bool completed_depth = true;

      // Create a mutable copy for the search
      Game search_game = g;

      for (const auto &mv : moves) {
        if (time_out(start_time, time_limit)) {
          throw TimeoutException();
        }

        auto state = search_game.store_state();
        search_game.play_move(mv);

        int val = minimax(search_game, max_depth - 1, std::numeric_limits<int>::min(),
                          std::numeric_limits<int>::max(), !maximizing,
                          start_time, time_limit);
        
        search_game.restore_state(state);

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
        // std::cout << "Depth " << max_depth << " completed. Best move: " <<
        // best_move.src << " Val: " << best_val << "\n";
        max_depth++;
      }
    }
  } catch (const TimeoutException &) {
    time_up = true;
    // Do not update best_move from this incomplete depth
  }
  return best_move;
}

int AlphaBetaBot::minimax(Game &g, int depth, int alpha, int beta,
                          bool maximizingPlayer,
                          std::chrono::steady_clock::time_point start_time,
                          double time_limit) {
  nodes_explored++;

  State s = g.check_end();
  if (depth == 0 || s != State::CONTINUE) {
      if (s == State::G1WIN) return 1000000;  // Win for P1
      if (s == State::G2WIN) return -1000000; // Win for P2
      if (s == State::DRAW) return 0;
      return evaluate(g);
  }

  if (nodes_explored % 1000 == 0) {
    if (time_out(start_time, time_limit))
      throw TimeoutException();
  }

  // 1. TT Probe
  TTEntry tt_entry = {};
  bool has_tt = tt.probe(g.hash, tt_entry);
  
  if (has_tt) {
      if (tt_entry.depth >= depth) {
          if (tt_entry.type == NodeType::EXACT)
              return tt_entry.value;
          if (tt_entry.type == NodeType::LOWER && tt_entry.value > alpha)
              alpha = tt_entry.value;
          else if (tt_entry.type == NodeType::UPPER && tt_entry.value < beta)
              beta = tt_entry.value;
          
          if (alpha >= beta)
              return tt_entry.value;
      }
  }

  std::vector<Move> moves = g.generate_moves(g.to_play);

  // 2. Move Ordering (Put TT Best Move first)
  if (has_tt) { // Valid entry exists
      for(size_t i=0; i<moves.size(); ++i) {
          if (moves[i].src == tt_entry.best_move.src && moves[i].type == tt_entry.best_move.type) {
              std::swap(moves[0], moves[i]);
              break;
          }
      }
  }

  if (maximizingPlayer) { // P1
    int maxEval = std::numeric_limits<int>::min();
    Move best_move = {0, MoveType::R};
    
    for (const auto &mv : moves) {
      auto state = g.store_state();
      g.play_move(mv);
      int eval =
          minimax(g, depth - 1, alpha, beta, false, start_time, time_limit);
      g.restore_state(state);
      
      if (eval > maxEval) {
          maxEval = eval;
          best_move = mv;
      }
      alpha = std::max(alpha, eval);
      if (beta <= alpha)
        break;
    }
    
    // Store TT
    // Simplified: always store as EXACT for now to avoid unused var warning 
    // and complexity without alpha_orig.
    // Ideally: Implement bounds checking.
    tt.store(g.hash, maxEval, depth, NodeType::EXACT, best_move);
    return maxEval;
    
  } else { // P2

    int minEval = std::numeric_limits<int>::max();
    Move best_move = {0, MoveType::R};
    
    for (const auto &mv : moves) {
      auto state = g.store_state();
      g.play_move(mv);
      int eval =
          minimax(g, depth - 1, alpha, beta, true, start_time, time_limit);
      g.restore_state(state);
      
      if (eval < minEval) {
          minEval = eval;
          best_move = mv;
      }
      beta = std::min(beta, eval);
      if (beta <= alpha)
        break;
    }
    
    tt.store(g.hash, minEval, depth, NodeType::EXACT, best_move);
    return minEval;
  }
}
