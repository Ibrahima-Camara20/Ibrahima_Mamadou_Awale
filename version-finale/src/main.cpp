#include "io/UI.hpp"
#include "game/GameEngine.hpp"

int main() {
    auto config = UI::configure_game();
    
    // Parametre global du temps de reflexion des bots (en secondes)
    config.bot_thinking_time = 0.3;
    
    GameEngine::run(std::move(config));
    return 0;
}