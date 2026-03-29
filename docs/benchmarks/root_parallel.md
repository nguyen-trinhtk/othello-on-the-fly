# Root-Parallel Benchmark Snapshot

Date: 2026-03-29

This file records a local benchmark snapshot comparing the current sequential search against the current root-parallel search.

## Environment

- Benchmark executable: `./build/src/engine/search_benchmark`
- Build directory: `build/`
- CMake build type: unset (`CMAKE_BUILD_TYPE` is empty in `build/CMakeCache.txt`)
- Compiler from cache: `/usr/bin/c++`

Important:

- Treat these as local comparison numbers, not universal performance numbers.
- Absolute timings depend on machine, load, compiler, and build flags.
- The root-parallel implementation here is the current v1 thread-per-root-move design.
- Budgeted searches are intentionally omitted from this comparison because `parallel_root` falls back to sequential mode when time or node limits are set.

## Commands Run

```bash
./build/src/engine/search_benchmark --depth 4 --repeats 5 --scenario opening
./build/src/engine/search_benchmark --depth 4 --repeats 5 --scenario opening --parallel-root
./build/src/engine/search_benchmark --depth 4 --repeats 5 --scenario midgame
./build/src/engine/search_benchmark --depth 4 --repeats 5 --scenario midgame --parallel-root
./build/src/engine/search_benchmark --depth 4 --repeats 5 --scenario endgame
./build/src/engine/search_benchmark --depth 4 --repeats 5 --scenario endgame --parallel-root
./build/src/engine/search_benchmark --depth 5 --repeats 5 --scenario opening
./build/src/engine/search_benchmark --depth 5 --repeats 5 --scenario opening --parallel-root
./build/src/engine/search_benchmark --depth 5 --repeats 5 --scenario midgame
./build/src/engine/search_benchmark --depth 5 --repeats 5 --scenario midgame --parallel-root
./build/src/engine/search_benchmark --depth 5 --repeats 5 --scenario endgame
./build/src/engine/search_benchmark --depth 5 --repeats 5 --scenario endgame --parallel-root
```

## Fixed-Depth Comparison

| Scenario | Depth | Best Move | Score | Seq Avg ms | Par Avg ms | Par/Seq Time | Seq Avg Nodes | Par Avg Nodes | Seq Nodes/sec | Par Nodes/sec |
|---|---:|---|---:|---:|---:|---:|---:|---:|---:|---:|
| opening | 4 | D3 | -2 | 0.8934 | 129.134 | 144.5x | 144 | 202 | 161182 | 1564.27 |
| midgame | 4 | E1 | 2 | 8.1798 | 397.356 | 48.6x | 725 | 4017 | 88633 | 10109.3 |
| endgame | 4 | G8 | 24 | 0.2400 | 128.202 | 534.2x | 43 | 52 | 179167 | 405.611 |
| opening | 5 | D3 | 3 | 2.0360 | 160.270 | 78.7x | 322 | 541 | 158153 | 3375.55 |
| midgame | 5 | E1 | 9 | 29.4640 | 518.077 | 17.6x | 3127 | 13107 | 106130 | 25299.3 |
| endgame | 5 | G8 | 24 | 0.3330 | 160.504 | 482.0x | 62 | 77 | 186186 | 479.738 |

## Search-Profile Comparison

| Scenario | Depth | Seq Avg TT Hits | Par Avg TT Hits | Seq Avg Beta Cutoffs | Par Avg Beta Cutoffs |
|---|---:|---:|---:|---:|---:|
| opening | 4 | 18 | 3 | 36 | 38 |
| midgame | 4 | 52 | 3 | 184 | 532 |
| endgame | 4 | 13 | 3 | 11 | 0 |
| opening | 5 | 51 | 4 | 73 | 112 |
| midgame | 5 | 235 | 104 | 493 | 2944 |
| endgame | 5 | 27 | 4 | 18 | 0 |

## Quick Takeaways

- All tested fixed-depth runs returned the same best move and score in sequential and parallel modes.
- On this machine and build, the current root-parallel v1 implementation is slower in every tested case.
- The slowdown is smallest in the most expensive case tested, midgame depth 5, but it is still substantial at about 17.6x slower.
- Opening and endgame positions are especially poor fits for this implementation because the trees are too small to amortize thread-launch overhead.
- The parallel version also explores more nodes in every tested case, which is consistent with losing the sequential root loop's shared alpha progression and shared transposition-table reuse.
