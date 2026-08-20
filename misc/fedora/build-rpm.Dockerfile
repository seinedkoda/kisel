FROM fedora:44

RUN dnf install --disablerepo=fedora-cisco-openh264 -y \
    rpm-build \
    cmake \
    gcc-c++ \
    ninja-build \
    qt6-qtbase-devel \
    qt6-qttools-devel

COPY . /src
WORKDIR /src

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release -G Ninja && \
    cmake --build build -j $(nproc) && \
    cd build && cpack -G RPM
