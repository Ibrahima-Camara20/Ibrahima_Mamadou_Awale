#ifndef RULES_HPP
#define RULES_HPP

constexpr int NHOLES = 16;

// Joueurs
enum class Player { P1 = 0, P2 = 1 };

// Comportement (rouge ou bleu)
enum class Behav { RED = 0, BLUE = 1 };

// Type de mouvement
// how much space in memory this enum takes ?
// https://stackoverflow.com/questions/98650/how-do-i-determine-the-size-of-an-enum-type-in-c

enum class MoveType { R, B, TR, TB };

// États du jeu
enum class State { CONTINUE, G1WIN, G2WIN, DRAW };

#endif
