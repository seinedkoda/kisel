#!/usr/bin/env bash

SCRIPT_DIR=$(realpath "$(dirname "${BASH_SOURCE[0]}")")
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build"

cd "$PROJECT_DIR" || exit 1

cmake -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release && cmake --build "$BUILD_DIR"
