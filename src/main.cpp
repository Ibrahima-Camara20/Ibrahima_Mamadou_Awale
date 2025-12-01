#include <iostream>
#include <string>
#include <limits>

#include "game/Game.hpp"
#include "io/Parser.hpp"
#include "game/AlphaBetaBot.hpp"

int main() {
    Game g;
    AlphaBetaBot bot;
    
    std::cout << "=== Nouveau jeu AwaleX ===\n";
    std::cout << "Choisissez votre joueur (1 ou 2) : ";
    int human_player_idx;
    if (!(std::cin >> human_player_idx)) {
        human_player_idx = 1; // Default to 1
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer
    
    Player human = (human_player_idx == 2) ? Player::P2 : Player::P1;
    Player ai = (human == Player::P1) ? Player::P2 : Player::P1;
    
    std::cout << "Vous etes Joueur " << (human == Player::P1 ? "1" : "2") << "\n";
    std::cout << "L'IA est Joueur " << (ai == Player::P1 ? "1" : "2") << "\n";

    g.print_board();

    std::string line;

    while (true) {
        // Check end game
        State s = g.check_end();
        if (s != State::CONTINUE) {
            if (s == State::G1WIN) std::cout << ">>> Joueur 1 gagne !\n";
            else if (s == State::G2WIN) std::cout << ">>> Joueur 2 gagne !\n";
            else std::cout << ">>> Match nul !\n";
            break;
        }

        if (g.to_play == human) {
            std::cout << "\n[VOUS] Entrez un coup (ex: 3R, 14B, 4TR, 8TB) ou Q (q) pour quitter: ";
            std::getline(std::cin, line);

            if (line.empty()) continue;

            // Quitter
            if (line.size() >= 1 && toupper(line[0]) == 'Q')
                break;

            Move mv;
            if (!Parser::parse(line, mv)) {
                std::cout << "Coup invalide.\n";
                continue;
            }

            if (!g.play_move(mv)) {
                std::cout << "Coup illegal (mauvais trou ou vide).\n";
                continue;
            }
        } else {
            std::cout << "\n[IA] Reflechit...\n";
            // 1.0 second time limit for the bot
            Move mv = bot.suggest_move(g, 1.0);
            
            // Convert move to string for display
            std::string type_str;
            switch(mv.type) {
                case MoveType::R: type_str = "R"; break;
                case MoveType::B: type_str = "B"; break;
                case MoveType::TR: type_str = "TR"; break;
                case MoveType::TB: type_str = "TB"; break;
            }
            std::cout << ">>> L'IA joue : " << (mv.src + 1) << type_str << "\n";
            
            if (!g.play_move(mv)) {
                std::cout << "ERREUR: L'IA a joue un coup illegal !\n";
                break;
            }
        }

        g.print_board();
    }

    std::cout << "Fin de partie.\n";
    return 0;
}
