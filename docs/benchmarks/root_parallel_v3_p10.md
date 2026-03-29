# Root-Parallel V3 Phase 10 Benchmark Snapshot

Date: 2026-03-29

This file records a local benchmark snapshot for the current V3 root-parallel search after:

- Phase 8 coarser batching
- Phase 9 stronger sequential seeding
- Phase 10 root-level PVS / scout-window follow-ups

It compares the current code against:

- the current sequential path measured in the same session
- the saved V2 snapshot in `docs/benchmarks/root_parallel_v2.md`
- the saved V3 Phase 5 snapshot in `docs/benchmarks/root_parallel_v3_p5.md`
- the saved sequential snapshot in `docs/benchmarks/sequential.md`

## Environment

- Benchmark executable: `./build/src/engine/search_benchmark`
- Build directory: `build/`
- CMake build type: unset (`CMAKE_BUILD_TYPE` is empty in `build/CMakeCache.txt`)
- Compiler from cache: `/usr/bin/c++`

Important:

- Treat these as local comparison numbers, not universal performance numbers.
- The most trustworthy comparisons are same-session comparisons.
- This session was noticeably slower than older saved snapshots overall, so direct comparisons to `docs/benchmarks/sequential.md` should be read as directional, not absolute.
- Cheap cases still mostly fall back to the sequential path because of the profitability gate.
- The best current configs among the ones tested here were:
  - midgame depth 5: `parallel_root_max_workers = 4`, `parallel_root_batch_scale = 1`, `parallel_root_seed_moves = 2`, `parallel_root_use_pvs = true`
  - midgame depth 6: `parallel_root_max_workers = 4`, `parallel_root_batch_scale = 3`, `parallel_root_seed_moves = 3`, `parallel_root_use_pvs = true`

## Verification

```bash
cmake --build build
ctest --test-dir build --output-on-failure
```

Both passed before benchmarking.

## Commands Run

Heavy-case sequential baselines:

```bash
./build/src/engine/search_benchmark --depth 5 --repeats 10 --scenario midgame
./build/src/engine/search_benchmark --depth 6 --repeats 5 --scenario midgame
```

Midgame depth 5 sweep:

```bash
./build/src/engine/search_benchmark --depth 5 --repeats 10 --scenario midgame --parallel-root --parallel-root-max-workers 4 --parallel-root-batch-scale 1 --parallel-root-seed-moves 1
./build/src/engine/search_benchmark --depth 5 --repeats 10 --scenario midgame --parallel-root --parallel-root-max-workers 4 --parallel-root-batch-scale 1 --parallel-root-seed-moves 2
./build/src/engine/search_benchmark --depth 5 --repeats 10 --scenario midgame --parallel-root --parallel-root-max-workers 4 --parallel-root-batch-scale 1 --parallel-root-seed-moves 3
./build/src/engine/search_benchmark --depth 5 --repeats 10 --scenario midgame --parallel-root --parallel-root-max-workers 4 --parallel-root-batch-scale 1 --parallel-root-seed-moves 1 --parallel-root-pvs
./build/src/engine/search_benchmark --depth 5 --repeats 10 --scenario midgame --parallel-root --parallel-root-max-workers 4 --parallel-root-batch-scale 1 --parallel-root-seed-moves 2 --parallel-root-pvs
./build/src/engine/search_benchmark --depth 5 --repeats 10 --scenario midgame --parallel-root --parallel-root-max-workers 4 --parallel-root-batch-scale 1 --parallel-root-seed-moves 3 --parallel-root-pvs
```

Midgame depth 6 sweep:

```bash
./build/src/engine/search_benchmark --depth 6 --repeats 5 --scenario midgame --parallel-root --parallel-root-max-workers 4 --parallel-root-batch-scale 3 --parallel-root-seed-moves 1
./build/src/engine/search_benchmark --depth 6 --repeats 5 --scenario midgame --parallel-root --parallel-root-max-workers 4 --parallel-root-batch-scale 3 --parallel-root-seed-moves 2
./build/src/engine/search_benchmark --depth 6 --repeats 5 --scenario midgame --parallel-root --parallel-root-max-workers 4 --parallel-root-batch-scale 3 --parallel-root-seed-moves 3
./build/src/engine/search_benchmark --depth 6 --repeats 5 --scenario midgame --parallel-root --parallel-root-max-workers 4 --parallel-root-batch-scale 3 --parallel-root-seed-moves 1 --parallel-root-pvs
./build/src/engine/search_benchmark --depth 6 --repeats 5 --scenario midgame --parallel-root --parallel-root-max-workers 4 --parallel-root-batch-scale 3 --parallel-root-seed-moves 2 --parallel-root-pvs
./build/src/engine/search_benchmark --depth 6 --repeats 5 --scenario midgame --parallel-root --parallel-root-max-workers 4 --parallel-root-batch-scale 3 --parallel-root-seed-moves 3 --parallel-root-pvs
```

