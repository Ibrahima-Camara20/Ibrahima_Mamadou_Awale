#include "Game.hpp"
#include "../io/Parser.hpp"
#include <cctype>
#include <algorithm>
#include <iomanip>
#include <cstdint>
#include <iostream>

#include <random>

// Static members
uint64_t Game::z_keys[16][3][50];
uint64_t Game::z_turn;

void Game::init_zobrist() {
    static bool initialized = false;
    if (initialized) return;
    
    std::mt19937_64 rng(12345); // Fixed seed for reproducibility
    
    for(int i=0; i<16; ++i) {
        for(int j=0; j<3; ++j) {
            for(int k=0; k<50; ++k) {
                z_keys[i][j][k] = rng();
            }
        }
    }
    z_turn = rng();
    initialized = true;
}

Game::Game() : to_play(Player::P1) { 
    init_zobrist();
    init_game(); 
}

void Game::compute_hash() {
    hash = 0;
    for(int i=0; i<NHOLES; ++i) {
        int r = h[i].r;
        int b = h[i].b;
        int t = h[i].t;
        
        if (r >= 50) r = 49; // Cap for hash safety
        if (b >= 50) b = 49;
        if (t >= 50) t = 49;
        
        hash ^= z_keys[i][0][r];
        hash ^= z_keys[i][1][b];
        hash ^= z_keys[i][2][t];
    }
    
    if (to_play == Player::P2) {
        hash ^= z_turn;
    }
}

void Game::init_game() {
  for (auto &cell : h)
    cell = {INITIAL_SEEDS_PER_COLOR, INITIAL_SEEDS_PER_COLOR,
            INITIAL_SEEDS_PER_COLOR};

  cap[0] = cap[1] = 0;
  moves[0] = moves[1] = 0;
  to_play = Player::P1;
  compute_hash();
}

int Game::wrap(int i) const { return (i + NHOLES) % NHOLES; }

bool Game::is_owned(Player p, int idx) const {
  int n = idx + 1;
  return (p == Player::P1) ? (n % 2 == 1) : (n % 2 == 0);
}

int Game::sum_hole(int idx) const { return h[idx].r + h[idx].b + h[idx].t; }

int Game::sum_board() const {
  int s = 0;
  for (int i = 0; i < NHOLES; i++)
    s += sum_hole(i);
  return s;
}
void Game::print_board() const {
  std::cout << "\n=========== PLATEAU ===========\n";

  // Ligne du haut
  for (int i = 0; i < 8; i++) {
    const Hole &c = h[i];
    std::cout << std::setw(2) << i + 1 << "(";

    if (sum_hole(i) == 0)
      std::cout << ")";
    else {
      if (c.r)
        std::cout << (int)c.r << "R";
      if (c.b)
        std::cout << (int)c.b << "B";
      if (c.t)
        std::cout << (int)c.t << "T";
      std::cout << ")";
    }
    std::cout << "  ";
  }
  std::cout << "\n";

  // Ligne du bas
  for (int i = 15; i >= 8; i--) {
    const Hole &c = h[i];
    std::cout << std::setw(2) << i + 1 << "(";

    if (sum_hole(i) == 0)
      std::cout << ")";
    else {
      if (c.r)
        std::cout << (int)c.r << "R";
      if (c.b)
        std::cout << (int)c.b << "B";
      if (c.t)
        std::cout << (int)c.t << "T";
      std::cout << ")";
    }
    std::cout << "  ";
  }
  std::cout << "\n";

  std::cout << "Captures -> P1:" << (int)cap[0] << "  P2:" << (int)cap[1]
            << "  | Moves -> P1:" << (int)moves[0] << "  P2:" << (int)moves[1]
            << "  | À jouer: "
            << (to_play == Player::P1 ? "Joueur 1" : "Joueur 2") << "\n";
}

// ---------------------------
// Génération des coups
// ---------------------------
std::vector<Move> Game::generate_moves(Player p) const {
  std::vector<Move> moves;

  for (int i = 0; i < NHOLES; i++) {
    if (!is_owned(p, i))
      continue;

    const Hole &cell = h[i];

    if (cell.r > 0)
      moves.push_back({(uint8_t)i, MoveType::R});
    if (cell.b > 0)
      moves.push_back({(uint8_t)i, MoveType::B});
    if (cell.t > 0) {
      moves.push_back({(uint8_t)i, MoveType::TR});
      moves.push_back({(uint8_t)i, MoveType::TB});
    }
  }

  return moves;
}

// ---------------------------
// Parser utilisateur
// ---------------------------
bool Game::parse_move(const std::string &s, Move &mv) const {
  return Parser::parse(s, mv);
}

