#!/usr/bin/env bash

SCRIPT_DIR=$(realpath "$(dirname "${BASH_SOURCE[0]}")")
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

bash $SCRIPT_DIR/build.sh && cd $PROJECT_DIR/build && cpack -G RPM
