#ifndef BOT_FACTORY_HPP
#define BOT_FACTORY_HPP

#include <memory>
#include <string>
#include "Bot.hpp"

class BotFactory {
public:
    static std::unique_ptr<Bot> create(int choice);
    static std::string get_name(int choice);
};

#endif
