# Heuristic Evaluation Implementation Plan

Goal: turn the feature formulas in [`heuristics.md`](/Users/nguyentrinh/Documents/GitHub/othello-on-the-fly/docs/heuristics/heuristics.md) into a clean engine implementation without mixing board-state code and evaluation policy too early.

## Design Direction

Recommended structure:

- keep [`Board`](/Users/nguyentrinh/Documents/GitHub/othello-on-the-fly/include/board/board.hpp) focused on board state and cheap board queries
- add a new engine evaluation module for heuristic scoring
- make alpha-beta call the new evaluator instead of relying on parity-only `Board::evaluate(...)`

Planned files:

- `include/engine/evaluation.hpp`
- `src/engine/evaluation.cpp`

Expected supporting edits:

- [`include/board/board.hpp`](/Users/nguyentrinh/Documents/GitHub/othello-on-the-fly/include/board/board.hpp)
- [`src/engine/alphabeta.cpp`](/Users/nguyentrinh/Documents/GitHub/othello-on-the-fly/src/engine/alphabeta.cpp)
- [`src/engine/CMakeLists.txt`](/Users/nguyentrinh/Documents/GitHub/othello-on-the-fly/src/engine/CMakeLists.txt)
- new tests under `test/engine/`

## Phase 1: Scaffolding And Safe Integration

Goal:

- create the evaluation module
- keep search behavior unchanged at first

Work:

- add `engine::evaluate_position(const Board &, Disc)` in the new module
- initially make it return the current parity-style score so behavior stays identical
- switch alpha-beta leaf evaluation to call `evaluate_position(...)`
- keep `Board::evaluate(...)` as a simple utility for now, not the engine's main evaluator

Why:

- this separates responsibilities first
- it gives a safe integration point before real heuristics arrive

Tests:

- add unit tests showing `evaluate_position(...)` matches the old parity result on representative boards
- run `ctest --test-dir build --output-on-failure`

## Phase 2: Phase Detection And Core Features

Goal:

- ship the first real heuristic version with the simplest strong features

Work:

- add game-phase detection from empty-square count
- add a shared `norm(a, b)` helper
- implement:
  - actual mobility
  - corners
  - parity
- define abstract early/mid/end weight tables in code using starter numeric placeholders

Why:

- mobility + corners + parity is already much more Othello-like than raw disc count
- this is the smallest meaningful upgrade that still stays easy to debug

Tests:

- unit tests for phase classification
- unit tests for normalization edge cases
- small board-position tests where:
  - corner ownership raises score
  - extra legal moves raises score
  - parity matters more in endgame than opening
- run `ctest --test-dir build --output-on-failure`

## Phase 3: Frontier And Potential Mobility

Goal:

- improve positional understanding without introducing the hardest feature yet

Work:

- implement frontier-disc counting
- implement potential mobility from empties adjacent to opponent discs
- add both terms to the weighted phase-aware evaluation

Why:

- these features improve midgame judgment a lot
- both are still cheap and bitboard-friendly

Tests:

- unit tests for frontier counting
- unit tests for potential mobility on controlled positions
- regression tests that search still completes and returns legal moves
- run `ctest --test-dir build --output-on-failure`

## Phase 4: Stability-ish

Goal:

- add a first corner-anchored stability approximation

Work:

- count occupied corners as stable
- count contiguous same-color edge discs extending from owned corners as stable-like
- avoid exact full-board stability for this phase

Why:

- stability matters, but exact stability is easy to overcomplicate
- this approximation captures the most important idea first

Tests:

- unit tests for corner stability
- unit tests for edge runs extending from corners
- unit tests proving non-corner-anchored edge discs are not over-counted
- run `ctest --test-dir build --output-on-failure`

## Phase 5: Weighting Cleanup And Evaluator Shape

Goal:

- make the evaluator readable, tunable, and easy to inspect

Work:

- introduce structs such as:
  - `FeatureVector`
  - `PhaseWeights`
- keep feature extraction separate from weighted summation
- store starter numeric weights in one place in `evaluation.cpp`
- optionally expose a debug-only feature breakdown helper later

Why:

- this makes later tuning much easier
- it prevents the evaluator from turning into one giant opaque function

Tests:

- unit tests for feature-vector values on sample boards
- unit tests for phase-weight lookup
- run `ctest --test-dir build --output-on-failure`

## Phase 6: Benchmark And Tune

Goal:

- turn the starter evaluation into a measured improvement instead of just a nicer formula

Work:

- benchmark fixed-depth search before and after each major eval phase
- compare node counts and elapsed time
- play self-play or controlled matchups against the old parity evaluator
- convert abstract priorities into better numeric weights

Why:

- stronger evaluation should improve both playing quality and move ordering
- benchmarking keeps the project resume-friendly and evidence-based

Tests:

- full `ctest --test-dir build --output-on-failure`
- save benchmark snapshots under `docs/benchmarks/` or `docs/heuristics/`

## First Recommended Cut

If the goal is to ship value quickly, stop after:

1. Phase 1
2. Phase 2
3. Phase 3

That gives:

- clean evaluator architecture
- phase-aware scoring
- mobility/corners/parity/frontier/potential mobility

That is already a credible first heuristic engine pass. Stability-ish can then land as the next focused improvement instead of blocking the first rollout.

## Non-Goals For The First Pass

Do not include these yet:

- exact full-board stability
- pattern tables
- `X`/`C` square corner-danger terms
- learned weights
- neural-network evaluation

These all make sense later, but they should not complicate the first working heuristic module.

## High-Level Implementation Notes

- The evaluator should live in `engine`, not `board`.
- `Board` should only expose facts, not strategic judgment.
- Keep the old parity logic available temporarily so tests can compare old vs new behavior.
- Add features in layers so regressions are attributable.
- Re-run tests after every major feature phase, not just at the end.
