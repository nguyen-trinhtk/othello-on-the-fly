# Current Benchmark Snapshot

Date: 2026-03-24

This file records a local benchmark snapshot for the current engine.

## Environment

- Benchmark executable: `./build/src/engine/search_benchmark`
- Build directory: `build/`
- CMake build type: unset (`CMAKE_BUILD_TYPE` is empty in `build/CMakeCache.txt`)
- Compiler from cache: `/usr/bin/c++`

Important:

- Treat these as **local comparison numbers**, not universal performance numbers.
- Absolute timings depend on machine, load, compiler, and build flags.
- The time-limited search checks time periodically, so it may slightly overshoot the requested limit.

## Commands Run

```bash
./build/src/engine/search_benchmark --depth 4 --repeats 5 --scenario opening
./build/src/engine/search_benchmark --depth 5 --repeats 5 --scenario opening
./build/src/engine/search_benchmark --depth 4 --repeats 5 --scenario midgame
./build/src/engine/search_benchmark --depth 5 --repeats 5 --scenario midgame
./build/src/engine/search_benchmark --depth 4 --repeats 5 --scenario endgame
./build/src/engine/search_benchmark --depth 5 --repeats 5 --scenario endgame
./build/src/engine/search_benchmark --depth 8 --repeats 5 --scenario midgame --node-limit 1000
./build/src/engine/search_benchmark --depth 8 --repeats 5 --scenario midgame --time-ms 5
```

## Fixed-Depth Results

| Scenario | Depth | Repeats | Completed | Best Move | Score | Avg ms | Avg Nodes | Avg TT Hits | Avg Beta Cutoffs | Nodes/sec |
|---|---:|---:|---|---|---:|---:|---:|---:|---:|---:|
| opening | 4 | 5 | yes | D3 | -2 | 0.6972 | 144 | 18 | 36 | 206540 |
| opening | 5 | 5 | yes | D3 | 3 | 1.5820 | 322 | 51 | 73 | 203540 |
| midgame | 4 | 5 | yes | E1 | 2 | 6.9440 | 725 | 52 | 184 | 104407 |
| midgame | 5 | 5 | yes | E1 | 9 | 21.2172 | 3127 | 235 | 493 | 147380 |
| endgame | 4 | 5 | yes | G8 | 24 | 0.2298 | 43 | 13 | 11 | 187119 |
| endgame | 5 | 5 | yes | G8 | 24 | 1.8140 | 62 | 27 | 18 | 34178.6 |

## Budgeted Results

| Scenario | Requested Depth | Limit | Completed | Last Completed Depth | Best Move | Score | Avg ms | Avg Nodes | Avg TT Hits | Avg Beta Cutoffs | Nodes/sec |
|---|---:|---|---|---:|---|---:|---:|---:|---:|---:|---:|
| midgame | 8 | node-limit 1000 | no | 4 | E1 | 2 | 10.6294 | 1000 | 69 | 229 | 94078.7 |
| midgame | 8 | time-ms 5 | no | 4 | E1 | 2 | 11.1466 | 1024 | 69 | 235 | 91866.6 |

## Quick Takeaways

- Midgame is the most useful scenario for comparing search changes because it explores far more nodes than opening or endgame.
- Depth 5 midgame is a reasonable baseline for “normal” engine comparisons.
- The budgeted runs currently complete depth 4 but not depth 8 under the tested limits.
- The 5 ms run overshoots because time is checked periodically during search, not after every single node.
