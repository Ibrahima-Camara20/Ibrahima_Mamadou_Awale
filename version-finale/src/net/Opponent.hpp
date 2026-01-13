#ifndef OPPONENT_HPP
#define OPPONENT_HPP

#include "../game/Bot.hpp"
#include "MQTTClient.hpp"
#include <string>

class Opponent : public Bot {
public:
    Opponent(MQTTClientWrapper& client, const std::string& topic);
    
    std::string name() const override { return "Opponent (Network)"; }
    
    Move suggest_move(const Game& g, double time_limit) override;

private:
    MQTTClientWrapper& client;
    std::string topic;
};

#endif
