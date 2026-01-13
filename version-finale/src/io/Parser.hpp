#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include "../game/Move.hpp"


class Parser {
public:
    static bool parse(const std::string& s, Move& mv);
};

#endif
