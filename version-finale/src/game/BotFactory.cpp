#include "BotFactory.hpp"
#include "AlphaBetaBot.hpp"
#include "Bot.hpp"

std::unique_ptr<Bot> BotFactory::create(int choice) {
    switch (choice) {
        case 2: return std::make_unique<GreedyBot>();
        case 3: return std::make_unique<AlphaBetaBot>();
        case 1: return nullptr; // Humain
        default: return nullptr;
    }
}

std::string BotFactory::get_name(int choice) {
    switch (choice) {
        case 1: return "Humain";
        case 2: return "GreedyBot";
        case 3: return "AlphaBetaBot";
        case 4: return "En Ligne (Network)";
        default: return "Inconnu"; // 0 or others
    }
}
