#include <iostream>
#include <string>
#include <unistd.h>
#include "Game.hpp"
#include "AlphaBetaBot.hpp"
#include "protocol.hpp"

int main() {
    Game game;
    AlphaBetaBot bot;
    const double time = 2.8;
    
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        
        if (is_start_command(line)) {
            // Reset game
            game = Game();
            
            // If it's P1's turn (game starts with P1), play first move
            if (game.to_play == Player::P1) {
                // Redirect stdout to stderr to suppress debug output
                int stdout_copy = dup(STDOUT_FILENO);
                dup2(STDERR_FILENO, STDOUT_FILENO);
                
                // Use 2.5s to stay under 3s Arbitre timeout
                Move my_move = bot.suggest_move(game, time);
                
                // Restore stdout
                fflush(stdout);
                dup2(stdout_copy, STDOUT_FILENO);
                close(stdout_copy);
                
                game.play_move(my_move);
                std::cout << format_move(my_move) << std::endl;
            } else {
                // P2 just acknowledges and waits
                //std::cout << "OK" << std::endl;
            }
            std::cout.flush();
            continue;
        }
        
        
        // Check if it's a move (starts with a digit: 1R, 5TR, etc.)
        if (!line.empty() && std::isdigit(line[0])) {
            // Parse and apply opponent's move
            Move opp_move = parse_move(line);
            game.play_move(opp_move);
            
            // Check if game ended after opponent's move
            State s = game.check_end();
            if (s != State::CONTINUE) {
                std::cout << format_result(game, opp_move) << std::endl;
                continue;
            }
            
            // Compute my move (redirect stdout to suppress debug)
            int stdout_copy = dup(STDOUT_FILENO);
            dup2(STDERR_FILENO, STDOUT_FILENO);
            
            // Use 2.5s to stay under 3s Arbitre timeout
            Move my_move = bot.suggest_move(game, time);
            
            // Restore stdout
            fflush(stdout);
            dup2(stdout_copy, STDOUT_FILENO);
            close(stdout_copy);
            
            // Apply my move
            game.play_move(my_move);
            
            // Check if game ended after my move
            s = game.check_end();
            if (s != State::CONTINUE) {
                std::cout << format_result(game, my_move) << std::endl;
            } else {
                // Send my move
                std::cout << format_move(my_move) << std::endl;
            }
            
            std::cout.flush();
        }
    }
    
    return 0;
}
