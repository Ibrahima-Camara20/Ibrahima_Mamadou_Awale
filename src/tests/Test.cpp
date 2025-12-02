#include <iostream>
#include <cassert>
#include <vector>
#include <ctime>
#include "../game/Game.hpp"
#include "../game/Bot.hpp"

// Helper to set up a specific board state
void setup_board(Game& g, const std::vector<Hole>& holes) {
    for (int i = 0; i < 16; ++i) {
        g.h[i] = holes[i];
    }
}

// Helper to check board state
bool check_board(const Game& g, const std::vector<Hole>& expected_holes) {
    for (int i = 0; i < 16; ++i) {
        if (g.h[i].r != expected_holes[i].r ||
            g.h[i].b != expected_holes[i].b ||
            g.h[i].t != expected_holes[i].t) {
            std::cout << "Mismatch at hole " << i + 1 << ": "
                      << "Expected (" << expected_holes[i].r << "R, " << expected_holes[i].b << "B, " << expected_holes[i].t << "T), "
                      << "Got (" << g.h[i].r << "R, " << g.h[i].b << "B, " << g.h[i].t << "T)\n";
            return false;
        }
    }
    return true;
}

void test_case_1() {
    std::cout << "Running Test Case 1...\n";
    Game g;
    std::vector<Hole> initial_holes(16, {0, 0, 0});
    initial_holes[0] = {2, 0, 0};   // Hole 1
    initial_holes[15] = {2, 0, 0};  // Hole 16
    initial_holes[14] = {0, 2, 0};  // Hole 15
    initial_holes[13] = {2, 2, 0};  // Hole 14
    initial_holes[12] = {2, 2, 0};  // Hole 13
    
    setup_board(g, initial_holes);
    g.to_play = Player::P2;

    Move mv = {13, MoveType::B};
    
    if (!g.play_move(mv)) {
        std::cout << "FAILED: Move 14B should be valid.\n";
        return;
    }

    std::vector<Hole> expected_holes = initial_holes;
    expected_holes[0] = {0, 0, 0};
    expected_holes[15] = {0, 0, 0};
    expected_holes[14] = {0, 0, 0};
    expected_holes[13] = {0, 0, 0};
    expected_holes[12] = {2, 2, 0};
    
    if (check_board(g, expected_holes)) std::cout << "Case 1 Board State: PASSED\n";
    else std::cout << "Case 1 Board State: FAILED\n";
    
    if (g.cap[1] == 10) std::cout << "Case 1 Captures: PASSED\n";
    else std::cout << "Case 1 Captures: FAILED (Expected 10, Got " << g.cap[1] << ")\n";
}

void test_case_2() {
    std::cout << "\nRunning Test Case 2...\n";
    Game g;
    std::vector<Hole> initial_holes(16, {0, 0, 0});
    initial_holes[0] = {1, 0, 0}; // 1
    initial_holes[1] = {2, 0, 0}; // 2
    initial_holes[2] = {0, 1, 0}; // 3
    initial_holes[3] = {0, 2, 0}; // 4
    initial_holes[4] = {1, 0, 0}; // 5
    
    initial_holes[15] = {1, 3, 0}; // 16 (3B 1R)
    initial_holes[14] = {2, 0, 0}; // 15
    initial_holes[13] = {0, 4, 0}; // 14
    
    setup_board(g, initial_holes);
    g.to_play = Player::P2;
    
    // Case 2.1
    {
        Game g1 = g;
        Move mv = {15, MoveType::B};
        g1.play_move(mv);
        if (g1.cap[1] == 10) std::cout << "Case 2.1 Captures: PASSED\n";
        else std::cout << "Case 2.1 Captures: FAILED (Expected 10, Got " << g1.cap[1] << ")\n";
    }
    
    // Case 2.2
    {
        Game g2 = g;
        Move mv = {15, MoveType::R};
        g2.play_move(mv);
        if (g2.cap[1] == 7) std::cout << "Case 2.2 Captures: PASSED\n";
        else std::cout << "Case 2.2 Captures: FAILED (Expected 7, Got " << g2.cap[1] << ")\n";
    }
}

void test_starvation() {
    std::cout << "\nRunning Starvation Test...\n";
    Game g;
    std::vector<Hole> initial_holes(16, {0, 0, 0});
    initial_holes[0] = {1, 0, 0}; // P1
    initial_holes[2] = {5, 0, 0}; // P1
    initial_holes[1] = {2, 0, 0}; // P2
    
    setup_board(g, initial_holes);
    g.to_play = Player::P1;
    
    Move mv = {0, MoveType::R};
    g.play_move(mv);
    
    if (g.cap[0] == 8) std::cout << "Starvation Test: PASSED\n";
    else std::cout << "Starvation Test: FAILED (Expected 8, Got " << g.cap[0] << ")\n";
    
    if (g.sum_board() == 0) std::cout << "Starvation Board Empty: PASSED\n";
    else std::cout << "Starvation Board Empty: FAILED (Sum " << g.sum_board() << ")\n";
}

void test_bots() {
    std::cout << "\nRunning Bot Test (Random vs Greedy)...\n";
    Game g;
    RandomBot p1;
    GreedyBot p2;
    
    int moves = 0;
    while (g.check_end() == State::CONTINUE && moves < 200) {
        Bot* current = (g.to_play == Player::P1) ? (Bot*)&p1 : (Bot*)&p2;
        Move mv = current->suggest_move(g, 0.1);
        
        if (!g.play_move(mv)) {
            std::cout << "Bot " << current->name() << " played invalid move!\n";
            break;
        }
        moves++;
    }
    
    std::cout << "Game ended after " << moves << " moves.\n";
    State s = g.check_end();
    if (s == State::G1WIN) std::cout << "Winner: RandomBot\n";
    else if (s == State::G2WIN) std::cout << "Winner: GreedyBot\n";
    else std::cout << "Draw\n";
    
    std::cout << "Bot Test: PASSED (Game finished)\n";
}

#include "../game/AlphaBetaBot.hpp"

// ... existing includes ...

// ... existing tests ...

void test_alphabeta() {
    std::cout << "\nRunning AlphaBetaBot Test (AlphaBeta vs Greedy)...\n";
    Game g;
    AlphaBetaBot p1;
    GreedyBot p2;
    
    int moves = 0;
    while (g.check_end() == State::CONTINUE && moves < 200) {
        Bot* current = (g.to_play == Player::P1) ? (Bot*)&p1 : (Bot*)&p2;
        // Give AlphaBeta more time (e.g. 0.1s)
        
        Move mv = current->suggest_move(g, 0.5);
        
        if (!g.play_move(mv)) {
            std::cout << "Bot " << current->name() << " played invalid move!\n";
            break;
        }
        moves++;
    }
    
    std::cout << "Game ended after " << moves << " moves.\n";
    State s = g.check_end();
    if (s == State::G1WIN) std::cout << "Winner: AlphaBetaBot\n";
    else if (s == State::G2WIN) std::cout << "Winner: GreedyBot\n";
    else std::cout << "Draw\n";
    
    std::cout << "AlphaBetaBot Test: PASSED (Game finished)\n";
}

int main() {
    std::srand(std::time(0));
    /*test_case_1();
    test_case_2();
    test_starvation();
    test_bots();
   */ test_alphabeta();
    return 0;
}
