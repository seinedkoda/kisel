FROM debian:trixie

RUN apt update && apt install -y \
    cmake \
    g++ \
    ninja-build \
    qt6-base-dev \
    qt6-tools-dev

COPY . /src
WORKDIR /src

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release -G Ninja && \
    cmake --build build -j $(nproc) && \
    cd build && cpack -G DEB
