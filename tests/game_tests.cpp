#include "../src/game/Game.hpp"
#include "../src/io/Parser.hpp"
#include <cassert>
#include <iostream>

void test_initial_state() {
    Game g;
    int total = 0;
    for (const auto &cell : g.h) total += cell.r + cell.b + cell.t;
    assert(total == 96 && "Initial total seeds should be 96");

    auto moves_p1 = g.generate_moves(Player::P1);
    auto moves_p2 = g.generate_moves(Player::P2);
    // Each owned hole initially offers R,B,TR,TB so 4 moves per hole, 8 holes per player => 32
    assert((int)moves_p1.size() == 32 && "Player 1 should have 32 moves at start");
    assert((int)moves_p2.size() == 32 && "Player 2 should have 32 moves at start");
}

void test_parser() {
    Move mv;
    assert(Parser::parse("3R", mv) && mv.src == 2 && mv.type == MoveType::R);
    assert(Parser::parse("12TR", mv) && mv.src == 11 && mv.type == MoveType::TR);
    assert(!Parser::parse("0R", mv));
    assert(!Parser::parse("17R", mv));
    assert(!Parser::parse("5X", mv));
    assert(Parser::parse("  8tb  ", mv) && mv.src == 7 && mv.type == MoveType::TB);
}

void test_play_and_source_cleared() {
    Game g;
    auto moves = g.generate_moves(Player::P1);
    Move mv = moves[0];
    int src = mv.src;

    bool ok = g.play_move(mv);
    assert(ok);
    // depending on move type, the corresponding count at source must be zero
    if (mv.type == MoveType::R) assert(g.h[src].r == 0);
    if (mv.type == MoveType::B) assert(g.h[src].b == 0);
    if (mv.type == MoveType::TR || mv.type == MoveType::TB) assert(g.h[src].t == 0);
}

void test_capture_behavior() {
    Game g;
    // Clear board
    for (auto &c : g.h) c = {0,0,0};
    // Setup: source at 0 with 1 red so it will drop into hole 1
    g.h[0].r = 1;
    // make hole 1 have sum == 2 so after drop it becomes 3 and will be captured
    g.h[1].r = 2; // sum 2
    g.to_play = Player::P1;

    Move mv{0, MoveType::R};
    bool ok = g.play_move(mv);
    assert(ok);
    // hole 1 should be captured (zeroed) and cap for P1 increased by 3
    assert(g.h[1].r + g.h[1].b + g.h[1].t == 0);
    assert(g.cap[0] == 3);
}

void test_end_conditions() {
    Game g;
    // win by captures
    g.cap[0] = 49;
    assert(g.check_end() == State::G1WIN);

    // reset and test board depletion rule
    g.cap[0] = g.cap[1] = 0;
    for (auto &c : g.h) c = {0,0,0};
    // with empty board, sum_board < 10 and caps equal -> DRAW
    assert(g.check_end() == State::DRAW);

    // no moves for player
    Game g2;
    for (int i = 0; i < (int)g2.h.size(); ++i) {
        // ownership: Player 1 owns odd-numbered holes (index+1 odd)
        if (((i + 1) % 2) == 1) g2.h[i] = {0,0,0};
    }
    // Player::P1 has no moves -> generate_moves empty
    assert(g2.generate_moves(Player::P1).empty());
}

int main() {
    test_initial_state();
    test_parser();
    test_play_and_source_cleared();
    test_capture_behavior();
    test_end_conditions();

    std::cout << "All tests passed.\n";
    return 0;
}
