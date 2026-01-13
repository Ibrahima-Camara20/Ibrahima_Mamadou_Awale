#ifndef GAME_ENGINE_HPP
#define GAME_ENGINE_HPP

#include "Game.hpp"
#include "../io/UI.hpp"
#include <string>

class GameEngine {
public:
    static void run(GameConfig config);

private:
    static std::string move_to_json(const Move& mv);
};

#endif
