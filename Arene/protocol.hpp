#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <string>
#include <sstream>
#include "Game.hpp"
#include "Rules.hpp"

inline bool is_start_command(const std::string& line) {
    return line == "START";
}

inline Move parse_move(const std::string& line) {
    // New format: "1R", "5TR", "12B", etc.
    // Extract number and type
    std::string num_str;
    std::string type_str;
    
    size_t i = 0;
    // Get digits
    while (i < line.length() && std::isdigit(line[i])) {
        num_str += line[i];
        i++;
    }
    
    // Get remaining as type
    while (i < line.length()) {
        type_str += line[i];
        i++;
    }
    
    int src = std::stoi(num_str);
    
    MoveType type;
    if (type_str == "R") type = MoveType::R;
    else if (type_str == "B") type = MoveType::B;
    else if (type_str == "TR") type = MoveType::TR;
    else type = MoveType::TB;
    
    // Convert from user format (1-16) to internal (0-15)
    return {(uint8_t)(src - 1), type};
}


inline std::string format_move(const Move& mv) {
    std::stringstream ss;
    ss << (int)mv.src + 1;
    
    if (mv.type == MoveType::R) ss << "R";
    else if (mv.type == MoveType::B) ss << "B";
    else if (mv.type == MoveType::TR) ss << "TR";
    else ss << "TB";
    
    return ss.str();
}

inline std::string format_result(const Game& g, const Move& last_move) {
    std::stringstream ss;
    ss << "RESULT ";
    
    // Format: RESULT <last_move> <scoreJ1> <scoreJ2>
    ss << format_move(last_move);
    
    // Add scores
    ss << " " << (int)g.cap[0] << " " << (int)g.cap[1];
    
    return ss.str();
}

inline void apply_opponent_move(Game& g, const std::string& line) {
    Move mv = parse_move(line);
    g.play_move(mv);
}

#endif
