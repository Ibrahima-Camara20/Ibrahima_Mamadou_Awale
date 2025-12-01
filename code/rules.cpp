#include "rules.hpp"
#include <algorithm>

// --- Hole Implementation ---

void Hole::add(SeedType s) {
    switch(s) {
        case SeedType::Red: r++; break;
        case SeedType::Blue: b++; break;
        case SeedType::Trans: t++; break;
    }
}

uint8_t Hole::get_count(SeedType s) const {
    switch(s) {
        case SeedType::Red: return r;
        case SeedType::Blue: return b;
        case SeedType::Trans: return t;
    }
    return 0;
}

void Hole::clear() { r = 0; b = 0; t = 0; }

// --- Engine Implementation ---

Rules2025Engine::Rules2025Engine() {
    reset();
}

void Rules2025Engine::reset() {
    scores = {0, 0};
    current_turn = Player::Odd;
    game_over_flag = false;
    for (auto& h : board) {
        h.r = 2; h.b = 2; h.t = 2; h._padding = 0;
    }
}

Player Rules2025Engine::get_owner(int idx) const {
    // Even indices (0, 2...) are Player Odd (1st player)
    return (idx % 2 == 0)? Player::Odd : Player::Even;
}

int Rules2025Engine::next_physical(int idx) const {
    return (idx + 1) % BOARD_SIZE;
}

int Rules2025Engine::prev_physical(int idx) const {
    return (idx - 1 + BOARD_SIZE) % BOARD_SIZE;
}

bool Rules2025Engine::execute_move(Move m) {
    if (game_over_flag) return false;

    // 1. Validation
    if (get_owner(m.hole_idx)!= current_turn) return false;
    Hole& src_hole = board[m.hole_idx];
    if (src_hole.get_count(m.selected_type) == 0) return false;

    // 2. Build Hand (Stack Optimized)
    // Rule: "Transparent remains transparent and are distributed first"
    std::array<SeedType, MAX_SEEDS> hand;
    int hand_size = 0;

    auto push_seeds = [&](SeedType type, int count) {
        for(int i=0; i<count; ++i) hand[hand_size++] = type;
    };

    if (m.selected_type == SeedType::Trans) {
        push_seeds(SeedType::Trans, src_hole.t);
        src_hole.t = 0;
        
        // "Played with the other seeds of the same color"
        if (m.declaration == Behavior::RedLike) {
            push_seeds(SeedType::Red, src_hole.r);
            src_hole.r = 0;
        } else {
            push_seeds(SeedType::Blue, src_hole.b);
            src_hole.b = 0;
        }
    } 
    else if (m.selected_type == SeedType::Red) {
        push_seeds(SeedType::Red, src_hole.r);
        src_hole.r = 0;
    }
    else { // Blue
        push_seeds(SeedType::Blue, src_hole.b);
        src_hole.b = 0;
    }

    // 3. Sowing
    Behavior active_behavior;
    if (m.selected_type == SeedType::Trans) active_behavior = m.declaration;
    else if (m.selected_type == SeedType::Blue) active_behavior = Behavior::BlueLike;
    else active_behavior = Behavior::RedLike;

    int cursor = m.hole_idx;
    int last_sown = -1;

    for (int k = 0; k < hand_size; ++k) {
        // Step 1: Physical Move
        cursor = next_physical(cursor);

        // Step 2: Skip Start Hole Rule
        // "if a clock turn is made then the initial hole is skipped"
        if (cursor == m.hole_idx) {
            cursor = next_physical(cursor);
        }

        // Step 3: Behavior Logic
        if (active_behavior == Behavior::BlueLike) {
            // "Distributed only in the opponent's holes"
            // If current hole is OWN, we skip dropping, but we consumed the step.
            // Wait - Re-reading Case 1: 
            // 14B -> 13(Opp) Drop, 12(Own) Skip, 11(Opp) Drop.
            // The iterator moves physically. If hole is Own, we DO NOT drop, and we continue loop
            // WITHOUT consuming the seed from hand?
            // NO. If we didn't consume the seed, we'd travel forever.
            // The seed is consumed to traverse the "distance" of the own hole.
            // Actually, in Case 1, 2 seeds covered distance 14->13 and 14->...->1.
            // This implies: We iterate until we find a valid target.
            
            while (get_owner(cursor) == current_turn) {
                cursor = next_physical(cursor);
                // Must also respect Start Hole Skip inside this search
                if (cursor == m.hole_idx) cursor = next_physical(cursor);
            }
        }
        
        // Drop the seed
        board[cursor].add(hand[k]);
        last_sown = cursor;
    }

    // 4. Capture (The Reaper)
    // "Capturing... leads to previous-to-last... (backwards)"
    if (last_sown!= -1) {
        reap(last_sown);
    }

    // 5. Starvation & End Game
    Player opponent = (current_turn == Player::Odd)? Player::Even : Player::Odd;
    
    // Check if opponent is starved
    if (count_seeds_for_player(opponent) == 0) {
        // "In case of starving all the seeds are captured by the last player."
        sweep_board_to_score(current_turn);
    }

    check_game_end();

    if (!game_over_flag) {
        current_turn = opponent;
    }

    return true;
}

void Rules2025Engine::reap(int start_idx) {
    int cursor = start_idx;
    bool chaining = true;

    while (chaining) {
        int tot = board[cursor].total();
        if (tot == 2 || tot == 3) {
            // Capture
            scores[static_cast<int>(current_turn)] += tot;
            board[cursor].clear();
            
            // Move strictly backwards geometrically
            // Even if the hole was skipped during sowing (like Case 1, Hole 16)
            cursor = prev_physical(cursor); 
        } else {
            chaining = false;
        }
    }
}

int Rules2025Engine::count_seeds_for_player(Player p) const {
    int c = 0;
    for (int i=0; i<BOARD_SIZE; ++i) {
        if (get_owner(i) == p) c += board[i].total();
    }
    return c;
}

void Rules2025Engine::sweep_board_to_score(Player p) {
    for (int i=0; i<BOARD_SIZE; ++i) {
        scores[static_cast<int>(p)] += board[i].total();
        board[i].clear();
    }
}

void Rules2025Engine::check_game_end() {
    // 1. Majority Rule
    if (scores[0] >= 49 || scores[1] >= 49) {
        game_over_flag = true; 
        return;
    }
    
    // 2. Board Atrophy Rule (< 10 seeds)
    int total = 0;
    for(const auto& h : board) total += h.total();
    if (total < 10) {
        game_over_flag = true;
    }
}