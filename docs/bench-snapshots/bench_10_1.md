# Benchmark Table

| Date | Eval strategy | Parallel mode | Depth | Run | Open | Mid | End |
|------|---------------|---------------|-------|-----|------|-----|-----|
| 2026/06/11 20:52:19 | count | None | 10 | 1 | 9.44075 ms | 1676.78 ms | 2.99546 ms |
| 2026/06/11 20:52:48 | component | None | 10 | 1 | 134.839 ms | 14796 ms | 4.72325 ms |
| 2026/06/11 20:53:01 | mask | None | 10 | 1 | 31.4594 ms | 6074.99 ms | 13.0567 ms |
| 2026/06/11 20:53:02 | count | Thread-pool | 10 | 1 | 5.65904 ms | 505.151 ms | 7.44867 ms |
| 2026/06/11 20:53:09 | component | Thread-pool | 10 | 1 | 79.8648 ms | 3574.41 ms | 6.55467 ms |
| 2026/06/11 20:53:13 | mask | Thread-pool | 10 | 1 | 11.0318 ms | 1656.25 ms | 14.9725 ms |
