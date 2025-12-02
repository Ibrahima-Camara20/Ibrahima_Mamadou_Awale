#include <iostream>
#include <string>
#include <limits>
#include <memory>
#include <thread>
#include <chrono>
#include <algorithm>

#include "game/Game.hpp"
#include "io/Parser.hpp"
#include "game/Bot.hpp"
#include "game/AlphaBetaBot.hpp"

// Helper to create a bot based on selection
std::unique_ptr<Bot> create_bot(int choice) {
    switch (choice) {
        case 2: return std::make_unique<RandomBot>();
        case 3: return std::make_unique<GreedyBot>();
        case 4: return std::make_unique<AlphaBetaBot>();
        default: return nullptr;
    }
}

std::string get_player_type_name(int choice) {
    switch (choice) {
        case 1: return "Humain";
        case 2: return "RandomBot";
        case 3: return "GreedyBot";
        case 4: return "AlphaBetaBot";
        default: return "Inconnu";
    }
}

int get_player_choice(int player_num) {
    int choice;
    while (true) {
        std::cout << "Type de Joueur " << player_num << " :\n";
        std::cout << "  1. Humain\n";
        std::cout << "  2. RandomBot\n";
        std::cout << "  3. GreedyBot\n";
        std::cout << "  4. AlphaBetaBot\n";
        std::cout << "Votre choix (1-4) : ";
        if (std::cin >> choice && choice >= 1 && choice <= 4) {
            return choice;
        }
        std::cout << "Choix invalide. Veuillez reessayer.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

int main() {
    std::cout << "=== Nouveau jeu AwaleX ===\n";

    int p1_choice = get_player_choice(1);
    int p2_choice = get_player_choice(2);

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer

    Game g;
    std::unique_ptr<Bot> bot1 = create_bot(p1_choice);
    std::unique_ptr<Bot> bot2 = create_bot(p2_choice);

    std::cout << "\n--- Configuration ---\n";
    std::cout << "Joueur 1 : " << get_player_type_name(p1_choice) << "\n";
    std::cout << "Joueur 2 : " << get_player_type_name(p2_choice) << "\n";
    std::cout << "---------------------\n\n";

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

        bool is_p1 = (g.to_play == Player::P1);
        bool is_human = (is_p1 && !bot1) || (!is_p1 && !bot2);
        Bot* current_bot = is_p1 ? bot1.get() : bot2.get();

        if (is_human) {
            std::cout << "\n[" << (is_p1 ? "J1" : "J2") << "] Entrez un coup (ex: 3R, 14B, 4TR, 8TB) ou Q (q) pour quitter: ";
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
            std::cout << "\n[" << (is_p1 ? "J1" : "J2") << " - " << current_bot->name() << "] Reflechit...\n";
            
            // Add delay for visibility if it's AI vs AI or just to make it feel natural
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            // 1.0 second time limit for the bot
            Move mv = current_bot->suggest_move(g, 1);
            
            // Convert move to string for display
            std::string type_str;
            switch(mv.type) {
                case MoveType::R: type_str = "R"; break;
                case MoveType::B: type_str = "B"; break;
                case MoveType::TR: type_str = "TR"; break;
                case MoveType::TB: type_str = "TB"; break;
            }
            std::cout << ">>> Joue : " << (mv.src + 1) << type_str << "\n";
            
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
