Done:
- Transposition table
- TT move ordering
- Console UI cleanup
- Iterative deepening with shared TT reuse
- Configurable search budget API: depth, time, or node limit
- Better move ordering: corners, edges, killer/history heuristics
- Performance benchmarking
- Reduce board copies in search with make/unmake move
- Root-parallel search experiments and tuning: V1 -> V2 -> V3
- Parallel benchmark snapshots and tuning notes

Next:
- Optimize board and move handling
- Replace heap-heavy move state with a compact flip representation
- Remove avoidable move-generation overhead in the hot path

After that:
- Add a real heuristic evaluation: mobility, corners, frontier, parity, stability-ish terms
- Improve sequential search first: aspiration windows and recursive PVS
- Explore Othello-specific selective pruning, especially ProbCut / Multi-ProbCut

Later:
- Revisit parallel search after the sequential core is stronger
- Benchmark and document each optimization pass cleanly
- Better UI polish
- Add an opening book or exact endgame solver

Stretch:
- Neural-network evaluation as an experimental extension, not the main path
- Additional pruning experiments only after the stronger core search is in place
