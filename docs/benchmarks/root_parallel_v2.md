# Root-Parallel V2 Benchmark Snapshot

Date: 2026-03-29

This file records a local benchmark snapshot for the current V2 root-parallel search and compares it against:

- the current sequential path measured in the same session
- the saved V1 snapshot in `docs/benchmarks/root_parallel.md`
- the saved sequential snapshot in `docs/benchmarks/sequential.md`

## Environment

- Benchmark executable: `./build/src/engine/search_benchmark`
- Build directory: `build/`
- CMake build type: unset (`CMAKE_BUILD_TYPE` is empty in `build/CMakeCache.txt`)
- Compiler from cache: `/usr/bin/c++`

Important:

- Treat these as local comparison numbers, not universal performance numbers.
- The current V2 configuration uses:
  - `parallel_root = true`
  - `parallel_root_min_depth = 5`
  - `parallel_root_min_moves = 5`
  - `parallel_root_max_workers = auto`
- Most matrix cases below still fall back to the sequential path because they do not satisfy the V2 profitability gate.
- In the fixed depth-4/depth-5 matrix, the only case that clearly exercises the V2 parallel path is `midgame depth 5`.

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
./build/src/engine/search_benchmark --depth 6 --repeats 5 --scenario midgame
./build/src/engine/search_benchmark --depth 6 --repeats 5 --scenario midgame --parallel-root
```

## Current Session: Sequential vs V2

| Scenario | Depth | Best Move | Score | Seq Avg ms | V2 Avg ms | V2/Seq Time | Seq Avg Nodes | V2 Avg Nodes | Seq Avg TT Hits | V2 Avg TT Hits | Seq Avg Beta Cutoffs | V2 Avg Beta Cutoffs |
|---|---:|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| opening | 4 | D3 | -2 | 0.9056 | 0.9194 | 1.02x | 144 | 144 | 18 | 18 | 36 | 36 |
| opening | 5 | D3 | 3 | 1.7446 | 1.7044 | 0.98x | 322 | 322 | 51 | 51 | 73 | 73 |
| midgame | 4 | E1 | 2 | 10.8060 | 9.0722 | 0.84x | 725 | 725 | 52 | 52 | 184 | 184 |
| midgame | 5 | E1 | 9 | 29.1706 | 125.8990 | 4.32x | 3127 | 2968 | 235 | 86 | 493 | 499 |
| endgame | 4 | G8 | 24 | 0.4650 | 0.6896 | 1.48x | 43 | 43 | 13 | 13 | 11 | 11 |
| endgame | 5 | G8 | 24 | 0.5138 | 0.4366 | 0.85x | 62 | 62 | 27 | 27 | 18 | 18 |

Notes:

- Equal node / TT-hit / cutoff counts strongly suggest the V2 run fell back to the sequential path for that case.
- `midgame depth 5` is the important fixed-depth case here because it is the one that actually uses V2 root parallelism in this matrix.

## Heavier Case: Midgame Depth 6

| Scenario | Depth | Best Move | Score | Seq Avg ms | V2 Avg ms | V2/Seq Time | Seq Avg Nodes | V2 Avg Nodes | Seq Avg TT Hits | V2 Avg TT Hits | Seq Avg Beta Cutoffs | V2 Avg Beta Cutoffs |
|---|---:|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| midgame | 6 | E1 | 0 | 139.902 | 320.839 | 2.29x | 8619 | 8831 | 804 | 401 | 2269 | 2334 |

This extra case is not directly comparable to the saved V1 snapshot, but it is useful because it is a genuinely heavier search and still exercises the V2 parallel path.

## Historical Comparison: V1 vs V2

The table below compares the current V2 numbers against the saved V1 snapshot in `docs/benchmarks/root_parallel.md`.

| Scenario | Depth | V1 Avg ms | V2 Avg ms | V1/V2 Time | V1 Avg Nodes | V2 Avg Nodes |
|---|---:|---:|---:|---:|---:|---:|
| opening | 4 | 129.134 | 0.9194 | 140.5x | 202 | 144 |
| opening | 5 | 160.270 | 1.7044 | 94.0x | 541 | 322 |
| midgame | 4 | 397.356 | 9.0722 | 43.8x | 4017 | 725 |
| midgame | 5 | 518.077 | 125.8990 | 4.12x | 13107 | 2968 |
| endgame | 4 | 128.202 | 0.6896 | 185.9x | 52 | 43 |
| endgame | 5 | 160.504 | 0.4366 | 367.6x | 77 | 62 |

Important interpretation:

- Most of the huge V1-to-V2 improvement comes from the V2 profitability gate deciding not to parallelize cheap searches.
- The meaningful “real parallel” comparison in this matrix is still `midgame depth 5`.
- On that case, V2 is materially better than V1:
  - `125.899 ms` vs `518.077 ms`
  - about `4.12x` faster
  - `2968` nodes vs `13107`

## Comparison To Saved Sequential Snapshot

The saved sequential snapshot in `docs/benchmarks/sequential.md` is still directionally consistent with the current run:

- sequential remains the fastest choice in the meaningful heavy parallel cases
- `midgame depth 5` is still the most useful comparison point
- V2 has improved enormously over V1, but it still has not overtaken sequential where it truly parallelizes

At `midgame depth 5`:

- saved sequential snapshot: `21.2172 ms`
- current session sequential: `29.1706 ms`
- current session V2: `125.899 ms`

So even allowing for normal local timing drift, V2 is still well behind sequential on the key fixed-depth case.

## Quick Takeaways

- V2 is a major improvement over V1.
- The profitability gate is doing important work by avoiding obviously bad parallel cases.
- In the current depth-4/depth-5 matrix, only `midgame depth 5` clearly exercises the V2 parallel path.
- Where V2 really does parallelize, it is still slower than sequential:
  - `midgame depth 5`: about `4.32x` slower
  - `midgame depth 6`: about `2.29x` slower
- V2 is moving in the right direction, but it is not yet a performance win over sequential.
