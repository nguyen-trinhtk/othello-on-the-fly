#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/build-coverage}"
PROFILE_DIR="$BUILD_DIR/profiles"
REPORT_DIR="${REPORT_DIR:-$ROOT_DIR/coverage-report}"
PROFDATA_FILE="$BUILD_DIR/coverage.profdata"
JOBS="${JOBS:-4}"
LLVM_COV_BIN="${LLVM_COV_BIN:-$(xcrun --find llvm-cov)}"
LLVM_PROFDATA_BIN="${LLVM_PROFDATA_BIN:-$(xcrun --find llvm-profdata)}"
GOOGLETEST_SOURCE_DIR="${GOOGLETEST_SOURCE_DIR:-$ROOT_DIR/build/_deps/googletest-src}"

cmake_args=(
    -S "$ROOT_DIR"
    -B "$BUILD_DIR"
    -DENABLE_COVERAGE=ON
)

if [ -d "$GOOGLETEST_SOURCE_DIR" ]; then
    cmake_args+=("-DFETCHCONTENT_SOURCE_DIR_GOOGLETEST=$GOOGLETEST_SOURCE_DIR")
fi

cmake "${cmake_args[@]}"
cmake --build "$BUILD_DIR" -j"$JOBS"

rm -rf "$PROFILE_DIR" "$REPORT_DIR"
mkdir -p "$PROFILE_DIR" "$REPORT_DIR"

LLVM_PROFILE_FILE="$PROFILE_DIR/%p-%m.profraw" ctest --test-dir "$BUILD_DIR" --output-on-failure

profiles=()
while IFS= read -r profile; do
    profiles+=("$profile")
done < <(find "$PROFILE_DIR" -name '*.profraw' -print)

if [ "${#profiles[@]}" -eq 0 ]; then
    echo "No coverage profiles were produced." >&2
    exit 1
fi

"$LLVM_PROFDATA_BIN" merge -sparse "${profiles[@]}" -o "$PROFDATA_FILE"

objects=(
    "$BUILD_DIR/test/board/test_board"
    "$BUILD_DIR/test/board/test_move"
    "$BUILD_DIR/test/engine/test_alphabeta"
    "$BUILD_DIR/test/game/test_console_ui"
)

existing_objects=()
for object in "${objects[@]}"; do
    if [ -x "$object" ]; then
        existing_objects+=("$object")
    fi
done

if [ "${#existing_objects[@]}" -eq 0 ]; then
    echo "No instrumented test binaries were found in $BUILD_DIR." >&2
    exit 1
fi

primary_object="${existing_objects[0]}"
llvm_cov_objects=()
for ((i = 1; i < ${#existing_objects[@]}; ++i)); do
    llvm_cov_objects+=("-object=${existing_objects[i]}")
done

"$LLVM_COV_BIN" report \
    "$primary_object" \
    "${llvm_cov_objects[@]}" \
    -instr-profile="$PROFDATA_FILE" \
    -ignore-filename-regex='.*/(test|_deps)/.*'

"$LLVM_COV_BIN" show \
    "$primary_object" \
    "${llvm_cov_objects[@]}" \
    -instr-profile="$PROFDATA_FILE" \
    -ignore-filename-regex='.*/(test|_deps)/.*' \
    -format=html \
    -output-dir="$REPORT_DIR"

echo
echo "HTML coverage report: $REPORT_DIR/index.html"
