#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include <string>
#include "Rules.hpp"
#include "Move.hpp"

struct Hole {
    int r = 0;
    int b = 0;
    int t = 0;
};

class Game {
public:
    Game();

    // Affichage
    void print_board() const;

    // Logique principale
    bool play_move(const Move& mv);
    bool parse_move(const std::string& s, Move& mv) const;
    State check_end() const;

    // Génération des coups
    std::vector<Move> generate_moves(Player p) const;

    // Données publiques
    std::vector<Hole> h;
    int cap[2];
    Player to_play;
    
    int sum_board() const;
    bool is_owned(Player p, int idx) const;

private:
    // Fonctions utilitaires (gardaient static en C)
    int wrap(int i) const;
    int sum_hole(int idx) const;

    // Semis
    int next_drop_idx(Player p, int src, Behav beh, int cur) const;
    int sow_color(int src, Behav beh, char color, int count, int cur, Player p);

    // Captures
    void apply_capture(Player p, int last);

    // Setup
    void init_game();
};

#endif
