#!/usr/bin/env bash

SCRIPT_DIR=$(realpath "$(dirname "${BASH_SOURCE[0]}")")
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

cd $SCRIPT_DIR
mkdir -p build
rm -R packages && mkdir -p packages


echo "Building Pacman package..."
docker build \
    -f "./archlinux/build-pacman.Dockerfile" \
    -t kisel-pacman \
    "${PROJECT_DIR}"

docker run --rm -v "./packages:/output" kisel-pacman \
sh -c "cp /home/builder/kisel-*.pacman /output/" &&
sudo chown ${USER}:${USER} ./packages/kisel-*.pacman


echo "Building DEB package..."
docker build \
    -f "./debian/build-deb.Dockerfile" \
    -t kisel-deb \
    "${PROJECT_DIR}"

docker run --rm -v "./packages:/output" kisel-deb \
sh -c "cp /src/build/kisel-*.deb /output/" &&
sudo chown ${USER}:${USER} ./packages/kisel-*.deb


echo "Building RPM package..."
docker build \
    -f "./fedora/build-rpm.Dockerfile" \
    -t kisel-rpm \
    "${PROJECT_DIR}"

docker run --rm -v "./packages:/output" kisel-rpm \
sh -c "cp /src/build/kisel-*.rpm /output/" &&
sudo chown ${USER}:${USER} ./packages/kisel-*.rpm


echo "Building Flatpak package..."
cd ./build
flatpak-builder --repo=repo --force-clean flatpak-build ../flatpak/io.github.seinedkoda.kisel.yml && \
flatpak build-bundle repo ../packages/kisel.flatpak io.github.seinedkoda.kisel
cd $SCRIPT_DIR


echo "Packages built successfully:"
ls packages
