# Kisel

Efficient launch of Windows programs

![screenshot](resources/screenshot.png)

## Highlights
- Currently uses [umu-launcher](https://github.com/Open-Wine-Components/umu-launcher) to run exe
- Compatibility with the Steam environment
- C++ code and minimal actions to run exe
- Ability to create desktop shortcuts instead of displaying the list of added applications
- Managing Prefixes and Compatibility Tools
- Support for downloading compatibility tools:
    - Proton-GE (Default)
    - Proton-CachyOS
- Configure the following parameters:
    - Runtime auto-update
    - MangoHud
    - Obs Vulkan Game Capture
    - Xalia
    - Wayland driver
    - Steam simulation
    - WOW64
- Available translations:
    - English
    - Русский

## Dependencies
- qt6-base: graphical interface
- [umu-launcher](https://github.com/Open-Wine-Components/umu-launcher): way to launch Windows applications
- icoutils: extracting icons

## Installation

### Arch Linux
[> Download the latest version of pkg.tar.zst](https://github.com/seinedkoda/kisel/releases/latest)

```bash
sudo pacman -U kisel-*.pkg.tar.zst
```

### Build from source using CMake

*Qt6 LinguistTools(qt6-tools) required*

```bash
git clone https://github.com/seinedkoda/kisel.git
cd kisel
cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build
```

## License
*GPL-3.0*