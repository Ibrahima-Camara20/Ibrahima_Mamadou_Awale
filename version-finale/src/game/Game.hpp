#ifndef GAME_HPP
#define GAME_HPP

#include "Move.hpp"
#include "Rules.hpp"
#include <array>
#include <cstdint>
#include <string>
#include <vector>

struct Hole {
  uint8_t r = 0;
  uint8_t b = 0;
  uint8_t t = 0;
};

class Game {
public:
  Game();

  // Affichage
  void print_board() const;

  // Logique principale
  bool play_move(const Move &mv);
  bool parse_move(const std::string &s, Move &mv) const;
  State check_end() const;

  // Génération des coups
  std::vector<Move> generate_moves(Player p) const;

  // Données publiques
  // Use std::array or C-style array for fixed size and better locality
  Hole h[16];
  uint8_t cap[2];
  uint8_t moves[2];
  Player to_play;

  int sum_board() const;
  bool is_owned(Player p, int idx) const;

  struct GameState {
      Hole h[16];
      uint8_t cap[2];
      uint8_t moves[2];
      Player to_play;
      uint64_t hash;
  };

  GameState store_state() const;
  void restore_state(const GameState& s);

  // Zobrist Hashing
  uint64_t hash;
  void compute_hash();
  static uint64_t z_keys[16][3][50]; // [Hole][Color][Count]
  static uint64_t z_turn;            // Hash used when it's P2's turn
  static void init_zobrist();

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
