# Othello on the Fly

This is a C++ Othello (aka Reversi) engine aimed at strong play and fast search. It uses **negamax** with **alpha–beta pruning**, a **transposition table** (Zobrist hashing), **thread-pool** root parallelism, and an **n-tuple neural network** evaluator.

This is how a gameplay can look like (timelapsed). 

<img width="300" height="300" alt="othello" src="https://github.com/user-attachments/assets/53ad9bdd-b556-4aa5-a531-695a5b6066d6" />

(Thanks Claude for helping create the simulator.)

For more about the game, please see this [rulebook by World Othello Foundation](https://www.worldothello.org/about/about-othello/othello-rules/official-rules/english).

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
src/ include/  search, eval, board rules (C++)
python/        n-tuple training (PyTorch)
benchmarks/    speed and strength tests
docs/          notes and stats
```
