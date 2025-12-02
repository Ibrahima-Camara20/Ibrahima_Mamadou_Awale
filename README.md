# Awale AI Project

This project implements a competitive AI for a specific variant of the Awale game (Oware).

## Project Structure

- `src/`: Source code
  - `game/`: Game logic and Bot implementations
    - `Game.cpp/hpp`: Core game engine
    - `Bot.hpp`: Bot interface
    - `AlphaBetaBot.cpp/hpp`: Advanced AI using Minimax/Alpha-Beta
  - `io/`: Input/Output parsing
  - `tests/`: Unit tests and verification
  - `main.cpp`: Main entry point (Human vs AI)

## Features

- **Robust Game Engine**: Strictly follows the provided rules, including complex capture logic (starvation, multiple captures) and draw conditions.
- **Competitive AI**:
  - **Alpha-Beta Pruning**: Efficiently searches the game tree.
  - **Iterative Deepening**: Ensures the bot always returns a move within the time limit.
  - **Custom Heuristic**: Evaluates positions based on score difference and strategic seed distribution.
- **Playable Interface**: Console-based interface to play against the AI.

## How to Build and Run

### Prerequisites

- A C++17 compliant compiler (e.g., `g++`).
- Make (optional, but recommended).

### Compilation

Navigate to the `src` directory:

```bash
cd src
```

Compile the game:

```bash
g++ -std=c++17 -I. main.cpp game/Game.cpp game/Bot.cpp game/AlphaBetaBot.cpp io/Parser.cpp -o awale
```

### Running the Game

```bash
./awale
```

Follow the on-screen instructions. You can choose to be Player 1 or Player 2.

### Running Tests

To verify the game rules and AI performance:

```bash
g++ -std=c++17 -I. tests/Test.cpp game/Game.cpp game/Bot.cpp game/AlphaBetaBot.cpp io/Parser.cpp -o test_runner
./test_runner
```

## AI Strategy

The `AlphaBetaBot` uses a Minimax algorithm with Alpha-Beta pruning to look ahead in the game.

- **Depth**: It searches as deep as possible within the time limit (Iterative Deepening).
- **Evaluation**: It prioritizes:
  1.  Score difference (Captures).
  2.  Seed advantage (having more seeds on its side implies better defense and attack potential).

## Authors

- [Your Name/Team Name]
