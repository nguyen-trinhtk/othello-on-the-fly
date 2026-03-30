# Board / Move Optimization Benchmark

Date: 2026-03-29

This note records a clean post-optimization benchmark snapshot for the board/move pass:

- compact `flip_mask` move metadata
- mask-based valid-move hot path
- incremental board hashing

## Verification

```bash
cmake --build build
ctest --test-dir build --output-on-failure
```

Both passed before benchmarking.

## Commands Run

```bash
./build/src/engine/search_benchmark --depth 4 --repeats 5 --scenario opening
./build/src/engine/search_benchmark --depth 5 --repeats 5 --scenario opening
./build/src/engine/search_benchmark --depth 4 --repeats 5 --scenario midgame
./build/src/engine/search_benchmark --depth 5 --repeats 5 --scenario midgame
./build/src/engine/search_benchmark --depth 4 --repeats 5 --scenario endgame
./build/src/engine/search_benchmark --depth 5 --repeats 5 --scenario endgame
./build/src/engine/search_benchmark --depth 6 --repeats 5 --scenario midgame
./build/src/engine/search_benchmark --depth 8 --repeats 5 --scenario midgame --node-limit 1000
./build/src/engine/search_benchmark --depth 8 --repeats 5 --scenario midgame --time-ms 5
```

## Current Results

| Scenario | Depth | Repeats | Completed | Best Move | Score | Avg ms | Avg Nodes | Avg TT Hits | Avg Beta Cutoffs | Nodes/sec |
|---|---:|---:|---|---|---:|---:|---:|---:|---:|---:|
| opening | 4 | 5 | yes | D3 | -2 | 0.1946 | 144 | 18 | 36 | 739979 |
| opening | 5 | 5 | yes | D3 | 3 | 0.4002 | 322 | 51 | 73 | 804598 |
| midgame | 4 | 5 | yes | E1 | 2 | 2.3724 | 725 | 52 | 184 | 305598 |
| midgame | 5 | 5 | yes | E1 | 9 | 5.9550 | 3127 | 235 | 493 | 525105 |
| midgame | 6 | 5 | yes | E1 | 0 | 20.8952 | 8619 | 804 | 2269 | 412487 |
| endgame | 4 | 5 | yes | G8 | 24 | 0.0736 | 43 | 13 | 11 | 584239 |
| endgame | 5 | 5 | yes | G8 | 24 | 0.1088 | 62 | 27 | 18 | 569853 |

## Budgeted Results

| Scenario | Requested Depth | Limit | Completed | Last Completed Depth | Best Move | Score | Avg ms | Avg Nodes | Avg TT Hits | Avg Beta Cutoffs | Nodes/sec |
|---|---:|---|---|---:|---|---:|---:|---:|---:|---:|---:|
| midgame | 8 | node-limit 1000 | no | 4 | E1 | 2 | 2.1834 | 1000 | 69 | 229 | 458001 |
| midgame | 8 | time-ms 5 | no | 5 | E1 | 9 | 5.8790 | 3276 | 247 | 522 | 557374 |

## Comparison To Earlier Pre-Optimization Snapshot

Earlier project notes for the pre-optimization sequential engine recorded these fixed-depth timings:

| Scenario | Depth | Earlier Avg ms | Current Avg ms | Speedup |
|---|---:|---:|---:|---:|
| opening | 4 | 0.6972 | 0.1946 | 3.58x |
| opening | 5 | 1.5820 | 0.4002 | 3.95x |
| midgame | 4 | 6.9440 | 2.3724 | 2.93x |
| midgame | 5 | 21.2172 | 5.9550 | 3.56x |
| endgame | 4 | 0.2298 | 0.0736 | 3.12x |
| endgame | 5 | 1.8140 | 0.1088 | 16.67x |

Earlier project notes for the budgeted midgame cases recorded:

| Limit | Earlier Avg ms | Current Avg ms | Earlier Last Depth | Current Last Depth | Notes |
|---|---:|---:|---:|---:|---|
| node-limit 1000 | 10.6294 | 2.1834 | 4 | 4 | Same node budget, much faster wall time |
| time-ms 5 | 11.1466 | 5.8790 | 4 | 5 | Faster core now completes one more depth under the same time target |

## High-Level Takeaways

- The board/move pass is a real engine-speed win, not a micro-optimization.
- The fixed-depth sequential engine is materially faster across the full matrix.
- Midgame depth 5 improved from about `21.22 ms` to about `5.96 ms`, which is the clearest “normal search” signal.
- The 5 ms budgeted midgame run now reaches completed depth `5` instead of `4`, which is a meaningful functional improvement, not just a timing improvement.
- This pass strengthens the foundation for the next steps:
  - richer evaluation
  - aspiration windows / recursive PVS
  - Othello-specific selective pruning