Fallback / cheap-case matrix with the tuned depth-5 config:

```bash
./build/src/engine/search_benchmark --depth 4 --repeats 5 --scenario opening
./build/src/engine/search_benchmark --depth 4 --repeats 5 --scenario opening --parallel-root --parallel-root-max-workers 4 --parallel-root-batch-scale 1 --parallel-root-seed-moves 2 --parallel-root-pvs
./build/src/engine/search_benchmark --depth 5 --repeats 5 --scenario opening
./build/src/engine/search_benchmark --depth 5 --repeats 5 --scenario opening --parallel-root --parallel-root-max-workers 4 --parallel-root-batch-scale 1 --parallel-root-seed-moves 2 --parallel-root-pvs
./build/src/engine/search_benchmark --depth 4 --repeats 5 --scenario midgame
./build/src/engine/search_benchmark --depth 4 --repeats 5 --scenario midgame --parallel-root --parallel-root-max-workers 4 --parallel-root-batch-scale 1 --parallel-root-seed-moves 2 --parallel-root-pvs
./build/src/engine/search_benchmark --depth 4 --repeats 5 --scenario endgame
./build/src/engine/search_benchmark --depth 4 --repeats 5 --scenario endgame --parallel-root --parallel-root-max-workers 4 --parallel-root-batch-scale 1 --parallel-root-seed-moves 2 --parallel-root-pvs
./build/src/engine/search_benchmark --depth 5 --repeats 5 --scenario endgame
./build/src/engine/search_benchmark --depth 5 --repeats 5 --scenario endgame --parallel-root --parallel-root-max-workers 4 --parallel-root-batch-scale 1 --parallel-root-seed-moves 2 --parallel-root-pvs
```

## Same-Session Heavy-Case Baselines

All heavy-case runs returned the same best move and score:

- depth 5: `E1`, score `9`
- depth 6: `E1`, score `0`

Sequential baselines from this session:

- midgame depth 5: `38.9796 ms`
- midgame depth 6: `166.309 ms`

## Midgame Depth 5 Sweep

All runs below use:

- `parallel_root = true`
- `parallel_root_max_workers = 4`
- `parallel_root_batch_scale = 1`

| Mode | Seed Moves | Avg ms | Current/Seq Time | Avg Nodes | Avg TT Hits | Avg Beta Cutoffs |
|---|---:|---:|---:|---:|---:|---:|
| no PVS | 1 | 48.4363 | 1.24x | 1922 | 94 | 407 |
| no PVS | 2 | 43.4451 | 1.11x | 1953 | 107 | 409 |
| no PVS | 3 | 43.0122 | 1.10x | 2066 | 122 | 412 |
| PVS | 1 | 50.3021 | 1.29x | 1853 | 94 | 404 |
| PVS | 2 | 37.4052 | 0.96x | 1918 | 107 | 408 |
| PVS | 3 | 55.3382 | 1.42x | 2051 | 123 | 416 |

Best tested depth-5 config:

- `parallel_root_max_workers = 4`
- `parallel_root_batch_scale = 1`
- `parallel_root_seed_moves = 2`
- `parallel_root_use_pvs = true`

At depth 5, the best PVS config was:

- about `1.16x` faster than the best tested non-PVS config (`37.4052 ms` vs `43.0122 ms`)
- about `1.04x` faster than same-session sequential (`37.4052 ms` vs `38.9796 ms`)

## Midgame Depth 6 Sweep

All runs below use:

- `parallel_root = true`
- `parallel_root_max_workers = 4`
- `parallel_root_batch_scale = 3`

| Mode | Seed Moves | Avg ms | Current/Seq Time | Avg Nodes | Avg TT Hits | Avg Beta Cutoffs |
|---|---:|---:|---:|---:|---:|---:|
| no PVS | 1 | 156.713 | 0.94x | 6379 | 469 | 1633 |
| no PVS | 2 | 142.695 | 0.86x | 6068 | 438 | 1528 |
| no PVS | 3 | 146.596 | 0.88x | 6545 | 505 | 1690 |
| PVS | 1 | 114.448 | 0.69x | 5877 | 418 | 1411 |
| PVS | 2 | 135.758 | 0.82x | 5998 | 444 | 1505 |
| PVS | 3 | 106.669 | 0.64x | 6365 | 505 | 1572 |

Best tested depth-6 config:

- `parallel_root_max_workers = 4`
- `parallel_root_batch_scale = 3`
- `parallel_root_seed_moves = 3`
- `parallel_root_use_pvs = true`

At depth 6, the best PVS config was:

