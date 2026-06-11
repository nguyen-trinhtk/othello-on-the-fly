#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BENCH="${ROOT}/build/benchmarks/bench"
BENCH_MD="${ROOT}/docs/bench.md"

DEPTH="${1:-10}"
RUNS="${2:-20}"
EVAL="${3:-component}"
PARALLEL="${4:-thread-pool}"

if [[ ! -x "$BENCH" ]]; then
    echo "error: bench not found at ${BENCH} — run ./build.sh first" >&2
    exit 1
fi

output="$("$BENCH" "$DEPTH" "$RUNS" "$EVAL" "$PARALLEL")"

parse_field() {
    local key="$1"
    echo "$output" | sed -n "s/.*${key}=\([^ ]*\).*/\1/p"
}

eval="$(parse_field eval)"
eval_label="$(parse_field eval_label)"
parallel="$(parse_field parallel)"
depth="$(parse_field depth)"
runs="$(parse_field runs)"
total_ms="$(parse_field total_ms)"
avg_ms="$(parse_field avg_ms)"

if [[ -z "$eval_label" ]]; then
    eval_label="$eval"
fi
case "$parallel" in
    none) parallel_label="None" ;;
    thread-pool) parallel_label="Thread-pool" ;;
    *) parallel_label="$parallel" ;;
esac

if [[ -z "$depth" || -z "$runs" || -z "$total_ms" || -z "$avg_ms" ]]; then
    echo "error: failed to parse bench output: ${output}" >&2
    exit 1
fi

timestamp="$(date '+%Y/%m/%d %H:%M:%S')"
row="| ${timestamp} | ${eval_label} | ${parallel_label} | ${depth} | ${runs} | ${avg_ms} ms | ${total_ms} ms |"

TABLE_HEADER='| Date | Eval strategy | Parallel mode | Depth | Run | Avg. time | Total time |'
TABLE_SEP='|------|---------------|---------------|-------|-----|-----------|------------|'

if [[ ! -f "$BENCH_MD" ]]; then
    mkdir -p "$(dirname "$BENCH_MD")"
    cat >"$BENCH_MD" <<EOF
# Benchmark Table

${TABLE_HEADER}
${TABLE_SEP}
EOF
fi

if ! grep -qF "$TABLE_HEADER" "$BENCH_MD"; then
    {
        echo ""
        echo "$TABLE_HEADER"
        echo "$TABLE_SEP"
    } >>"$BENCH_MD"
fi

# Keep table rows contiguous (blank lines break markdown tables)
while [[ -f "$BENCH_MD" ]] && [[ "$(tail -n 1 "$BENCH_MD")" == "" ]]; do
    sed -i '' '$d' "$BENCH_MD"
done
printf '%s\n' "$row" >>"$BENCH_MD"
echo "Logged: ${row}"
