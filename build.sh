#! /bin/bash
# too tired to run these all over again
rm -rf build
cmake -S . -B build
cmake --build build -j 8