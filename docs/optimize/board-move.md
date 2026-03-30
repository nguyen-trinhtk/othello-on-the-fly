# Board / Move Optimization Roadmap

Goal: finish the `Optimize board and move handling` item by reducing hot-path allocation, simplifying make/undo, removing avoidable move-generation overhead, and making board hashing cheap enough for search-heavy use.

## Phase 1: Flip Mask Migration

Step:
- Replace heap-backed `Move::flipped_discs` metadata with a compact `Move::flip_mask`.
- Finish the board-side `collect_flip_mask(...)` path and update move ordering/tests to use the new representation.

Code:
- `include/board/board.hpp`
- `src/board/move.cpp`
- `src/engine/alphabeta.cpp`
- `test/board/test_move.cpp`

Why:
- `std::vector<std::pair<int, int>>` is expensive in the search hot path.
- The board already ultimately wants a bitmask for make/undo, so storing the mask directly avoids extra conversion work and copies.

## Phase 2: Move-Mask Hot Path

Step:
- Add a direct valid-move bitmask generator.
- Switch `has_valid_moves`, `is_valid_move`, `compute_valid_moves`, and `get_moves_for_current_state` to use the move mask directly.
- Replace the remaining legacy valid-move helper with the mask-based API too.

Code:
- `include/board/board.hpp`
- `src/board/move.cpp`

Why:
- The current `unordered_set` route does extra container work just to answer simple hot-path questions.
- Search mostly needs fast existence/count/enumeration, not hash-table semantics.

## Phase 3: Incremental Hashing

Step:
- Cache the board hash in `Board`.
- Update it incrementally inside `set_square`, `set_current_player`, `process_move`, and `undo_move`.
- Stop rebuilding the Zobrist hash from all 64 squares on every `get_hash()` call.

Code:
- `include/board/board.hpp`
- `src/board/board.cpp`
- `src/board/hash.cpp`
- `src/board/move.cpp`

Why:
- Search queries `get_hash()` constantly.
- Recomputing the full board hash every time is unnecessary once board updates are already localized.

## Verification Plan

After each phase:
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`

Focus signals:
- `MoveLogic` tests stay green.
- `AlphaBetaTests` stay green because search correctness depends on make/undo and hashing.
- Any additional search-side suites should also stay green once enabled in this branch.

## Implementation Notes

Phase 1:
- Implemented.
- `Move` now stores `flip_mask` directly and exposes `flip_count()`.
- `process_move(...)`, `undo_move(...)`, `get_moves_for_current_state(...)`, and alpha-beta move ordering now use the compact flip metadata path.
- Result: move metadata is now stored in the same bitboard-friendly form that make/undo already wants.
- Verification:
  - `cmake --build build`
  - `ctest --test-dir build --output-on-failure`

Phase 2:
- Implemented.
- Added `generate_valid_move_mask(...)` and switched the search-critical board queries to use it directly:
  - `has_valid_moves(...)`
  - `is_valid_move(...)`
  - `compute_valid_moves(...)`
  - `get_moves_for_current_state(...)`
- Removed the remaining `unordered_set`-based valid-move API and updated the UI/tests to use `get_valid_move_mask()`.
- Result: the board layer is now consistently mask/vector-based instead of mixing fast internal paths with a legacy set representation.
- Verification:
  - `cmake --build build`
  - `ctest --test-dir build --output-on-failure`

Phase 3:
- Implemented.
- Added cached board hash state and incremental Zobrist maintenance.
- `set_square(...)`, `set_current_player(...)`, `process_move(...)`, and `undo_move(...)` now keep `hash_value` in sync as state changes.
- `get_hash()` is now a cheap field read instead of scanning the full board every time.
- Result: transposition-table-heavy search paths no longer pay a 64-square hash rebuild cost per lookup.
- Verification:
  - `cmake --build build`
  - `ctest --test-dir build --output-on-failure`

## High-Level Summary

- This optimization pass kept the external board API mostly stable while making the internal representation much cheaper for search.
- The cleanup follow-up finished the API simplification too, so valid-move queries are now consistently mask-based.
- The three changes reinforce each other:
  - compact move metadata avoids heap-heavy flip storage
  - direct move masks avoid avoidable container work
  - incremental hashing makes TT lookups much cheaper
- At this point, the `Optimize board and move handling` item can reasonably be treated as done at the board-layer level.
- The next logical follow-up is benchmarking this pass and then moving upward into evaluation and sequential search improvements.

## Benchmark Note

- Clean benchmark results for this pass are recorded in `docs/mynotes/optimize/board-move-benchmark.md`.
