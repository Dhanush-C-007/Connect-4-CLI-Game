# Connect-4-CLI

Simple Connect Four command-line game in C++.

Build (out-of-source):

```powershell
mkdir build
cd build
cmake ..
cmake --build . --config Release

```

Run:

- Choose Human vs Human or Human vs AI when prompted.

Files:
- `apps/main.cpp` - entry point
- `include/` - headers for `Board`, `GameEngine`, `Player`, `HumanPlayer`, `AIPlayer`
- `src/` - implementations

Notes:
- AI uses a simple Minimax with alpha-beta pruning and a heuristic evaluation.
