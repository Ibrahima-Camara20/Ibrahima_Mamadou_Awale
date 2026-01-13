#include "GameEngine.hpp"
#include "../io/Parser.hpp"
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

std::string GameEngine::move_to_json(const Move& mv) {
    std::string type_str;
    switch(mv.type) {
        case MoveType::R: type_str = "R"; break;
        case MoveType::B: type_str = "B"; break;
        case MoveType::TR: type_str = "TR"; break;
        case MoveType::TB: type_str = "TB"; break;
    }
    std::stringstream ss;
    ss << "{\"src\": " << (int)mv.src << ", \"type\": \"" << type_str << "\"}";
    return ss.str();
}

void GameEngine::run(GameConfig config) {
    Game g;
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
        Bot* current_bot = is_p1 ? config.bot1.get() : config.bot2.get();
        
        bool is_remote_turn = (current_bot && current_bot->name().find("Network") != std::string::npos);
        bool is_my_turn = !is_remote_turn;

        Move mv;
        bool move_generated = false;

        if (!current_bot) { // Human
            std::cout << "\n[" << (is_p1 ? "J1" : "J2") << "] Entrez un coup (ex: 3R, 14B, 4TR, 8TB) ou Q (q) pour quitter: ";
            std::getline(std::cin, line);

            if (line.empty()) continue;
            if (line.size() >= 1 && toupper(line[0]) == 'Q') break;

            if (!Parser::parse(line, mv)) {
                std::cout << "Coup invalide.\n";
                continue;
            }
            move_generated = true;
        } else {
            // Bot or Opponent
            if (!is_remote_turn)
                 std::cout << "\n[" << (is_p1 ? "J1" : "J2") << " - " << current_bot->name() << "] ...";
            
            if (!is_remote_turn)
                 std::this_thread::sleep_for(std::chrono::milliseconds(500));

            mv = current_bot->suggest_move(g, config.bot_thinking_time);
            move_generated = true;
            
            std::string type_str;
            switch(mv.type) {
                case MoveType::R: type_str = "R"; break;
                case MoveType::B: type_str = "B"; break;
                case MoveType::TR: type_str = "TR"; break;
                case MoveType::TB: type_str = "TB"; break;
            }
            std::cout << (mv.src + 1) << type_str << "\n";
        }

        if (move_generated) {
            if (g.play_move(mv)) {
                if (config.online_mode && is_my_turn && config.mqtt_client) {
                    std::string json = move_to_json(mv);
                    config.mqtt_client->publish(config.topic_publish, json);
                }
                g.print_board();
            } else {
                std::cout << "Coup illegal (ou erreur de synchro) !\n";
                if (!current_bot) continue; 
                else break; 
            }
        }
    }
    std::cout << "Fin de partie.\n";
}
