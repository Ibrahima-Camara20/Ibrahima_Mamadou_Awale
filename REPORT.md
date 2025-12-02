# Awale AI Project Report

## 1. Introduction

The goal of this project was to develop a competitive Artificial Intelligence agent for a variant of the Awale game. The project involved implementing a robust game engine compliant with specific rules, designing an AI architecture, and optimizing the agent for performance and strategic depth.

## 2. Game Engine Implementation

### 2.1 Rule Verification
The game rules differ slightly from standard Oware. Key implementation details include:
- **Interleaved Ownership**: Player 1 controls odd holes (1, 3...), Player 2 controls even holes (2, 4...).
- **Color Logic**: Moves are based on seed colors (Red, Blue, Transparent).
- **Captures**: Captures occur when a hole reaches 2 or 3 seeds. Multi-step captures (backtracking) are implemented.
- **Starvation**: If a player cannot move, the opponent captures all remaining seeds on their side.
- **Draw Condition**: The game ends in a draw if both players capture 40 or more seeds, or if the board has fewer than 10 seeds.

### 2.2 Testing
A rigorous test suite (`tests/Test.cpp`) was developed to verify the engine:
- **Case 1 & 2**: Specific scenarios from the rulebook were implemented as unit tests to ensure capture and sowing logic correctness.
- **Starvation Test**: Verified that the engine correctly handles the end-game starvation rule.
- **Bot vs Bot**: Automated matches between Random, Greedy, and AlphaBeta bots to ensure stability.

## 3. AI Architecture

### 3.1 Bot Interface
An abstract `Bot` class was created to allow easy swapping of strategies. This facilitated the development of baseline agents (`RandomBot`, `GreedyBot`) for benchmarking.

### 3.2 Alpha-Beta Pruning
The core of the competitive agent is the `AlphaBetaBot`. It uses the Minimax algorithm with Alpha-Beta pruning to efficiently search the game tree.
- **Minimax**: Minimizes the maximum possible loss.
- **Alpha-Beta Pruning**: Eliminates branches that cannot influence the final decision, significantly increasing the search depth.

### 3.3 Iterative Deepening
To strictly adhere to time limits (e.g., 1 second per move), Iterative Deepening was implemented. The bot performs a depth-first search at depth 1, then depth 2, and so on. If the time limit is reached, it returns the best move found in the last fully completed depth. This ensures the bot always has a valid move ready.

### 3.4 Heuristic Evaluation
Since the game tree is too large to solve completely, a heuristic function evaluates non-terminal states. The evaluation function considers:
1.  **Score Difference**: The primary metric. $(MyCaptures - OpponentCaptures)$.
2.  **Material Advantage**: A secondary weight is given to the number of seeds on the player's side of the board. This acts as a proxy for mobility and defense capability.

## 4. Results

The `AlphaBetaBot` was tested against the `GreedyBot` (which always picks the move that maximizes immediate capture).
- **Performance**: `AlphaBetaBot` consistently defeats `GreedyBot`.
- **Depth**: On average, the bot reaches a depth of 6-8 ply within a 0.1s time limit, and deeper with more time.

## 5. Conclusion

The developed AI demonstrates strong tactical play through lookahead and pruning. The robust game engine ensures valid play under all rule variations. Future improvements could include a Transposition Table to cache board states and further optimize the search.
