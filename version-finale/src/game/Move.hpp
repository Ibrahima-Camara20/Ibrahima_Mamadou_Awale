#ifndef MOVE_HPP
#define MOVE_HPP

#include "Rules.hpp"
#include <cstdint>

struct Move {
    uint8_t src;        // index 0..15
    MoveType type;  // R, B, TR, TB
};

#endif
