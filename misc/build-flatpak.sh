#!/usr/bin/env bash

SCRIPT_DIR=$(realpath "$(dirname "${BASH_SOURCE[0]}")")
cd $SCRIPT_DIR

flatpak-builder --repo=repo --force-clean flatpak-build io.github.seinedkoda.kisel.yml && \
flatpak build-bundle repo kisel.flatpak io.github.seinedkoda.kisel
