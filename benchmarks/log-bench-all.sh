#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
BENCH_MD="${ROOT}/docs/bench.md"
DEPTH="${1:-10}"
RUNS="${2:-20}"

TABLE_HEADER='| Date | Eval strategy | Parallel mode | Depth | Run | Avg. time | Total time |'
TABLE_SEP='|------|---------------|---------------|-------|-----|-----------|------------|'

cat >"$BENCH_MD" <<EOF
# Benchmark Table

${TABLE_HEADER}
${TABLE_SEP}
EOF

EVALS=(count component mask)
PARALLELS=(none thread-pool)

for parallel in "${PARALLELS[@]}"; do
    for eval in "${EVALS[@]}"; do
        bash "${SCRIPT_DIR}/log-bench.sh" "$DEPTH" "$RUNS" "$eval" "$parallel"
    done
done
