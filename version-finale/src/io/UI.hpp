#ifndef UI_HPP
#define UI_HPP

#include <memory>
#include <string>
#include "../game/Bot.hpp"
#include "../net/MQTTClient.hpp"

struct GameConfig {
    std::unique_ptr<Bot> bot1;
    std::unique_ptr<Bot> bot2;
    std::unique_ptr<MQTTClientWrapper> mqtt_client;
    
    bool online_mode = false;
    std::string topic_publish;
    std::string topic_subscribe;
    int my_role = 0; // 1 or 2
    
    // Time limit for bots in seconds (configurable in main)
    double bot_thinking_time = 1.0;
};

class UI {
public:
    static GameConfig configure_game();
    static int get_player_choice(int player_num);
};

#endif
