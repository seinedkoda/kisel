#!/usr/bin/env bash

SCRIPT_DIR=$(realpath "$(dirname "${BASH_SOURCE[0]}")")
cd ../..

flatpak-builder --force-clean --user --install flatpak-build "${SCRIPT_DIR}/io.github.seinedkoda.kisel.yml"
