# Root-Parallel V3 Benchmark Snapshot

Date: 2026-03-29

This file records a local benchmark snapshot for the current V3 root-parallel search after the Phase 5 batch-based alpha refresh work and compares it against:

- the current sequential path measured in the same session
- the saved V2 snapshot in `docs/benchmarks/root_parallel_v2.md`
- the saved sequential snapshot in `docs/benchmarks/sequential.md`

## Environment

- Benchmark executable: `./build/src/engine/search_benchmark`
- Build directory: `build/`
- CMake build type: unset (`CMAKE_BUILD_TYPE` is empty in `build/CMakeCache.txt`)
- Compiler from cache: `/usr/bin/c++`

Important:

- Treat these as local comparison numbers, not universal performance numbers.
- The current V3 matrix below uses:
  - `parallel_root = true`
  - `parallel_root_min_depth = 5`
  - `parallel_root_min_moves = 5`
  - `parallel_root_max_workers = 4`
- `4` workers was chosen from the same-session V3 sweep below because it was the least-bad current setting on the heavy midgame cases.
- As in V2, many cheap cases still fall back to the sequential path because they do not satisfy the profitability gate.

## Commands Run

```bash
./build/src/engine/search_benchmark --depth 5 --repeats 10 --scenario midgame
./build/src/engine/search_benchmark --depth 5 --repeats 10 --scenario midgame --parallel-root --parallel-root-max-workers 0
./build/src/engine/search_benchmark --depth 5 --repeats 10 --scenario midgame --parallel-root --parallel-root-max-workers 1
./build/src/engine/search_benchmark --depth 5 --repeats 10 --scenario midgame --parallel-root --parallel-root-max-workers 2
./build/src/engine/search_benchmark --depth 5 --repeats 10 --scenario midgame --parallel-root --parallel-root-max-workers 3
./build/src/engine/search_benchmark --depth 5 --repeats 10 --scenario midgame --parallel-root --parallel-root-max-workers 4
./build/src/engine/search_benchmark --depth 6 --repeats 5 --scenario midgame
./build/src/engine/search_benchmark --depth 6 --repeats 5 --scenario midgame --parallel-root --parallel-root-max-workers 0
./build/src/engine/search_benchmark --depth 6 --repeats 5 --scenario midgame --parallel-root --parallel-root-max-workers 1
./build/src/engine/search_benchmark --depth 6 --repeats 5 --scenario midgame --parallel-root --parallel-root-max-workers 2
./build/src/engine/search_benchmark --depth 6 --repeats 5 --scenario midgame --parallel-root --parallel-root-max-workers 3
./build/src/engine/search_benchmark --depth 6 --repeats 5 --scenario midgame --parallel-root --parallel-root-max-workers 4

./build/src/engine/search_benchmark --depth 4 --repeats 5 --scenario opening
./build/src/engine/search_benchmark --depth 4 --repeats 5 --scenario opening --parallel-root --parallel-root-max-workers 4
./build/src/engine/search_benchmark --depth 5 --repeats 5 --scenario opening
./build/src/engine/search_benchmark --depth 5 --repeats 5 --scenario opening --parallel-root --parallel-root-max-workers 4
./build/src/engine/search_benchmark --depth 4 --repeats 5 --scenario midgame
./build/src/engine/search_benchmark --depth 4 --repeats 5 --scenario midgame --parallel-root --parallel-root-max-workers 4
./build/src/engine/search_benchmark --depth 5 --repeats 5 --scenario midgame
./build/src/engine/search_benchmark --depth 5 --repeats 5 --scenario midgame --parallel-root --parallel-root-max-workers 4
./build/src/engine/search_benchmark --depth 4 --repeats 5 --scenario endgame
./build/src/engine/search_benchmark --depth 4 --repeats 5 --scenario endgame --parallel-root --parallel-root-max-workers 4
./build/src/engine/search_benchmark --depth 5 --repeats 5 --scenario endgame
./build/src/engine/search_benchmark --depth 5 --repeats 5 --scenario endgame --parallel-root --parallel-root-max-workers 4
./build/src/engine/search_benchmark --depth 6 --repeats 5 --scenario midgame
./build/src/engine/search_benchmark --depth 6 --repeats 5 --scenario midgame --parallel-root --parallel-root-max-workers 4
```

## Same-Session Worker-Cap Sweep

Heavy-case sequential baselines from the same session:

- midgame depth 5: `20.4497 ms`
- midgame depth 6: `92.7286 ms`

All sweep runs returned the same best move and score:

- depth 5: `E1`, score `9`
- depth 6: `E1`, score `0`

| Workers | Depth 5 Avg ms | V3/Seq Time | Depth 6 Avg ms | V3/Seq Time |
|---:|---:|---:|---:|---:|
| auto | 355.322 | 17.38x | 833.588 | 8.99x |
| 1 | 436.626 | 21.35x | 928.915 | 10.02x |
| 2 | 399.450 | 19.53x | 802.798 | 8.66x |
| 3 | 404.762 | 19.79x | 587.114 | 6.33x |
| 4 | 256.578 | 12.55x | 560.001 | 6.04x |

