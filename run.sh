#!/usr/bin/env bash
set -e

mkdir -p build

cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

cmake --build build

./build/linux-resource-monitor

