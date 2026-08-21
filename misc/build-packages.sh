#!/usr/bin/env bash

SCRIPT_DIR=$(realpath "$(dirname "${BASH_SOURCE[0]}")")
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="${PROJECT_DIR}/build"
PACKAGES_DIR="${BUILD_DIR}/packages"

cd $SCRIPT_DIR
mkdir -p $BUILD_DIR
rm -fR $PACKAGES_DIR
mkdir -p $PACKAGES_DIR

echo "Building Pacman package..."
docker build \
    -f "./archlinux/build-pacman.Dockerfile" \
    -t kisel-pacman \
    "${PROJECT_DIR}"

docker run --rm -v "${PACKAGES_DIR}:/output" kisel-pacman \
sh -c "cp /home/builder/kisel-*.pacman /output/"


echo "Building DEB package..."
docker build \
    -f "./debian/build-deb.Dockerfile" \
    -t kisel-deb \
    "${PROJECT_DIR}"

docker run --rm -v "${PACKAGES_DIR}:/output" kisel-deb \
sh -c "cp /src/build/kisel-*.deb /output/"


echo "Building RPM package..."
docker build \
    -f "./fedora/build-rpm.Dockerfile" \
    -t kisel-rpm \
    "${PROJECT_DIR}"

docker run --rm -v "${PACKAGES_DIR}:/output" kisel-rpm \
sh -c "cp /src/build/kisel-*.rpm /output/"


echo "Building Flatpak package..."
cd $BUILD_DIR
flatpak-builder --repo=repo --force-clean flatpak-build "${SCRIPT_DIR}/flatpak/io.github.seinedkoda.kisel.yml" && \
flatpak build-bundle repo "${PACKAGES_DIR}/kisel.flatpak" io.github.seinedkoda.kisel


cd $PACKAGES_DIR
sudo chown ${USER}:${USER} ./kisel-*
echo "Packages built successfully:"
ls