Takeaway:

- Phase 5 changed the scaling pattern enough that wider parallelism helps again.
- `4` workers is the best current setting among the tested caps.
- Even the best current cap is still far behind sequential on both heavy cases.

## Current Session: Sequential vs Tuned V3

The table below compares the current sequential path against the current V3 path with `parallel_root_max_workers = 4`.

| Scenario | Depth | Best Move | Score | Seq Avg ms | V3 Avg ms | V3/Seq Time | Seq Avg Nodes | V3 Avg Nodes | Seq Avg TT Hits | V3 Avg TT Hits | Seq Avg Beta Cutoffs | V3 Avg Beta Cutoffs |
|---|---:|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| opening | 4 | D3 | -2 | 0.6762 | 0.6818 | 1.01x | 144 | 144 | 18 | 18 | 36 | 36 |
| opening | 5 | D3 | 3 | 1.4800 | 1.5050 | 1.02x | 322 | 322 | 51 | 51 | 73 | 73 |
| midgame | 4 | E1 | 2 | 6.7306 | 6.6250 | 0.98x | 725 | 725 | 52 | 52 | 184 | 184 |
| midgame | 5 | E1 | 9 | 19.9102 | 249.519 | 12.53x | 3127 | 2968 | 235 | 86 | 493 | 499 |
| endgame | 4 | G8 | 24 | 0.2514 | 0.2440 | 0.97x | 43 | 43 | 13 | 13 | 11 | 11 |
| endgame | 5 | G8 | 24 | 0.3282 | 0.3154 | 0.96x | 62 | 62 | 27 | 27 | 18 | 18 |

Notes:

- Equal nodes / TT-hit / cutoff counts strongly suggest the V3 run fell back to the sequential path for that case.
- `midgame depth 5` is the main fixed-depth case that clearly exercises the current V3 parallel path.

## Heavier Case: Midgame Depth 6

| Scenario | Depth | Best Move | Score | Seq Avg ms | V3 Avg ms | V3/Seq Time | Seq Avg Nodes | V3 Avg Nodes | Seq Avg TT Hits | V3 Avg TT Hits | Seq Avg Beta Cutoffs | V3 Avg Beta Cutoffs |
|---|---:|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| midgame | 6 | E1 | 0 | 92.7286 | 563.095 | 6.07x | 8619 | 8831 | 804 | 401 | 2269 | 2334 |

## Historical Comparison: Saved V2 vs Current V3

The table below compares the current tuned V3 numbers against the saved V2 snapshot in `docs/benchmarks/root_parallel_v2.md`.

| Scenario | Depth | V2 Avg ms | V3 Avg ms | V3/V2 Time | V2 Avg Nodes | V3 Avg Nodes |
|---|---:|---:|---:|---:|---:|---:|
| opening | 4 | 0.9194 | 0.6818 | 0.74x | 144 | 144 |
| opening | 5 | 1.7044 | 1.5050 | 0.88x | 322 | 322 |
| midgame | 4 | 9.0722 | 6.6250 | 0.73x | 725 | 725 |
| midgame | 5 | 125.8990 | 249.519 | 1.98x | 2968 | 2968 |
| endgame | 4 | 0.6896 | 0.2440 | 0.35x | 43 | 43 |
| endgame | 5 | 0.4366 | 0.3154 | 0.72x | 62 | 62 |
| midgame | 6 | 320.839 | 563.095 | 1.75x | 8831 | 8831 |

Important interpretation:

- The apparent improvements on opening, endgame, and `midgame depth 4` are not meaningful parallel wins; those cases still fall back to sequential.
- On the important heavy parallel cases, current V3 regressed relative to saved V2:
  - `midgame depth 5`: `249.519 ms` vs `125.899 ms` (`1.98x` slower)
  - `midgame depth 6`: `563.095 ms` vs `320.839 ms` (`1.75x` slower)

## Comparison To Saved Sequential Snapshot

The saved sequential snapshot in `docs/benchmarks/sequential.md` still points in the same direction:

- sequential remains the fastest option on the meaningful heavy cases
- the current same-session sequential numbers are slightly different, but the conclusion is the same

At `midgame depth 5`:

- saved sequential snapshot: `21.2172 ms`
- current session sequential: `19.9102 ms`
- current tuned V3: `249.519 ms`

At `midgame depth 6`:

- current session sequential: `92.7286 ms`
- current tuned V3: `563.095 ms`

## Quick Takeaways

- Phase 5 changed the worker-cap story: `4` workers is now the best of the tested settings.
- That improvement in scaling did not translate into competitiveness with sequential.
- On the meaningful heavy cases, tuned V3 is still much slower than sequential:
  - `midgame depth 5`: about `12.53x` slower
  - `midgame depth 6`: about `6.07x` slower
- Current Phase 5 V3 is also slower than the saved V2 snapshot on those same heavy cases.
- Right now, Phase 5 should be treated as a correctness experiment with a performance regression, not a win.
