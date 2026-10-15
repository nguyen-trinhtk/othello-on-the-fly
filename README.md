# Othello on the Fly

This is a C++ Othello (aka Reversi) engine aimed at strong play and fast search. It uses **negamax** with **alpha–beta pruning**, a **transposition table** (Zobrist hashing), **thread-pool** root parallelism, and an **n-tuple neural network** evaluator.

Try the game in your browser: **[Othello on the Fly](https://nguyen-trinhtk.github.io/othello-on-the-fly)**

<p align="center">
  <img src="docs/web-gui.gif" alt="Playing a few moves against the AI in the web version" width="480">
</p>

## Implementation

**Search**
- **Negamax**: finds a move recursivelee assuming both sides play optimally
- **Alpha–beta pruning**: skips irrelevant branches in search tree that cannot beat the best move found so far
- **Transposition table + Zobrist hashing**: caches scores for positions already seen
- **Thread pool**: searches root moves in parallel (got about **5–6×** faster on 8 cores)

**Evaluation**

A good eval lets the engine stay strong without searching as deep:

- **N-tuple neural network**: lightweight model learning board state via cell pairs. Trained in **PyTorch**, then loaded by the C++ engine for fast scoring.

## Repo structure

```
app/           main app
web/           web version of the game (HTML, CSS, JavaScript)
wasm/          builds the engine for the web (./wasm/build.sh, needs Emscripten)
src/ include/  search, eval, board rules (C++)
python/        n-tuple training (PyTorch)
benchmarks/    speed and strength tests
docs/          notes and stats
```
