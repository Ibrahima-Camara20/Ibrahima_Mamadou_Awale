#ifndef MOVE_HPP
#define MOVE_HPP

#include "Rules.hpp"

struct Move {
    int src;        // index 0..15
    MoveType type;  // R, B, TR, TB
};

#endif
