#!/usr/bin/env bash

SCRIPT_DIR=$(realpath "$(dirname "${BASH_SOURCE[0]}")")
cd $SCRIPT_DIR

flatpak-builder --force-clean --user --install flatpak-build io.github.seinedkoda.kisel.yml
