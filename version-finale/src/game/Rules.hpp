#ifndef RULES_HPP
#define RULES_HPP

#include <cstdint>

constexpr int NHOLES = 16;
constexpr int WINNING_SCORE = 49;
constexpr int END_BOARD_SEEDS = 10;
constexpr int MAX_MOVES_PER_PLAYER = 200;
constexpr int INITIAL_SEEDS_PER_COLOR = 2;

// Phase Thresholds for Dynamic Evaluation
constexpr int THRESHOLD_OPENING = 75;
constexpr int THRESHOLD_ENDGAME = 25;

// AI Evaluation Constants
struct EvalWeights {
    double w_score;
    double w_board_mat;
    double w_vuln;
    double w_attack;
    double w_chain;
    double w_kroo;
    double w_mob;
    double w_blue;
    double w_trans;
};

// Phase 1: Opening
inline EvalWeights phase1 = {100.0, 10.0, -40.0, 30.0, -50.0, 60.0, 50.0, 5.0, 20.0};
// Phase 2: Midgame (Agression maximale)
inline EvalWeights phase2 = {500.0, 20.0, -150.0, 120.0, -200.0, 30.0, 40.0, 25.0, 20.0};
// Phase 3: Endgame (Précision et Anti-Famine)
inline EvalWeights phase3 = {10000.0, 50.0, -500.0, 200.0, -1000.0, 0.0, 1000.0, 10.0, 5.0};

// Extended Hole structure for Evaluator
struct EvalHole {
    uint8_t red;
    uint8_t blue;
    uint8_t transparent;
    
    inline int total() const { return red + blue + transparent; }
    inline bool is_critical() const {
        int t = total();
        return (t == 1 || t == 2);
    }
};

// GameState for Evaluator
struct GameState {
    EvalHole holes[16];
    int score_p1;
    int score_p2;
    int total_seeds;
    bool turn_p1;
};

// Joueurs
enum class Player : uint8_t { P1 = 0, P2 = 1 };

// Comportement (rouge ou bleu)
enum class Behav : uint8_t { RED = 0, BLUE = 1 };

// Type de mouvement
enum class MoveType : uint8_t { R, B, TR, TB };

// États du jeu
enum class State : uint8_t { CONTINUE, G1WIN, G2WIN, DRAW };

#endif
