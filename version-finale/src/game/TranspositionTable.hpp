#ifndef TRANSPOSITION_TABLE_HPP
#define TRANSPOSITION_TABLE_HPP

#include "Move.hpp"
#include <vector>
#include <cstdint>

enum class NodeType : uint8_t { EXACT, LOWER, UPPER };

struct TTEntry {
    uint64_t key;
    int value;
    int8_t depth;
    NodeType type;
    Move best_move;
};

class TranspositionTable {
public:
    TranspositionTable(size_t size_mb = 64) {
        resize_mb(size_mb);
    }
    
    void resize_mb(size_t size_mb) {
        size_t num_entries = (size_mb * 1024 * 1024) / sizeof(TTEntry);
        table.resize(num_entries);
        clear();
    }
    
    void clear() {
        for(auto& e : table) {
            e.key = 0;
            e.depth = -1;
        }
    }
    
    void store(uint64_t key, int val, int8_t depth, NodeType type, Move best_move) {
        size_t idx = key % table.size();
        
        // Deepest replacement implementation
        // Only overwrite if new depth is greater or equal
        // Or if the slot is empty (depth == -1)
        // Or simple collision (always replace)
        // Let's stick to simple always replace for this version
        table[idx] = {key, val, depth, type, best_move};
    }
    
    bool probe(uint64_t key, TTEntry& entry) {
        size_t idx = key % table.size();
        if (table[idx].key == key) {
            entry = table[idx];
            return true;
        }
        return false;
    }

private:
    std::vector<TTEntry> table;
};

#endif
