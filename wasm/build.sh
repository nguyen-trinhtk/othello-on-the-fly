#!/bin/bash
# Builds the C++ engine for the web page, into web/engine/.
# Needs Emscripten (brew install emscripten).
#
# Makes two versions:
#   othello.js / .wasm          one thread, works on any website
#   othello-threads.js / .wasm  uses every CPU core, needs a cross-origin isolated page
set -e
cd "$(dirname "$0")/.."

SOURCES="
  wasm/web_engine.cpp
  src/ai.cpp
  src/board.cpp
  src/bitboard.cpp
  src/rules.cpp
  src/zobrist.cpp
  src/trans_table.cpp
  src/eval/mask_eval.cpp
  src/util/thread-pool.cpp
"

SETTINGS="
  -O3 -std=c++20
  -Iinclude -Iinclude/othello
  --post-js wasm/answer-page.js
  -sENVIRONMENT=web,worker
  -sEXPORTED_FUNCTIONS=_best_move
  -sEXPORTED_RUNTIME_METHODS=ccall
  -sALLOW_MEMORY_GROWTH -sMAXIMUM_MEMORY=1gb
"

mkdir -p web/engine

em++ $SOURCES $SETTINGS -o web/engine/othello.js

em++ $SOURCES $SETTINGS -pthread \
  -sPTHREAD_POOL_SIZE=navigator.hardwareConcurrency \
  -sMALLOC=mimalloc \
  -o web/engine/othello-threads.js

echo "Built web/engine/"
