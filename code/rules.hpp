#pragma once
#include <array>
#include <cstdint>
#include <vector>

// ============================================================================
// CONSTANTS
// ============================================================================
constexpr int BOARD_SIZE = 16;
constexpr int MAX_SEEDS = 96; // 16 holes * 6 seeds

// ============================================================================
// TYPES
// ============================================================================

enum class Player : uint8_t {
    Odd = 0, // Controls indices 0, 2, 4... (Holes 1, 3, 5...)
    Even = 1 // Controls indices 1, 3, 5... (Holes 2, 4, 6...)
};

enum class SeedType : uint8_t { Red, Blue, Trans };

// Behavior declaration for Transparent seeds
enum class Behavior : uint8_t {
    RedLike,  // Visits every hole
    BlueLike  // Visits only opponent holes
};

// Represents a requested move
struct Move {
    int hole_idx;           // 0-15
    SeedType selected_type; 
    Behavior declaration;   // Only used if selected_type == Trans
};

// Optimized hole structure (4 bytes)
struct Hole {
    uint8_t r;
    uint8_t b;
    uint8_t t;
    uint8_t _padding; 

    uint8_t total() const { return r + b + t; }
    void add(SeedType s);
    uint8_t get_count(SeedType s) const;
    void clear();
};

// ============================================================================
// ENGINE CLASS
// ============================================================================

class Rules2025Engine {
private:
    std::array<Hole, BOARD_SIZE> board;
    std::array<int, 2> scores;
    Player current_turn;
    bool game_over_flag;

    // Helpers
    Player get_owner(int idx) const;
    int next_physical(int idx) const;
    int prev_physical(int idx) const;
    int count_seeds_for_player(Player p) const;
    void reap(int start_idx_geometric);
    void check_game_end();
    void sweep_board_to_score(Player p);

public:
    Rules2025Engine();
    void reset();

    // Getters
    const std::array<Hole, BOARD_SIZE>& get_board() const { return board; }
    int get_score(Player p) const { return scores[static_cast<int>(p)]; }
    Player get_turn() const { return current_turn; }
    bool is_game_over() const { return game_over_flag; }

    // Core Logic
    // Returns false if move is illegal
    bool execute_move(Move m);
};