- about `1.34x` faster than the best tested non-PVS config (`106.669 ms` vs `142.695 ms`)
- about `1.56x` faster than same-session sequential (`106.669 ms` vs `166.309 ms`)

## Current Session: Sequential vs Tuned Current V3

Notes:

- Cheap cases below use `5` repeats.
- The meaningful heavy cases use the more detailed sweep results above:
  - midgame depth 5 uses the `10`-repeat best config
  - midgame depth 6 uses the `5`-repeat best config
- Equal node / TT-hit / cutoff counts strongly suggest the tuned V3 path fell back to sequential for that case.

| Scenario | Depth | Best Move | Score | Seq Avg ms | Current V3 Avg ms | Current/Seq Time | Seq Avg Nodes | Current V3 Avg Nodes | Seq Avg TT Hits | Current V3 Avg TT Hits | Seq Avg Beta Cutoffs | Current V3 Avg Beta Cutoffs |
|---|---:|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| opening | 4 | D3 | -2 | 1.3262 | 1.1908 | 0.90x | 144 | 144 | 18 | 18 | 36 | 36 |
| opening | 5 | D3 | 3 | 7.3448 | 2.8590 | 0.39x | 322 | 322 | 51 | 51 | 73 | 73 |
| midgame | 4 | E1 | 2 | 19.8240 | 14.2388 | 0.72x | 725 | 725 | 52 | 52 | 184 | 184 |
| midgame | 5 | E1 | 9 | 38.9796 | 37.4052 | 0.96x | 3127 | 1918 | 235 | 107 | 493 | 408 |
| midgame | 6 | E1 | 0 | 166.309 | 106.669 | 0.64x | 8619 | 6365 | 804 | 505 | 2269 | 1572 |
| endgame | 4 | G8 | 24 | 0.3840 | 0.4890 | 1.27x | 43 | 43 | 13 | 13 | 11 | 11 |
| endgame | 5 | G8 | 24 | 0.7502 | 0.6212 | 0.83x | 62 | 62 | 27 | 27 | 18 | 18 |

Interpretation:

- The meaningful current wins are the heavy midgame cases, not the cheap fallback cases.
- At midgame depth 5 and 6, tuned current V3 now beats same-session sequential while also searching fewer nodes.
- The opening / depth-4 / endgame cases remain mostly sequential fallbacks, so their timing differences are mostly local noise rather than meaningful parallel wins.

## Historical Comparison On Heavy Cases

The table below compares the current best tuned V3 results against the saved V2 and saved V3 Phase 5 snapshots on the meaningful heavy cases.

| Depth | Saved V2 Avg ms | Saved V3 Phase 5 Avg ms | Current V3 Avg ms | Current/V2 Time | Current/V3 Phase 5 Time |
|---:|---:|---:|---:|---:|---:|
| 5 | 125.8990 | 249.519 | 37.4052 | 0.30x | 0.15x |
| 6 | 320.839 | 563.095 | 106.669 | 0.33x | 0.19x |

Equivalent speedup view:

- depth 5 current V3 is about `3.37x` faster than saved V2 and about `6.67x` faster than saved V3 Phase 5
- depth 6 current V3 is about `3.01x` faster than saved V2 and about `5.28x` faster than saved V3 Phase 5

## Comparison To Saved Sequential Snapshot

The saved sequential snapshot in `docs/benchmarks/sequential.md` still matters as a historical baseline, but the current session was clearly slower overall, so same-session comparisons are more trustworthy.

At midgame depth 5:

- saved sequential snapshot: `21.2172 ms`
- current session sequential: `38.9796 ms`
- current tuned V3: `37.4052 ms`

That means:

- current tuned V3 slightly beat same-session sequential
- but this session was slower than the older saved sequential snapshot overall
- so the right conclusion is that current V3 is now competitive locally, not that it universally beats every older sequential result

## Quick Takeaways

- This is the first saved V3 snapshot where the tuned root-parallel path beats same-session sequential on the meaningful heavy cases.
- Best tested configs in this run were:
  - depth 5: `workers=4`, `batch_scale=1`, `seed_moves=2`, `pvs=yes`
  - depth 6: `workers=4`, `batch_scale=3`, `seed_moves=3`, `pvs=yes`
- PVS helped once it was paired with the Phase 8 batching and the Phase 9 sequential seed:
  - depth 5: best PVS config beat best non-PVS config by about `1.16x`
  - depth 6: best PVS config beat best non-PVS config by about `1.34x`
- On the meaningful heavy cases, current tuned V3 is much better than the earlier saved parallel generations:
  - depth 5: about `3.37x` faster than saved V2, about `6.67x` faster than saved V3 Phase 5
  - depth 6: about `3.01x` faster than saved V2, about `5.28x` faster than saved V3 Phase 5
- Cheap cases still mostly fall back to sequential, so their wall-clock differences are not the main signal.