// ---------------------------
// Semis
// ---------------------------
int Game::next_drop_idx(Player p, int src, Behav beh, int cur) const {
  int idx = wrap(cur + 1);

  if (idx == src)
    idx = wrap(idx + 1);

  if (beh == Behav::RED)
    return idx;

  while (is_owned(p, idx) || idx == src) {
    idx = wrap(idx + 1);
    if (idx == src)
      idx = wrap(idx + 1);
  }

  return idx;
}

int Game::sow_color(int src, Behav beh, char color, int count, int start,
                    Player p) {
  int dst = start;

  for (int i = 0; i < count; i++) {
    dst = next_drop_idx(p, src, beh, dst);
    Hole &cell = h[dst];

    if (color == 'R')
      cell.r++;
    else if (color == 'B')
      cell.b++;
    else
      cell.t++;

    // Safety check to avoid infinite sow loops if count is erroneous (e.g.
    // logic bug)
    if (i > 1000)
      break;
  }

  return dst;
}

// ---------------------------
// Captures
// ---------------------------
void Game::apply_capture(Player p, int last) {
  int idx = last;
  int gained = 0;

  while (true) {
    int s = sum_hole(idx);

    if (s == 2 || s == 3) {
      gained += s;
      h[idx] = {0, 0, 0};
      idx = wrap(idx - 1);
    } else
      break;
  }

  cap[(int)p] += gained;
}

// ---------------------------
// Vérification fin
// ---------------------------
State Game::check_end() const {
    if (cap[0] >= 49)
        return State::G1WIN;
    if (cap[1] >= 49)
        return State::G2WIN;

    if (sum_board() < END_BOARD_SEEDS) {
        if (cap[0] > cap[1])
        return State::G1WIN;
        if (cap[1] > cap[0])
        return State::G2WIN;
        return State::DRAW;
    }
    
    if (moves[0] >= MAX_MOVES_PER_PLAYER && moves[1] >= MAX_MOVES_PER_PLAYER) {
        if (cap[0] > cap[1])
        return State::G1WIN;
        if (cap[1] > cap[0])
        return State::G2WIN;
        return State::DRAW;
    }

    if (generate_moves(to_play).empty()) {
        // Should be handled by starvation check in play_move, but as fallback:
        return (to_play == Player::P1) ? State::G2WIN : State::G1WIN;
    }

    return State::CONTINUE;
}
// ---------------------------
// Exécuter un coup complet
// ---------------------------
bool Game::play_move(const Move &mv) {
  Player p = to_play;
  int src = mv.src;

  if (!is_owned(p, src))
    return false;

  Hole &H = h[src];
  Behav beh = (mv.type == MoveType::R || mv.type == MoveType::TR) ? Behav::RED
                                                                  : Behav::BLUE;

  int reds = 0, blues = 0, trans = 0;

  switch (mv.type) {
  case MoveType::R:
    if (H.r == 0)
      return false;
    reds = H.r;
    H.r = 0;
    break;

  case MoveType::B:
    if (H.b == 0)
      return false;
    blues = H.b;
    H.b = 0;
    break;

  case MoveType::TR:
    if (H.t == 0)
      return false;
    trans = H.t;
    reds = H.r;
    H.t = H.r = 0;
    break;

  case MoveType::TB:
    if (H.t == 0)
      return false;
    trans = H.t;
    blues = H.b;
    H.t = H.b = 0;
    break;
  }

  int cur = src;
  int last = -1;

  if (trans > 0)
    last = sow_color(src, beh, 'T', trans, cur, p), cur = last;

  if (reds > 0)
    last = sow_color(src, Behav::RED, 'R', reds, cur, p), cur = last;

  if (blues > 0)
    last = sow_color(src, Behav::BLUE, 'B', blues, cur, p), cur = last;

  if (last >= 0)
    apply_capture(p, last);

  // Check for starvation
  Player opponent = (p == Player::P1 ? Player::P2 : Player::P1);
  if (generate_moves(opponent).empty()) {
    int remaining = 0;
    for (auto &cell : h) {
      remaining += cell.r + cell.b + cell.t;
      cell = {0, 0, 0};
    }
    cap[(int)p] += remaining;
  }

  moves[(int)p]++;
  to_play = opponent;
  compute_hash();
  return true;
}

Game::GameState Game::store_state() const {
    GameState s;
    std::copy(&h[0], &h[16], s.h);
    s.cap[0] = cap[0];
    s.cap[1] = cap[1];
    s.moves[0] = moves[0];
    s.moves[1] = moves[1];
    s.to_play = to_play;
    s.hash = hash;
    return s;
}

void Game::restore_state(const GameState& s) {
    std::copy(&s.h[0], &s.h[16], h);
    cap[0] = s.cap[0];
    cap[1] = s.cap[1];
    moves[0] = s.moves[0];
    moves[1] = s.moves[1];
    to_play = s.to_play;
    hash = s.hash;
}
