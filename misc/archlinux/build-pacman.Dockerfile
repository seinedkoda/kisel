FROM archlinux:latest

RUN pacman -Syu --noconfirm && \
    pacman -S --noconfirm \
    base-devel \
    cmake \
    ninja \
    gcc \
    qt6-base \
    qt6-tools \
    qt6-declarative \
    icoutils \
    git \
    sudo

# Root is forbidden in makepkg
RUN useradd -m builder && \
    echo "builder ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

USER builder
COPY misc/archlinux/PKGBUILD /home/builder
WORKDIR /home/builder

RUN makepkg -f

RUN for file in kisel-*.pkg.tar.zst; do \
        mv "$file" "${file%.pkg.tar.zst}.pacman"; \
    done
