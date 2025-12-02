#include "rules.hpp"
#include <iostream>
#include <string>
#include <iomanip>

// Helper to visualize board
void print_state(const Rules2025Engine& engine) {
    const auto& b = engine.get_board();
    std::cout << "\n=================================================\n";
    std::cout << "Turn: " << (engine.get_turn() == Player::Odd? "ODD (P1)" : "EVEN (P2)") << "\n";
    std::cout << "Score Odd: " << engine.get_score(Player::Odd) << " | Score Even: " << engine.get_score(Player::Even) << "\n";
    std::cout << "-------------------------------------------------\n";
    
    // Render Holes 16 down to 9
    std::cout << "Evn(Own): ";
    for(int i=15; i>=8; --i) {
        if (i % 2!= 0) std::cout << "[" << std::setw(2) << (int)b[i].total() << "]"; // Even Player Holes (Odd Indices)
        else std::cout << " . "; 
    }
    std::cout << "\nIndices : ";
    for(int i=15; i>=8; --i) std::cout << std::setw(3) << i + 1 << " ";
    std::cout << "\n          ";
    for(int i=15; i>=8; --i) {
         if (i % 2 == 0) std::cout << "[" << std::setw(2) << (int)b[i].total() << "]"; // Odd Player Holes
         else std::cout << " . ";
    }
    std::cout << " :Odd(Own)\n\n";

    // Render Holes 1 to 8
    std::cout << "Odd(Own): ";
    for(int i=0; i<=7; ++i) {
        if (i % 2 == 0) std::cout << "[" << std::setw(2) << (int)b[i].total() << "]";
        else std::cout << " . ";
    }
    std::cout << "\nIndices : ";
    for(int i=0; i<=7; ++i) std::cout << std::setw(3) << i + 1 << " ";
    std::cout << "\n          ";
    for(int i=0; i<=7; ++i) {
        if (i % 2!= 0) std::cout << "[" << std::setw(2) << (int)b[i].total() << "]";
        else std::cout << " . ";
    }
    std::cout << " :Evn(Own)\n";
    std::cout << "=================================================\n";
}

int main() {
    Rules2025Engine engine;
    
    // --- Verify Case 1 from Research ---
    // Setup specific board state for Case 1
    // Hole 1 (Idx 0): 2R
    // Hole 16 (Idx 15): 2R
    // Hole 15 (Idx 14): 2B
    // Hole 14 (Idx 13): 2B, 2R -> Player Even plays 14B
    
    // Note: Holes 1-16 are 1-based. Indices 0-15.
    // Hole 14 is Index 13.
    
    // We can't manually set board in the class (private), 
    // so in a real test we'd need a debug setter or just run the game.
    // For this demo, we run the standard game loop.

    std::string input;
    while (!engine.is_game_over()) {
        print_state(engine);
        std::cout << "Enter move (e.g., 14B, 4TR, 4TB): ";
        std::cin >> input;

        if (input == "q") break;

        // Parse Input
        try {
            // Extract Hole
            size_t alpha_pos = 0;
            while (alpha_pos < input.size() && isdigit(input[alpha_pos])) alpha_pos++;
            
            int hole_num = std::stoi(input.substr(0, alpha_pos));
            char type_char = input[alpha_pos];
            
            Move m;
            m.hole_idx = hole_num - 1; // Convert 1-based to 0-based
            
            if (type_char == 'R') m.selected_type = SeedType::Red;
            else if (type_char == 'B') m.selected_type = SeedType::Blue;
            else if (type_char == 'T') {
                m.selected_type = SeedType::Trans;
                char decl_char = input[alpha_pos + 1]; // Next char
                if (decl_char == 'R') m.declaration = Behavior::RedLike;
                else if (decl_char == 'B') m.declaration = Behavior::BlueLike;
                else throw std::runtime_error("Invalid Declaration");
            } else {
                throw std::runtime_error("Invalid Type");
            }

            if (!engine.execute_move(m)) {
                std::cout << ">> ILLEGAL MOVE <<\n";
            } else {
                std::cout << ">> Move Executed <<\n";
            }

        } catch (...) {
            std::cout << ">> Parse Error. Format: 14B or 4TR <<\n";
        }
    }
    
    std::cout << "GAME OVER\n";
    std::cout << "Final Score - Odd: " << engine.get_score(Player::Odd) 
              << " Even: " << engine.get_score(Player::Even) << "\n";

    return 0;
}