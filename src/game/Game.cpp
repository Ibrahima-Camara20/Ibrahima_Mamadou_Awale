#include "Game.hpp"
#include "../io/Parser.hpp"
#include <iostream>
#include <iomanip>
#include <cctype>

Game::Game() : h(NHOLES), to_play(Player::P1) {
    init_game();
}

void Game::init_game() {
    for (auto &cell : h)
        cell = {2, 2, 2};

    cap[0] = cap[1] = 0;
    to_play = Player::P1;
}

int Game::wrap(int i) const {
    return (i + NHOLES) % NHOLES;
}

bool Game::is_owned(Player p, int idx) const {
    int n = idx + 1;
    return (p == Player::P1) ? (n % 2 == 1) : (n % 2 == 0);
}

int Game::sum_hole(int idx) const {
    return h[idx].r + h[idx].b + h[idx].t;
}

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

        if (sum_hole(i) == 0) std::cout << ")";
        else {
            if (c.r) std::cout << c.r << "R";
            if (c.b) std::cout << c.b << "B";
            if (c.t) std::cout << c.t << "T";
            std::cout << ")";
        }
        std::cout << "  ";
    }
    std::cout << "\n";

    // Ligne du bas
    for (int i = 15; i >= 8; i--) {
        const Hole &c = h[i];
        std::cout << std::setw(2) << i + 1 << "(";

        if (sum_hole(i) == 0) std::cout << ")";
        else {
            if (c.r) std::cout << c.r << "R";
            if (c.b) std::cout << c.b << "B";
            if (c.t) std::cout << c.t << "T";
            std::cout << ")";
        }
        std::cout << "  ";
    }
    std::cout << "\n";

    std::cout << "Captures -> P1:" << cap[0]
              << "  P2:" << cap[1]
              << "  | À jouer: "
              << (to_play == Player::P1 ? "Joueur 1" : "Joueur 2")
              << "\n";
}

// ---------------------------
// Génération des coups
// ---------------------------
std::vector<Move> Game::generate_moves(Player p) const {
    std::vector<Move> moves;

    for (int i = 0; i < NHOLES; i++) {
        if (!is_owned(p, i)) continue;

        const Hole &cell = h[i];

        if (cell.r > 0) moves.push_back({i, MoveType::R});
        if (cell.b > 0) moves.push_back({i, MoveType::B});
        if (cell.t > 0) {
            moves.push_back({i, MoveType::TR});
            moves.push_back({i, MoveType::TB});
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

    if (idx == src) idx = wrap(idx + 1);

    if (beh == Behav::RED) return idx;

    while (is_owned(p, idx) || idx == src) {
        idx = wrap(idx + 1);
        if (idx == src) idx = wrap(idx + 1);
    }

    return idx;
}

int Game::sow_color(int src, Behav beh, char color, int count, int start, Player p) {
    int dst = start;

    for (int i = 0; i < count; i++) {
        dst = next_drop_idx(p, src, beh, dst);
        Hole &cell = h[dst];

        if (color == 'R') cell.r++;
        else if (color == 'B') cell.b++;
        else cell.t++;
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
        }
        else break;
    }

    cap[(int)p] += gained;
}

// ---------------------------
// Vérification fin
// ---------------------------
State Game::check_end() const {
    if (cap[0] >= 49) return State::G1WIN;
    if (cap[1] >= 49) return State::G2WIN;
    
    // Draw if both have >= 40
    if (cap[0] >= 40 && cap[1] >= 40) return State::DRAW;

    if (sum_board() < 10) {
        if (cap[0] > cap[1]) return State::G1WIN;
        if (cap[1] > cap[0]) return State::G2WIN;
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

    if (!is_owned(p, src)) return false;

    Hole &H = h[src];
    Behav beh = (mv.type == MoveType::R || mv.type == MoveType::TR)
                ? Behav::RED : Behav::BLUE;

    int reds = 0, blues = 0, trans = 0;

    switch (mv.type) {
        case MoveType::R:
            if (H.r == 0) return false;
            reds = H.r; H.r = 0;
            break;

        case MoveType::B:
            if (H.b == 0) return false;
            blues = H.b; H.b = 0;
            break;

        case MoveType::TR:
            if (H.t == 0) return false;
            trans = H.t;
            reds = H.r;
            H.t = H.r = 0;
            break;

        case MoveType::TB:
            if (H.t == 0) return false;
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

    to_play = opponent;
    return true;
}
