#!/usr/bin/env bash
set -e
mkdir -p build && cd build
cmake .. && cmake --build . -j
./batd -c ../conf/config.toml
